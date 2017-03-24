#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void sig_handler(int signo) {
    // https://www.youtube.com/watch?v=fjQMhsOQOB8
    printf("You never gonna catch me! You're wasing your time! Neeh-oh-neeh neeh-oh\n");
    signal(signo, sig_handler);
}

int main(int argc, char *argv[]) {
    printf("Releasing Greased Up Guy!\n");

    if (signal(SIGXCPU, sig_handler) == SIG_ERR) {
        perror("failed to catch SIGXCPU");
        return -1;
    }

    while (1);
}
