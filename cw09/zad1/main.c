#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>

static long gettid() {
    return syscall(SYS_gettid);
}

#define NUM_WRITERS 5
#define NUM_READERS 10
#define DATA_SIZE 100

static bool verbose = false;
static sem_t sem_writer;
static sem_t sem_reader;
static pthread_t writers[NUM_WRITERS];
static pthread_t readers[NUM_READERS];
static int data[DATA_SIZE];

void help() {
    printf("zad1 [-l]\n");
}

void *writer(void *_arg) {
    for (;;) {
        sem_wait(&sem_writer);
        int semval;
        sem_getvalue(&sem_reader, &semval);
        while (semval != 0);

        size_t n = (size_t) (rand() % DATA_SIZE);

        for (size_t i = 0; i < n; i++) {
            size_t j = (size_t) (rand() % DATA_SIZE);
            int x = rand();
            data[j] = x;

            if (verbose) {
                printf("[%ld] wrote %ld -> %d\n", gettid(), j, x);
                fflush(stdout);
            }
        }

        sem_post(&sem_writer);

        sleep(1);
    }

    return NULL;
}

void *reader(void *arg) {
    int divisor = *(int *) arg;

    for (;;) {
        int semval;
        sem_getvalue(&sem_writer, &semval);
        if (semval == 0) continue;

        sem_post(&sem_reader);

        for (size_t i = 0; i < DATA_SIZE; i++) {
            if (data[i] % divisor == 0 && verbose) {
                printf("[%ld] found %ld -> %d (mod %d)\n", gettid(), i, data[i], divisor);
            }
        }

        sem_wait(&sem_reader);

        sleep(1);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 1 && argc != 2) {
        help();
        return 0;
    }

    if (argc == 2 && strcmp("-l", argv[1]) == 0) {
        verbose = true;
    }

    if (sem_init(&sem_writer, 0, 1) == -1) {
        perror("failed initializing semaphore");
        return 1;
    }

    if (sem_init(&sem_reader, 0, 0) == -1) {
        perror("failed initializing semaphore");
        sem_destroy(&sem_writer);
        return 1;
    }

    srand((unsigned int) time(NULL));

    for (size_t i = 0; i < DATA_SIZE; i++) {
        data[i] = rand();
    }

    for (size_t i = 0; i < NUM_WRITERS; i++) {
        if ((errno = pthread_create(&writers[i], NULL, writer, NULL)) != 0) {
            perror("failed creating writer");
            return 1;
        }
    }

    for (size_t i = 0; i < NUM_READERS; i++) {
        int *divisor = malloc(sizeof(int));
        if (divisor == NULL) {
            perror("failed to allocate divisor cell");
            return 1;
        }

        *divisor = (rand() % 31) + 1;
        if ((errno = pthread_create(&readers[i], NULL, reader, divisor)) != 0) {
            perror("failed creating reader");
            return 1;
        }
    }

    for (size_t i = 0; i < NUM_WRITERS; i++) {
        if ((errno = pthread_join(writers[i], NULL)) != 0) {
            perror("failed joining writer");
            return 1;
        }
    }

    for (size_t i = 0; i < NUM_READERS; i++) {
        if ((errno = pthread_join(readers[i], NULL)) != 0) {
            perror("failed joining reader");
            return 1;
        }
    }

    sem_destroy(&sem_writer);
    sem_destroy(&sem_reader);

    return 0;
}
