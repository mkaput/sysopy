#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MALLOC_CHK(VAR) if ((VAR) == NULL) { \
    perror("Janusze RAMu detected!"); \
    abort(); \
}

#define SEEK_CHK(RESULT) if ((RESULT) == -1) { \
    perror("failed to seek"); \
    abort(); \
}

void help() {
    printf("usage:\n"
                   "\tzad1 generate <file name> <records> <record size>\n"
                   "\tzad1 shuffle <sys|lib> <file name> <records> <record size>\n"
                   "\tzad1 sort <sys|lib> <file name> <records> <record size>\n");
}

void generate(const char *file_name, size_t records, size_t size) {
    FILE *rnd = fopen("/dev/urandom", "r");
    if (rnd == NULL) {
        perror("Wtf? Cannot open /dev/urandom! What kind of world do you live in? "
                       "Where SysOpy takes few minutes to complete?");
        abort();
    }

    FILE *file = fopen(file_name, "w");
    if (file == NULL) {
        perror("cannot open output file!");
        abort();
    }

    void *buffer = calloc(records, size);
    MALLOC_CHK(buffer);

    for (int i = 0; i < 16; ++i) {
        size_t bts = fread(buffer, size, records, rnd);
        if (bts > 0) {
            fwrite(buffer, size, bts, file);
        }
    }

    free(buffer);

    fclose(file);
    fclose(rnd);
}

void shuffle_sys(const char *file_name, size_t records, size_t size) {
    int fd = open(file_name, O_RDWR);
    if (fd == -1) {
        perror("cannot open output file!");
        abort();
    }

    void *buffer_a = malloc(size);
    MALLOC_CHK(buffer_a);

    void *buffer_b = malloc(size);
    MALLOC_CHK(buffer_b);

    for (size_t i = 0; i < records - 1; ++i) {
        size_t j = rand() % records;

        SEEK_CHK(lseek(fd, i * size, SEEK_SET));
        ssize_t bts_a = read(fd, buffer_a, size);
        SEEK_CHK(lseek(fd, j * size, SEEK_SET));
        ssize_t bts_b = read(fd, buffer_b, size);

        SEEK_CHK(lseek(fd, i * size, SEEK_SET));
        write(fd, buffer_b, (size_t) bts_b);
        SEEK_CHK(lseek(fd, j * size, SEEK_SET));
        write(fd, buffer_a, (size_t) bts_a);
    }

    free(buffer_b);
    free(buffer_a);

    close(fd);
}

void shuffle_lib(const char *file_name, size_t records, size_t size) {
    FILE *file = fopen(file_name, "r+");
    if (file == NULL) {
        perror("cannot open output file!");
        abort();
    }

    void *buffer_a = malloc(size);
    MALLOC_CHK(buffer_a);

    void *buffer_b = malloc(size);
    MALLOC_CHK(buffer_b);

    for (size_t i = 0; i < records - 1; ++i) {
        size_t j = rand() % records;

        SEEK_CHK(fseek(file, i * size, SEEK_SET));
        size_t bts_a = fread(buffer_a, size, 1, file);
        SEEK_CHK(fseek(file, j * size, SEEK_SET));
        size_t bts_b = fread(buffer_b, size, 1, file);

        SEEK_CHK(fseek(file, i * size, SEEK_SET));
        fwrite(buffer_b, bts_b, 1, file);
        SEEK_CHK(fseek(file, j * size, SEEK_SET));
        fwrite(buffer_a, bts_a, 1, file);
    }

    free(buffer_b);
    free(buffer_a);
    fclose(file);
}

void sort_sys(const char *file_name, size_t records, size_t size) {
    int fd = open(file_name, O_RDWR);
    if (fd == -1) {
        perror("cannot open output file!");
        abort();
    }

    void *buffer_a = malloc(size);
    MALLOC_CHK(buffer_a);

    void *buffer_b = malloc(size);
    MALLOC_CHK(buffer_b);

    bool swapped;
    do {
        swapped = false;

        for (size_t i = 0; i < records - 1; ++i) {
            size_t j = i + 1;

            SEEK_CHK(lseek(fd, i * size, SEEK_SET));
            ssize_t bts_a = read(fd, buffer_a, size);
            SEEK_CHK(lseek(fd, j * size, SEEK_SET));
            ssize_t bts_b = read(fd, buffer_b, size);

            unsigned char key_a = ((unsigned char *) buffer_a)[0];
            unsigned char key_b = ((unsigned char *) buffer_b)[0];

            if (key_a > key_b) {
                SEEK_CHK(lseek(fd, i * size, SEEK_SET));
                write(fd, buffer_b, (size_t) bts_b);
                SEEK_CHK(lseek(fd, j * size, SEEK_SET));
                write(fd, buffer_a, (size_t) bts_a);

                swapped = true;
            }
        }
    } while (swapped);

    free(buffer_b);
    free(buffer_a);
    close(fd);
}

void sort_lib(const char *file_name, size_t records, size_t size) {
    FILE *file = fopen(file_name, "r+");
    if (file == NULL) {
        perror("cannot open output file!");
        abort();
    }

    void *buffer_a = malloc(size);
    MALLOC_CHK(buffer_a);

    void *buffer_b = malloc(size);
    MALLOC_CHK(buffer_b);

    bool swapped;
    do {
        swapped = false;

        for (size_t i = 0; i < records - 1; ++i) {
            size_t j = i + 1;

            SEEK_CHK(fseek(file, i * size, SEEK_SET));
            size_t bts_a = fread(buffer_a, size, 1, file);
            SEEK_CHK(fseek(file, j * size, SEEK_SET));
            size_t bts_b = fread(buffer_b, size, 1, file);

            unsigned char key_a = ((unsigned char *) buffer_a)[0];
            unsigned char key_b = ((unsigned char *) buffer_b)[0];

            if (key_a > key_b) {
                SEEK_CHK(fseek(file, i * size, SEEK_SET));
                fwrite(buffer_b, bts_b, 1, file);
                SEEK_CHK(fseek(file, j * size, SEEK_SET));
                fwrite(buffer_a, bts_a, 1, file);

                swapped = true;
            }
        }
    } while (swapped);

    free(buffer_b);
    free(buffer_a);
    fclose(file);
}

int main(int argc, char *argv[]) {
    srand((unsigned int) time(NULL));

    const char *task = argv[1];
    if (argc == 5 && strcmp(task, "generate") == 0) {
        const char *file_name = argv[2];
        size_t records = (size_t) atoi(argv[3]);
        size_t record_size = (size_t) atoi(argv[4]);

        generate(file_name, records, record_size);
    } else if (argc == 6) {
        const char *method = argv[2];
        const char *file_name = argv[3];
        size_t records = (size_t) atoi(argv[4]);
        size_t record_size = (size_t) atoi(argv[5]);

        if (strcmp(task, "shuffle") == 0) {
            if (strcmp(method, "sys") == 0) {
                shuffle_sys(file_name, records, record_size);
            } else if (strcmp(method, "lib") == 0) {
                shuffle_lib(file_name, records, record_size);
            }
        } else if (strcmp(task, "sort") == 0) {
            if (strcmp(method, "sys") == 0) {
                sort_sys(file_name, records, record_size);
            } else if (strcmp(method, "lib") == 0) {
                sort_lib(file_name, records, record_size);
            }
        }
    } else {
        help();
    }

    return 0;
}
