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

static int l;
static int t;

volatile int sent_sigs = 0;
volatile int got_sigs = 0;
volatile int child_got_sigs = 0;

void help() {
    printf("usage: zad3 L T\n");
}

int child_main() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGRTMIN);
    sigaddset(&mask, SIGRTMAX);

    sigprocmask(SIG_BLOCK, &mask, NULL);

    struct timespec timeout;
    timeout.tv_sec = 2;
    timeout.tv_nsec = 0;

    int signo;
    siginfo_t siginfo;
    while ((signo = sigtimedwait(&mask, &siginfo, &timeout)) != -1) {
        if (signo == SIGUSR1 || signo == SIGRTMIN) {
            child_got_sigs++;
            kill(siginfo.si_pid, signo);
        } else {
            printf("child received: %d\n", child_got_sigs);
            kill(siginfo.si_pid, t == 3 ? SIGRTMAX : SIGUSR2);
            _exit(0);
        }
    }

    perror("failed to wait for signal");
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        help();
        return 0;
    }

    l = atoi(argv[1]);
    t = atoi(argv[2]);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGRTMIN);
    sigaddset(&mask, SIGRTMAX);

    sigprocmask(SIG_BLOCK, &mask, NULL);

    pid_t pid = fork();
    if (pid == 0) {
        int result = child_main();
        _exit(result);
    } else if (pid > 0) {
        for (int i = 0; i < l; i++) {
            if (t == 1) {
                kill(pid, SIGUSR1);
            } else if (t == 2) {
                union sigval sval;
                sval.sival_int = 0;
                sigqueue(pid, SIGUSR1, sval);
            } else if (t == 3) {
                kill(pid, SIGRTMIN);
            }
            usleep(100);
            sent_sigs++;
        }

        printf("sent sigs:      %d\n", sent_sigs);

        if (t == 1) {
            kill(pid, SIGUSR2);
        } else if (t == 2) {
            union sigval sval;
            sval.sival_int = 0;
            sigqueue(pid, SIGUSR2, sval);
        } else if (t == 3) {
            kill(pid, SIGRTMAX);
        }
    } else {
        perror("fork failed");
        exit(1);
    }

    struct timespec timeout;
    timeout.tv_sec = 10;
    timeout.tv_nsec = 0;

    int signo;
    siginfo_t siginfo;
    while ((signo = sigtimedwait(&mask, &siginfo, &timeout)) != -1) {
        if (signo == SIGUSR1 || signo == SIGRTMIN) {
            got_sigs++;
        } else {
            printf("received back:  %d\n", got_sigs);
        }
    }

//    wait(NULL);
    return 0;
}
