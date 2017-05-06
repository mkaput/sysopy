#define _DEFAULT_SOURCE
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <zconf.h>
#include <stdbool.h>

#define NUM_THREADS 3

long gettid() {
    return syscall(SYS_gettid);
}

bool wait = true;

static void *good_boy(void *arg) {
    while (wait);

    for (int i = 0; i < 5; i++) {
        printf("tocze sie %ld\n", gettid());
        sleep(1);
    }

    return NULL;
}

static void *bad_boy(void *arg) {
    while (wait);

    sleep(2);
    int frobnicator = 1 / 0;

    return NULL;
}

void sighandler(int signum) {
    printf("pid %d, tid %ld, sig %d\n", getpid(), gettid(), signum);
}

int main(int argc, char *argv[]) {
    signal(SIGFPE, sighandler);

    pthread_t *threads = calloc(NUM_THREADS, sizeof(pthread_t));
    if (threads == NULL) {
        perror("malloc failed");
        return 1;
    }

    for (int i = 0; i < NUM_THREADS - 1; i++) {
        if (pthread_create(&threads[i], NULL, good_boy, NULL) != 0) {
            fprintf(stderr, "pthread_create\n");
            free(threads);
            return 1;
        }
    }

    if (pthread_create(&threads[NUM_THREADS - 1], NULL, bad_boy, NULL) != 0) {
        fprintf(stderr, "pthread_create\n");
        free(threads);
        return 1;
    }

    wait = false;

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "pthread_join\n");
            free(threads);
            return 1;
        }
    }

    return 0;
}
