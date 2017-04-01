#define _XOPEN_SOURCE 700

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

static char ch = 'a' - 1;
static int dir = 1;

#define PRINTF_BUFFER_SUZE 256
static char printf_buffer[PRINTF_BUFFER_SUZE];

void xprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(printf_buffer, PRINTF_BUFFER_SUZE, format, args);
    va_end(args);
    write(STDOUT_FILENO, printf_buffer, strlen(printf_buffer));
}

void sigint_handler(int signo) {
    xprintf("Odebrano sygnał SIGINT\n");
    exit(0);
}

void sigtstp_handler(int signo) {
    dir *= -1;
}

int main(int argc, char *argv[]) {
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("failed to catch SIGINT");
        return 1;
    }

    struct sigaction sa;
    sa.sa_handler = sigtstp_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("failed to catch SIGTSTP");
        return 1;
    }

    while (1) {
        ch += dir;
        if (ch < 'a') ch = 'z';
        else if (ch > 'z') ch = 'a';
        printf("%c\n", ch);
        sleep(1);
    }
}
