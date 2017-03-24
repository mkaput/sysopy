#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

void help() {
    printf("usage: zad2 <script file> <cpu limit in seconds> <memory limit in MB>\n");
}

void eval_env(char *line) {
    const char *name = strtok(line, " \t\n\r");
    const char *value = strtok(NULL, "\n\r");
    if (value != NULL) {
        if (setenv(name, value, 0) == -1) {
            perror("failed to set env var");
        }
    } else {
        if (unsetenv(name) == -1) {
            perror("failed to unset env var");
        }
    }
}

double timeval_to_double(struct timeval tv) {
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void eval_run(char *line, rlim_t cpu_limit, rlim_t mem_limit) {
#define MAX_ARGS 32

    size_t argc = 0;
    char *argv[MAX_ARGS + 2];
    memset(argv, NULL, (MAX_ARGS + 2) * sizeof(char *));

    argv[argc++] = strtok(line, " \t\n\r");

    if(argv[0] == NULL) {
        return;
    }

    char *current_arg = NULL;
    while ((current_arg = strtok(NULL, " \t\n\r")) != NULL) {
        if (argc == MAX_ARGS) {
            fprintf(stderr, "too many arguments!\n");
            exit(1);
        }

        argv[argc++] = current_arg;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // prestige
        struct rlimit rlim_cpu;
        rlim_cpu.rlim_cur = 1; // let's start spamming SIGXCPU after 1 second
        rlim_cpu.rlim_max = cpu_limit;
        if(setrlimit(RLIMIT_CPU, &rlim_cpu) == -1) {
            perror("setting cpu limit failed");
            exit(1);
        }

        struct rlimit rlim_mem;
        rlim_mem.rlim_cur = mem_limit/2;
        rlim_mem.rlim_max = mem_limit;
        if(setrlimit(RLIMIT_AS, &rlim_mem) == -1) {
            perror("setting mem limit failed");
            exit(1);
        }

        if (execvp(argv[0], argv) == -1) {
            perror("failed to execvp");
            exit(1);
        }
    } else if (pid > 0) {
        // one more drowning magician
        int wstatus;
        struct rusage rusek;
        wait4(pid, &wstatus, 0, &rusek);
        if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0) {
            fprintf(stderr, "is the magician a fish?\n");
            exit(1);
        } else {
            fprintf(stderr, "used user cpu: %.4lfs\n", timeval_to_double(rusek.ru_utime));
            fprintf(stderr, "used sys cpu:  %.4lfs\n", timeval_to_double(rusek.ru_stime));
        }
    } else {
        perror("failed to fork");
        exit(1);
    }

#undef MAX_ARGS
}

void eval_line(char *line, rlim_t cpu_limit, rlim_t mem_limit) {
    fprintf(stderr, "$ %s", line);
    if (line[0] == '#') {
        eval_env(line + 1);
    } else {
        eval_run(line, cpu_limit, mem_limit);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        help();
        exit(1);
    }

    const char *script_file_name = argv[1];
    rlim_t cpu_limit = (rlim_t) atoll(argv[2]);
    rlim_t mem_limit = ((rlim_t) atoll(argv[3])) * 1024 * 1024;

    FILE *script_file = fopen(script_file_name, "r");
    if (script_file == NULL) {
        perror("cannot open script");
        return -1;
    }

    size_t buffer_size = 16;
    char *line = calloc(buffer_size, sizeof(char));
    if (line == NULL) {
        perror("calloc failed!");
        exit(1);
    }

    while (getline(&line, &buffer_size, script_file) != -1) {
        eval_line(line, cpu_limit, mem_limit);
    }

    free(line);
    fclose(script_file);
    return 0;
}
