#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

struct timespec starttsp;

double now() {
    struct timespec rtsp;
    clock_gettime(CLOCK_MONOTONIC, &rtsp);

    long nanodiff = rtsp.tv_nsec - starttsp.tv_nsec;
    rtsp.tv_sec = rtsp.tv_sec - starttsp.tv_sec + nanodiff / 1000000000L;
    rtsp.tv_nsec = nanodiff % 1000000000L;

    return rtsp.tv_sec * 1000.0 + rtsp.tv_nsec / 1000000.0;
}

/*
 * Kolejka
 */

#define QUEUE_SIZE 5

typedef struct {
    size_t head;
    size_t tail;
    size_t size;
    int data[QUEUE_SIZE];
} queue_t;


void queue_init(queue_t *queue) {
    queue->head = 0;
    queue->tail = 0;
    queue->size = 0;
}

bool queue_push(queue_t *queue, int value) {
    if (queue->size == QUEUE_SIZE) {
        errno = 2048;
        return false;
    }

    queue->data[queue->tail] = value;
    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    queue->size++;
    return true;
}

bool queue_pop(queue_t *queue, int *value) {
    if (queue->size == 0) {
        errno = 2049;
        return false;
    }

    *value = queue->data[queue->head];
    queue->head = (queue->head + 1) % QUEUE_SIZE;
    queue->size--;
    return true;
}

bool queue_empty(queue_t *queue) {
    return queue->size == 0;
}

bool queue_full(queue_t *queue) {
    return queue->size == QUEUE_SIZE;
}

/*
 * Stan
 */

#define SEM_PIGEON    0
#define SEM_QUEUE     1
#define SEM_FREESEATS 2

typedef struct {
    queue_t queue;
} state_t;

static size_t K;
static size_t S;
static key_t KEY;

static state_t *state = NULL;
static int memid = -1;
static int semid = -1;

void state_init(state_t *state) {
    queue_init(&state->queue);
}

int init_semaphores() {
    int semid;
    if ((semid = semget(KEY, 3, 0777 | IPC_CREAT | IPC_EXCL)) == -1) {
        return -1;
    }

    unsigned short values[3];
    values[SEM_PIGEON] = 0;
    values[SEM_QUEUE] = 1;
    values[SEM_FREESEATS] = QUEUE_SIZE;
    if (semctl(semid, 0, SETALL, values) == -1) {
        semctl(semid, 0, IPC_RMID);
        return -1;
    }

    return semid;
}

int get_semaphores() {
    return semget(KEY, 0, 0777);
}

int free_semaphores(int semid) {
    return semctl(semid, 0, IPC_RMID);
}

int shared_state_init(state_t **pstate) {
    int memid;
    if ((memid = shmget(KEY, sizeof(state_t), 0777 | IPC_CREAT | IPC_EXCL)) == -1) {
        *pstate = NULL;
        return -1;
    }

    void *addr = shmat(memid, NULL, 0);
    if (addr == (void *) -1) {
        *pstate = NULL;
        shmctl(memid, IPC_RMID, NULL);
        return -1;
    }

    state_init(addr);

    *pstate = addr;
    return memid;
}

void shared_state_free(int memid, state_t *state) {
    shmdt(state);
    shmctl(memid, IPC_RMID, NULL);
}

state_t *shared_state_acquire() {
    int memid;
    if ((memid = shmget(KEY, sizeof(state_t), 0777)) == -1) {
        return NULL;
    }

    void *addr = shmat(memid, NULL, 0);
    return addr == (void *) -1 ? NULL : addr;
}

int shared_state_release(state_t *state) {
    return shmdt(state);
}

/*
 * Bachory
 */

int kid_wait_for_enqueuing() {
    struct sembuf sops2[2];
    sops2[0].sem_num = SEM_FREESEATS;
    sops2[0].sem_op = -1;
    sops2[0].sem_flg = 0;
    sops2[1].sem_num = SEM_QUEUE;
    sops2[1].sem_op = -1;
    sops2[1].sem_flg = 0;
    if (semop(semid, sops2, 2) == -1) {
        return -1;
    }

    printf("[%.4lf] %d: czekam w kolejce\n", now(), getpid());

    if (!queue_push(&state->queue, getpid())) {
        return -1;
    }

    sops2[0].sem_num = SEM_PIGEON;
    sops2[0].sem_op = 1;
    sops2[0].sem_flg = 0;
    sops2[1].sem_num = SEM_QUEUE;
    sops2[1].sem_op = 1;
    sops2[1].sem_flg = 0;
    if (semop(semid, sops2, 2) == -1) {
        return -1;
    }

    return 0;
}

int kid_wait_for_being_pooped() {
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGRTMIN);

    int sig;
    int e;
    if((e = sigwait(&sigset, &sig)) != 0) {
        errno = e;
        return -1;
    }

    printf("[%.4lf] %d: ide na strzyzenie\n", now(), getpid());
    return 0;
}

int kid_main() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    semid = get_semaphores();
    if (semid == -1) {
        perror("failed to get semaphores in kid");
        return 1;
    }

    memid = -1;
    state = shared_state_acquire();
    if (state == NULL) {
        perror("failed to acquire shared state in kid");
        return 1;
    }

    for (int i = 0; i < S; i++) {
        if (kid_wait_for_enqueuing() == -1) {
            perror("nie powiodlo sie kolejkowanie");
            return 1;
        }

        if(kid_wait_for_being_pooped() == -1) {
            perror("nie powiodlo sie czekanie na strzyzenie");
            return 1;
        }
    }

    shared_state_release(state);
    return 0;
}

/*
 * Gołąb
 */

pid_t *pids = NULL;
int spawned_kids = 0;

pid_t pigeon_wait_for_kid() {
    printf("[%.4lf] golibroda spi\n", now());
    fflush(stdout);

    struct sembuf sops[1];
    sops[0].sem_num = SEM_PIGEON;
    sops[0].sem_op = -1;
    sops[0].sem_flg = 0;
    if (semop(semid, sops, 1) == -1) {
        return -1;
    }

    sops[0].sem_num = SEM_QUEUE;
    sops[0].sem_op = -1;
    sops[0].sem_flg = 0;
    if (semop(semid, sops, 1) == -1) {
        return -1;
    }

    pid_t pid;
    if (!queue_pop(&state->queue, &pid)) {
        return -1;
    }

    struct sembuf sops2[2];
    sops2[0].sem_num = SEM_FREESEATS;
    sops2[0].sem_op = 1;
    sops2[0].sem_flg = 0;
    sops2[1].sem_num = SEM_QUEUE;
    sops2[1].sem_op = 1;
    sops2[1].sem_flg = 0;
    if (semop(semid, sops2, 2) == -1) {
        return -1;
    }

    printf("[%.4lf] klient!\n", now());
    fflush(stdout);

    return pid;
}

void pigeon_end_pooping(pid_t kid) {
    sleep(2);
    kill(kid, SIGRTMIN);
    printf("[%.4lf] ostrzyglem %d\n", now(), kid);
    fflush(stdout);
}

void pigeon_atexit() {
    if (semid != -1) free_semaphores(semid);
    if (memid != -1 && state != NULL) shared_state_free(memid, state);

    if (pids != NULL) {
        for (int i = 0; i < K; ++i) {
            if (pids[i] > 0) {
                kill(pids[i], SIGINT);
                waitpid(pids[i], NULL, 0);
            }
        }

        free(pids);
    }
}

int main(int argc, char *argv[]) {
    atexit(pigeon_atexit);

    clock_gettime(CLOCK_MONOTONIC, &starttsp);

    if (argc != 3) {
        printf("usage: zad1 K S\n");
        printf("queue size is fixed to: %d\n", QUEUE_SIZE);
        return 0;
    }

    K = (size_t) atoi(argv[1]);
    S = (size_t) atoi(argv[2]);

    KEY = ftok(argv[0], getpid());
    if (KEY == -1) {
        perror("failed generating key");
        return 1;
    }

    if ((memid = shared_state_init(&state)) < 0) {
        perror("failed to initialize shared state");
        return 1;
    }

    semid = init_semaphores();
    if (semid < 0) {
        perror("failed to init semaphores");
        return 1;
    }

    pids = calloc(K, sizeof(pid_t));
    if (pids == NULL) {
        perror("failed to allocate pid table");
        return 1;
    }

    memset(pids, 0, K * sizeof(pid_t));

    for (size_t i = 0; i < K; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            int result = kid_main();
            _exit(result);
        } else if (pids[i] > 0) {
            spawned_kids++;
        } else {
            perror("fork failed");
        }
    }

    while (spawned_kids > 0) {
        {
            pid_t pid = pigeon_wait_for_kid();
            if (pid == -1) {
                perror("czekanie na klienta nie wyszlo");
                return 1;
            }

            pigeon_end_pooping(pid);
        }

        while (true) {
            pid_t pid = waitpid(-1, NULL, WNOHANG);
            if (pid == -1 && errno != ECHILD) {
                perror("failed checking kids");
                return 1;
            }

            if (pid > 0) {
                spawned_kids--;
            } else {
                break;
            }
        }
    }

    return 0;
}
