#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

FILE *file;
char *word;
size_t records;
size_t num_threads;
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void *thread_func(void *arg) {
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
                printf("found %s, tid: %ld record_id: %d\n", word, gettid(), r[i].id);
                fflush(stdout);

                return NULL;
            }
        }

        pthread_mutex_lock(&mutex);
        rb = fread(r, sizeof(record_t), (size_t) records, file);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void help() {
    printf("usage: zad1X num_threads filename records word");
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        help();
        return 0;
    }

    num_threads = (size_t) atoi(argv[1]);
    char *filename = argv[2];
    records = (size_t) atoi(argv[3]);
    word = argv[4];

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

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], &attr, thread_func, NULL) != 0) {
            fprintf(stderr, "pthread_create\n");
            free(threads);
            return 1;
        }
    }

    pthread_exit(0);
}
