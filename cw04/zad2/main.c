#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>

static int n;
static int m;
volatile int got_m;

volatile int *children_statuses;
volatile pid_t *children;

void help() {
    printf("usage: zad2 N M\n");
}

#define PRINTF_BUFFER_SUZE 256
static char printf_buffer[PRINTF_BUFFER_SUZE];

void xprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(printf_buffer, PRINTF_BUFFER_SUZE, format, args);
    va_end(args);
    write(STDOUT_FILENO, printf_buffer, strlen(printf_buffer));
}

void sigint_handler(int signo, siginfo_t *siginfo, void *context) {
    xprintf("OMG SIGINT!\n");
    for (int i = 0; i < n; i++) {
        if (children[i] != 0) {
            kill(children[i], SIGKILL);
            xprintf("stabbing %d with SIGKILL\n", children[i]);
        }
    }
    exit(1);
}

void sigusr1_handler(int signo, siginfo_t *siginfo, void *context) {
    int i = 0;
    for (pid_t pid = children[i]; i < n && pid != siginfo->si_pid; pid = children[++i]);

    if (i == n) {
        xprintf("wtf? what is %d?\n", siginfo->si_pid);
        return;
    }

    got_m++;

    if (children_statuses[i] == 0) {
        children_statuses[i] = 1;
    }

    xprintf("got request from %d (%d/%d, total:%d)\n", siginfo->si_pid, got_m, m, n);

    if (got_m >= m && children_statuses[i] < 2) {
        xprintf("party is already going\n");
        kill(siginfo->si_pid, SIGUSR1);
        children_statuses[i] = 2;
    }

    if (got_m == m) {
        xprintf("got_m == m, let the party begin!\n");
        for (unsigned long j = 0; j < n; j++) {
            if (children_statuses[j] == 1 && children[j] != 0) {
                children_statuses[j] = 2;
                xprintf("let's go %d!\n", children[j]);
                kill(children[j], SIGUSR1);
            }
        }
    }

    fflush(stdout); //that's awful
}

void sigrt_handler(int signo, siginfo_t *siginfo, void *context) {
    xprintf("got rt %d from %d\n", signo, siginfo->si_pid);
}

int child_main(pid_t ppid) {
    srand((unsigned int) getpid());

    usleep((__useconds_t) ((rand() % 9500000) + 500 * 1000));

    kill(ppid, SIGUSR1);

    struct timespec start_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) == -1) {
        perror("failed to block SIGUSR1");
        _exit(127);
    }

    struct timespec timeout;
    timeout.tv_sec = 12;
    timeout.tv_nsec = 0;
    if (sigtimedwait(&mask, NULL, &timeout) == -1) {
        if (errno == EAGAIN) {
            printf("forever alone (%d)", getpid());
        } else {
            perror("timeout failed");
        }
        _exit(1);
    }

    printf("sending rt (%d)\n", getpid());
    union sigval sval;
    sval.sival_int = 0;
    int sig_code = SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN + 1));
    sigqueue(ppid, sig_code, sval);

    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);
    double delta_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;
    return (int) (delta_time + 0.5);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        help();
        return 0;
    }

    n = atoi(argv[1]);
    m = atoi(argv[2]);
    got_m = 0;

    children = calloc((size_t) n, sizeof(pid_t));
    children_statuses = calloc((size_t) n, sizeof(int));
    memset((void *) children_statuses, 0, n * sizeof(int));

    struct sigaction sa_usr;
    sa_usr.sa_sigaction = sigusr1_handler;
    sigemptyset(&sa_usr.sa_mask);
    sa_usr.sa_flags = SA_RESTART | SA_SIGINFO;
    if (sigaction(SIGUSR1, &sa_usr, NULL) == -1) {
        perror("failed hooking SIGUSR1");
        free((void *) children_statuses);
        free((void *) children);
        exit(1);
    }

    struct sigaction sa_int;
    sa_int.sa_sigaction = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART | SA_SIGINFO;
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("failed hooking SIGINT");
        free((void *) children_statuses);
        free((void *) children);
        exit(1);
    }

    for (int code = SIGRTMIN; code <= SIGRTMAX; ++code) {
        struct sigaction sa_rt;
        sa_rt.sa_sigaction = sigrt_handler;
        sigemptyset(&sa_rt.sa_mask);
        sa_rt.sa_flags = SA_RESTART | SA_SIGINFO;
        if (sigaction(code, &sa_rt, NULL) == -1) {
            perror("failed hooking readlime signal");
            free((void *) children_statuses);
            free((void *) children);
            exit(1);
        }
    }

    pid_t mypid = getpid();

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            int result = child_main(mypid);
            _exit(result);
        } else if (pid > 0) {
            printf("spawning %d, pid: %d\n", i, pid);
            fflush(stdout);
            children[i] = pid;
        } else {
            perror("fork failed");
            free((void *) children_statuses);
            free((void *) children);
            exit(1);
        }
    }

    pid_t wpid;
    int wstatus = 0;
    while ((wpid = wait(&wstatus)) > 0) {
        if (WIFEXITED(wstatus)) {
            printf("%d died with: %d\n", wpid, WEXITSTATUS(wstatus));
        }
        if (WIFSIGNALED((wstatus))) {
            printf("%d was signaled by %d\n", wpid, WTERMSIG(wstatus));
        }
    }

    free((void *) children_statuses);
    free((void *) children);

    return 0;
}
