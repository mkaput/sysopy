#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
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

typedef struct {
    queue_t queue;
} state_t;

static size_t K;
static size_t S;

static state_t *state = NULL;
static int memid = -1;

static sem_t *sem_pigeon = NULL;
static sem_t *sem_queue = NULL;
static sem_t *sem_free_seats = NULL;

void state_init(state_t *state) {
    queue_init(&state->queue);
}

bool init_semaphores() {
    sem_pigeon = sem_open("/pigeon", O_CREAT | O_EXCL | O_RDWR, 0777, 0);
    if (sem_pigeon == SEM_FAILED) {
        return false;
    }

    sem_queue = sem_open("/queue", O_CREAT | O_EXCL | O_RDWR, 0777, 1);
    if (sem_queue == SEM_FAILED) {
        sem_close(sem_pigeon);
        return false;
    }

    sem_free_seats = sem_open("/free_seats", O_CREAT | O_EXCL | O_RDWR, 0777, QUEUE_SIZE);
    if (sem_free_seats == SEM_FAILED) {
        sem_close(sem_pigeon);
        sem_close(sem_queue);
        return false;
    }

    return true;
}

bool get_semaphores() {
    sem_pigeon = sem_open("/pigeon", O_RDWR, 0777, 0);
    if (sem_pigeon == SEM_FAILED) {
        return false;
    }

    sem_queue = sem_open("/queue", O_RDWR, 0777, 1);
    if (sem_queue == SEM_FAILED) {
        sem_close(sem_pigeon);
        return false;
    }

    sem_free_seats = sem_open("/free_seats", O_RDWR, 0777, QUEUE_SIZE);
    if (sem_free_seats == SEM_FAILED) {
        sem_close(sem_pigeon);
        sem_close(sem_queue);
        return false;
    }

    return true;
}

void free_semaphores() {
    if (sem_pigeon != NULL) sem_close(sem_pigeon);
    if (sem_queue != NULL) sem_close(sem_queue);
    if (sem_free_seats != NULL) sem_close(sem_free_seats);

    sem_unlink("/pigeon");
    sem_unlink("/queue");
    sem_unlink("/free_seats");
}

int shared_state_init(state_t **pstate) {
    int memid;
    if ((memid = shm_open("/abrakadabra", O_CREAT | O_EXCL | O_RDWR, 0777)) == -1) {
        *pstate = NULL;
        return -1;
    }

    if (ftruncate(memid, sizeof(state_t)) == -1) {
        *pstate = NULL;
        shm_unlink("/abrakadabra");
        return -1;
    }

    void *addr = mmap(NULL, sizeof(state_t), PROT_READ | PROT_WRITE, MAP_SHARED, memid, 0);
    if (addr == (void *) -1) {
        *pstate = NULL;
        shm_unlink("/abrakadabra");
        return -1;
    }

    state_init(addr);

    *pstate = addr;
    return memid;
}

void shared_state_free(state_t *state) {
    munmap(state, sizeof(state_t));
    shm_unlink("/abrakadabra");
}

state_t *shared_state_acquire() {
    int memid;
    if ((memid = shm_open("/abrakadabra", O_RDWR, 0777)) == -1) {
        return NULL;
    }

    void *addr = mmap(NULL, sizeof(state_t), PROT_READ | PROT_WRITE, MAP_SHARED, memid, 0);
    return addr == (void *) -1 ? NULL : addr;
}

int shared_state_release(state_t *state) {
    return munmap(state, sizeof(state_t));
}

/*
 * Bachory
 */

int kid_wait_for_enqueuing() {
    if (sem_wait(sem_queue) == -1) {
        return -1;
    }

    printf("[%.4lf] %d: czekam w kolejce\n", now(), getpid());

    if (!queue_push(&state->queue, getpid())) {
        sem_post(sem_queue);
        return -1;
    }

    if (sem_wait(sem_free_seats) == -1) {
        return -1;
    }

    if (sem_post(sem_queue) == -1) {
        return -1;
    }

    if (sem_post(sem_pigeon) == -1) {
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
    if ((e = sigwait(&sigset, &sig)) != 0) {
        errno = e;
        return -1;
    }

    printf("[%.4lf] %d: ide na strzyzenie\n", now(), getpid());
    return 0;
}

void kid_atexit() {
    if (sem_queue != NULL) sem_close(sem_queue);
    if (sem_free_seats != NULL) sem_close(sem_free_seats);
    if (sem_pigeon != NULL) sem_close(sem_pigeon);
}

int kid_main() {
    atexit(kid_atexit);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    if (!get_semaphores()) {
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

        if (kid_wait_for_being_pooped() == -1) {
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

    if (sem_wait(sem_pigeon) == -1) {
        return -1;
    }

    if (sem_wait(sem_queue) == -1) {
        return -1;
    }

    pid_t pid;
    if (!queue_pop(&state->queue, &pid)) {
        return -1;
    }

    if (sem_post(sem_free_seats) == -1) {
        return -1;
    }

    if (sem_post(sem_queue) == -1) {
        return -1;
    }

    printf("[%.4lf] klient!\n", now());
    fflush(stdout);

    return pid;
}

void pigeon_end_pooping(pid_t kid) {
    kill(kid, SIGRTMIN);
    printf("[%.4lf] ostrzyglem %d\n", now(), kid);
    fflush(stdout);
}

void pigeon_atexit() {
    free_semaphores();
    shared_state_free(state);

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

    if ((memid = shared_state_init(&state)) < 0) {
        perror("failed to initialize shared state");
        return 1;
    }

    if (!init_semaphores()) {
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
