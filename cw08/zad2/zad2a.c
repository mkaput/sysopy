#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <zconf.h>
#include <signal.h>
#include <stdbool.h>

FILE *file;
char *word;
size_t records;
size_t num_threads;
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool wait = true;
int testno;
int signo;

void sighandler(int signum) {
    printf("PID %d, TID %ld, signal %d\n", getpid(), gettid(), signum);
}

static void *thread_func(void *arg) {
    if (testno == 4) {
        signal(signo, SIG_IGN);
    } else if (testno == 3 || testno == 5) {
        signal(signo, sighandler);
    }

    while (wait);

    record_t *r = calloc(records, sizeof(record_t));
    if (r == NULL) {
        perror("malloc in threads");
        exit(1);
    }

    pthread_mutex_lock(&mutex);
    size_t rb = fread(r, sizeof(record_t), (size_t) records, file);
    pthread_mutex_unlock(&mutex);

    while (rb > 0) {
        for (int i = 0; i < rb; i++) {
            if (strstr(r[i].text, word) != NULL) {
                printf("found %s, tid: %ld record_id: %d\n", word, gettid(), r->id);
                fflush(stdout);
                return NULL;
            }

            sleep(1);
        }

        pthread_mutex_lock(&mutex);
        rb = fread(r, sizeof(record_t), (size_t) records, file);
        pthread_mutex_unlock(&mutex);
    }

    printf("tid: %ld not found the word\n", gettid());
    fflush(stdout);

    return NULL;
}

void help() {
    printf("usage: zad2X num_threads filename records word testno signo");
}

int main(int argc, char *argv[]) {
    if (argc < 7) {
        help();
        return 0;
    }

    num_threads = (size_t) atoi(argv[1]);
    char *filename = argv[2];
    records = (size_t) atoi(argv[3]);
    word = argv[4];
    testno = atoi(argv[5]);
    signo = atoi(argv[6]);

    if (testno == 2) {
        signal(signo, SIG_IGN);
    } else if (testno == 3) {
        signal(signo, sighandler);
    }

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("file");
        return 1;
    }

    threads = calloc(num_threads, sizeof(pthread_t));
    if (threads == NULL) {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, thread_func, NULL) != 0) {
            fprintf(stderr, "pthread_create\n");
            free(threads);
            return 1;
        }
    }

    wait = false;

    if (testno == 1 || testno == 2 || testno == 3) {
        printf("sending signal %d\n", signo);
        kill(getpid(), signo);
    } else if (testno == 4 || testno == 5) {
        printf("sending signal %d to last thread\n", signo);
        pthread_kill(threads[num_threads - 1], signo);
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "pthread_join\n");
            free(threads);
            return 1;
        }
    }

    free(threads);
    return 0;
}
