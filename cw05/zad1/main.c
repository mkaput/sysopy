#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>

static void eval_line(char *line) {
#define MAX_PROGS 64
#define MAX_ARGS 32

    size_t progc = 0;

    size_t argc[MAX_PROGS + 2];
    memset(argc, 0, (MAX_PROGS + 2) * sizeof(size_t));

    char *argv[MAX_PROGS + 2][MAX_ARGS + 2];
    memset(argv, 0, (MAX_PROGS + 2) * (MAX_ARGS + 2) * sizeof(char *));

    {
        char *progs[MAX_PROGS + 2];
        memset(progs, 0, (MAX_PROGS + 2) * sizeof(char *));

        char *current_prog = NULL;
        while ((current_prog = strtok(current_prog == NULL ? line : NULL, "|")) != NULL) {
            if (progc == MAX_PROGS) {
                fprintf(stderr, "too many programs\n");
                exit(1);
            }

            progs[progc++] = current_prog;
        }

        for (size_t i = 0; i < progc; ++i) {
            char *current_arg = NULL;
            while ((current_arg = strtok(current_arg == NULL ? progs[i] : NULL, " \t\n\r")) != NULL) {
                if (argc[i] == MAX_ARGS) {
                    fprintf(stderr, "too many args\n");
                    exit(1);
                }

                argv[i][argc[i]++] = current_arg;
            }
        }
    }

    pid_t *pids = calloc(progc, sizeof(pid_t));
    for (int i = 0; i < progc; ++i) pids[i] = -1;

    int fds[4] = {-1, -1, -1, -1};

    for (int i = 0; i < progc; ++i) {
        fds[1] = fds[3];
        fds[0] = fds[2];

        if (pipe(fds + 2) == -1) {
            perror("failed to pipe");
            exit(1);
        }

        pids[i] = fork();
        if (pids[i] == 0) {
            if (i > 0) {
                if (fds[0] != -1) {
                    dup2(fds[0], STDIN_FILENO);
                }

                if (fds[1] != -1) {
                    close(fds[1]);
                }
            }

            if (i < progc - 1) {
                if (fds[2] != -1) {
                    close(fds[2]);
                }

                if (fds[3] != -1) {
                    dup2(fds[3], STDOUT_FILENO);
                }
            }

            if (execvp(argv[i][0], argv[i]) == -1) {
                perror("failed to execvp");
                exit(-1);
            }
        } else if (pids[i] < 0) {
            perror("fork failed");
            exit(1);
        }

        if (fds[1] != -1) close(fds[1]);
        if (fds[0] != -1) close(fds[0]);
    }

    for (int i = 0; i < progc; ++i) {
        int wstatus;
        if (waitpid(pids[i], &wstatus, 0) == -1) {
            perror("failed to wait for child");
            exit(1);
        }
    }

    free(pids);

#undef MAX_ARGS
#undef MAX_PROGS
}

int main() {
    size_t buffer_size = 16;
    char *line = calloc(buffer_size, sizeof(char));
    if (line == NULL) {
        perror("calloc failed!");
        exit(-1);
    }

    for (;;) {
        printf("$ ");
        fflush(stdout);
        if (getline(&line, &buffer_size, stdin) == -1) break;
        eval_line(line);
    }

    free(line);
    return 0;
}
