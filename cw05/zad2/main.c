#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <complex.h>
#include <math.h>
#include <time.h>

#ifndef CMPLX
#define CMPLX(x, y) ((double complex)((double)(x) + I * (double)(y)))
#endif

void help() {
    printf("usage:\n"
                   "\tmaster mode: zad2 <pipe path> R\n"
                   "\tslave  mode: zad2 <pipe path> N K\n");
}

int master_main(int argc, char *argv[]) {
    const char *const DATA_PATH = "data";
    const char *pipe_path = argv[1];
    size_t r = (size_t) atoi(argv[2]);

    if (mkfifo(pipe_path, 0777) != 0) {
        perror("failed to create named pipe");
        exit(1);
    }

    printf("master: waiting for data from slaves\n");
    fflush(stdout);

    FILE *pipe_file = fopen(pipe_path, "r");
    if (pipe_file == NULL) {
        perror("failed to open named pipe");
        remove(pipe_path);
        exit(1);
    }

    int **arr = calloc(r, sizeof(int *));
    for (size_t i = 0; i < r; ++i) {
        arr[i] = calloc(r, sizeof(int));
        memset(arr[i], 0, r * sizeof(int));
    }

    double re;
    double im;
    int iters;
    while (fscanf(pipe_file, "%lf %lf %d", &re, &im, &iters) != EOF) {
        size_t x = (size_t) lround((2.0 + re) / 3.0 * (r - 1)) % r;
        size_t y = (size_t) lround((1.0 + im) / 2.0 * (r - 1)) % r;
        arr[y][x] = iters;
    }

    fclose(pipe_file);
    remove(pipe_path);

    FILE *data_file = fopen(DATA_PATH, "w");
    if (data_file == NULL) {
        perror("failed to create data file");
        for (size_t i = 0; i < r; ++i) free(arr[i]);
        free(arr);
        exit(1);
    }

    for (size_t i = 0; i < r; ++i) {
        for (size_t j = 0; j < r; ++j) {
            fprintf(data_file, "%ld %ld %d\n", j, i, arr[i][j]);
        }
    }

    for (size_t i = 0; i < r; ++i) free(arr[i]);
    free(arr);

    if (fclose(data_file) != 0) {
        perror("failed to close data file");
        exit(1);
    }

    FILE *gnuplot = popen("gnuplot", "w");
    if (gnuplot == NULL) {
        perror("failed to start gnuplot");
        exit(1);
    }

    fprintf(gnuplot, "set view map\n");
    fprintf(gnuplot, "set xrange [0:%ld]\n", r);
    fprintf(gnuplot, "set yrange [0:%ld]\n", r);
    fprintf(gnuplot, "plot '%s' with image\n", DATA_PATH);
    fflush(gnuplot);

    printf("Type any key to exit...\n");
    fflush(stdout);
    getc(stdin);
    if (pclose(gnuplot) == -1) {
        perror("failed to close gnuplot");
    }

    remove(DATA_PATH);

    return 0;
}

int iters(complex double c, int k) {
    int i = 0;
    double complex z = 0;

    while (i < k && cabs(z) < 2) {
        z = cpow(z, 2) + c;
        i++;
    }

    return i;
}

int slave_main(int argc, char *argv[]) {
    srand((unsigned int) time(NULL));

    const char *pipe_path = argv[1];
    int n = atoi(argv[2]);
    int k = atoi(argv[3]);

    FILE *pipe_file = fopen(pipe_path, "w");
    if (pipe_file == NULL) {
        perror("failed to open named pipe");
        exit(1);
    }

    for (int trial = 0; trial < n; ++trial) {
        double re = (double) rand() * 3.0 / (double) RAND_MAX - 2.0;
        double im = (double) rand() * 2.0 / (double) RAND_MAX - 1.0;
        double complex c = CMPLX(re, im);
        fprintf(pipe_file, "%lf %lf %d\n", re, im, iters(c, k));
        fflush(pipe_file);
    }

    fclose(pipe_file);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 3) {
        master_main(argc, argv);
    } else if (argc == 4) {
        slave_main(argc, argv);
    } else {
        help();
        return 0;
    }
}
