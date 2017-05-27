#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define UNIX_PATH_MAX 108

#include "task.h"

void help() {
    printf("zad1_klient name [0 - internet | 1 - unix] [IP | path] [port | N/A]");
}

bool use_unix;

char name[MAX_NAME_LEN];
const char *ip_address;
in_port_t port;
char unix_path[UNIX_PATH_MAX];

int sock = -1;

void init_inet_socket() {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("failed creating internet socket");
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_aton(ip_address, &addr.sin_addr) == 0) {
        fprintf(stderr, "invalid IP address\n");
        exit(1);
    }

    if (connect(sock, (const struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("failed connecting internet socket");
        exit(1);
    }
}

void init_unix_socket() {
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("failed creating unix socket");
        exit(1);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, unix_path, UNIX_PATH_MAX);

    if (bind(sock, (const struct sockaddr *) &addr, sizeof(sa_family_t)) == -1) {
        perror("failed binding unix socket");
        exit(1);
    }

    if (connect(sock, (const struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("failed connecting unix socket");
        exit(1);
    }
}

void send_hello() {
    msg_t msg;
    msg.type = HELLO;
    strncpy(msg.msg.hello, name, MAX_NAME_LEN);
    if (send(sock, &msg, sizeof(msg), 0) == -1) {
        perror("failed to send hello");
    }
}

void eval(task_t task) {
    msg_t msg;
    msg.type = RESULT;

    result_t result;
    result.taskid = task.taskid;

    switch (task.type) {
        case ADD:
            result.result = task.a + task.b;
            break;
        case SUB:
            result.result = task.a - task.b;
            break;
        case MUL:
            result.result = task.a * task.b;
            break;
        case DIV:
            result.result = task.a / task.b;
            break;
    }

    msg.msg.result = result;

    if (send(sock, &msg, sizeof(msg), 0) == -1) {
        perror("failed to send result");
    }
}

void pong() {
    msg_t msg;
    msg.type = PING;
    if (send(sock, &msg, sizeof(msg), 0) == -1) {
        perror("failed to send pong");
    }
}

void on_atexit() {
    if (sock != -1) {
        if (close(sock) == -1) {
            perror("failed closing socket");
        }

        sock = -1;
    }
}

void on_sigint(int _signum) {
    on_atexit();
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 4 && argc != 5) {
        help();
        return 0;
    }

    atexit(on_atexit);
    signal(SIGINT, on_sigint);

    strncpy(name, argv[1], MAX_NAME_LEN);
    name[MAX_NAME_LEN - 1] = '\0';

    use_unix = strcmp(argv[2], "1") == 0;

    if (use_unix) {
        if (argc != 4) {
            help();
            return 0;
        }

        strncpy(unix_path, argv[3], UNIX_PATH_MAX);
        unix_path[UNIX_PATH_MAX - 1] = '\0';
    } else {
        if (argc != 5) {
            help();
            return 0;
        }

        ip_address = argv[3];
        port = (in_port_t) atoi(argv[4]);
    }

    if (use_unix) {
        init_unix_socket();
    } else {
        init_inet_socket();
    }

    send_hello();

    for (;;) {
        msg_t msg;
        ssize_t count = recv(sock, &msg, sizeof(msg), MSG_WAITALL);
        if (count == -1) {
            if (errno != EAGAIN) {
                perror("recv");
                return 1;
            }
        } else if (count == 0) {
            return 0;
        } else if (count < sizeof(msg)) {
            continue;
        }

        switch (msg.type) {
            case TASK:
                printf("Got task!\n");
                fflush(stdout);
                eval(msg.msg.task);
                break;
            case RESULT:
                fprintf(stderr, "stupid server\n");
                break;
            case PING:
                printf("Pinged!\n");
                fflush(stdout);
                pong();
                break;
            case PONG:
                break;
            case KILL:
                return 0;
            case HELLO:
                fprintf(stderr, "stupid server\n");
                break;
        }
    }
}
