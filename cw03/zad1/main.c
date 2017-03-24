#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>

void help() {
    printf("usage: zad1 <script file>\n");
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

void eval_run(char *line, unsigned long i, unsigned long i1) {
#define MAX_ARGS 32

    size_t argc = 0;
    char *argv[MAX_ARGS + 2];
    memset(argv, NULL, (MAX_ARGS + 2) * sizeof(char *));

    argv[argc++] = strtok(line, " \t\n\r");

    char *current_arg = NULL;
    while ((current_arg = strtok(NULL, " \t\n\r")) != NULL) {
        if (argc == MAX_ARGS) {
            fprintf(stderr, "too many arguments!\n");
            exit(-1);
        }

        argv[argc++] = current_arg;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // prestige
        if (execvp(argv[0], argv) == -1) {
            perror("failed to execvp");
            exit(-1);
        }
    } else if (pid > 0) {
        // one more drowning magician
        int wstatus;
        waitpid(pid, &wstatus, 0);
        if(WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0){
            fprintf(stderr, "is the magician a fish?\n");
            exit(-1);
        }
    } else {
        perror("failed to fork");
        exit(-1);
    }

#undef MAX_ARGS
}

void eval_line(char *line, unsigned long i, unsigned long i1) {
    fprintf(stderr, "$ %s", line);
    if (line[0] == '#') {
        eval_env(line + 1);
    } else {
        eval_run(line, 0, 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        help();
        exit(-1);
    }

    const char *script_file_name = argv[1];
    FILE *script_file = fopen(script_file_name, "r");
    if (script_file == NULL) {
        perror("cannot open script");
        return -1;
    }

    size_t buffer_size = 16;
    char *line = calloc(buffer_size, sizeof(char));
    if (line == NULL) {
        perror("calloc failed!");
        exit(-1);
    }

    while (getline(&line, &buffer_size, script_file) != -1) {
        eval_line(line, 0, 0);
    }

    free(line);
    fclose(script_file);
    return 0;
}
