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
#include <sys/epoll.h>

#define UNIX_PATH_MAX 108
#define MAX_EPOLL_EVENTS 64
#define MAX_CLIENTS (SOMAXCONN * 2)

#include "task.h"

void help() {
    printf("zad1_serwer PORT UNIX_PATH");
}

in_port_t port;
char unix_path[UNIX_PATH_MAX];

pthread_t network_thread;

int inet_socket = -1;
int unix_socket = -1;

struct epoll_event events[MAX_EPOLL_EVENTS];

int efd = -1;

int clientfds[MAX_CLIENTS] = {-1};
ssize_t clientfds_size = -1;
pthread_mutex_t clientfds_mutex = PTHREAD_MUTEX_INITIALIZER;

void add_client(int fd) {
    pthread_mutex_lock(&clientfds_mutex);
    if (clientfds_size >= MAX_CLIENTS) {
        fprintf(stderr, "too many clients\n");
        pthread_mutex_unlock(&clientfds_mutex);
        exit(1);
    }
    clientfds[++clientfds_size] = fd;
    pthread_mutex_unlock(&clientfds_mutex);
}

int random_client(void) {
    pthread_mutex_lock(&clientfds_mutex);
    if (clientfds_size == 0) return -1;
    int fd = clientfds[rand() % clientfds_size];
    pthread_mutex_unlock(&clientfds_mutex);
    return fd;
}

void close_client(int fd) {
    pthread_mutex_lock(&clientfds_mutex);
    for (size_t i = 0, j = 0; i < clientfds_size; i++) {
        if (clientfds[i] != fd) {
            clientfds[j++] = clientfds[i];
        } else {
            if (close(fd) == -1) {
                perror("failed closing client");
            }
        }
    }
    pthread_mutex_unlock(&clientfds_mutex);
}

void init_inet_socket() {
    inet_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (inet_socket == -1) {
        perror("failed creating internet socket");
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(inet_socket, (const struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("failed binding internet socket");
        exit(1);
    }
}

void init_unix_socket() {
    unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (unix_socket == -1) {
        perror("failed creating unix socket");
        exit(1);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, unix_path, UNIX_PATH_MAX);

    // Everybody is said to do this, so let's do it!
    unlink(unix_path);

    if (bind(unix_socket, (const struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("failed binding unix socket");
        exit(1);
    }
}

int make_socket_non_blocking(int sfd) {
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1) return -1;

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if (s == -1) return -1;

    return 0;
}

void *network_thread_routine(void *_arg) {
    init_inet_socket();
    init_unix_socket();

    if (make_socket_non_blocking(inet_socket) == -1) {
        perror("failed making internet socket non-blocking");
        exit(1);
    }

    if (make_socket_non_blocking(unix_socket) == -1) {
        perror("failed making unix socket non-blocking");
        exit(1);
    }

    if (listen(inet_socket, SOMAXCONN) == -1) {
        perror("failed listening on internet socket");
        exit(1);
    }

    if (listen(unix_socket, SOMAXCONN) == -1) {
        perror("failed listening on unix socket");
        exit(1);
    }

    efd = epoll_create1(0);
    if (efd == -1) {
        perror("epoll_create1");
        exit(1);
    }

    struct epoll_event ev;
    ev.data.fd = inet_socket;
    ev.events = EPOLLIN | EPOLLRDHUP;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, inet_socket, &ev) == -1) {
        perror("failed to add internet socket to epoll");
        exit(1);
    }

    ev.data.fd = unix_socket;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, unix_socket, &ev) == -1) {
        perror("failed to add unix socket to epoll");
        exit(1);
    }

    for (;;) {
        int n = epoll_wait(efd, events, MAX_EPOLL_EVENTS, -1);
        if (n == -1) {
            perror("epoll_wait");
            return NULL;
        }

        for (int i = 0; i < n; i++) {
            ev = events[i];

            if ((ev.events & EPOLLERR) != 0 || (ev.events & EPOLLHUP) != 0) {
                fprintf(stderr, "epoll fd error\n");
                if (ev.data.fd != inet_socket && ev.data.fd != unix_socket) {
                    close_client(ev.data.fd);
                }
            } else if ((ev.events & EPOLLRDHUP) != 0) {
                if (ev.data.fd != inet_socket && ev.data.fd != unix_socket) {
                    close_client(ev.data.fd);
                }
            } else if (ev.data.fd == inet_socket || ev.data.fd == unix_socket) {
                for (;;) {
                    struct sockaddr in_addr;
                    socklen_t in_len = sizeof(in_addr);
                    int fd = accept(ev.data.fd, &in_addr, &in_len);
                    if (fd == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        } else {
                            perror("accept");
                            break;
                        }
                    }

                    if (make_socket_non_blocking(fd) == -1) {
                        perror("failed making incoming socket non-blocking");
                        exit(1);
                    }

                    struct epoll_event ev2;
                    ev2.events = EPOLLIN | EPOLLET;
                    ev2.data.fd = fd;
                    if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev2) == -1) {
                        perror("failed to add incoming socket to epoll");
                        exit(1);
                    }

                    add_client(fd);
                }
            } else {
                for (;;) {
                    msg_t msg;
                    ssize_t count = recv(ev.data.fd, &msg, sizeof(msg), MSG_WAITALL);
                    if (count == -1) {
                        if (errno != EAGAIN) {
                            perror("recv");
                            close_client(ev.data.fd);
                        }
                        break;
                    } else if (count == 0) {
                        close_client(ev.data.fd);
                        break;
                    } else if (count < sizeof(msg)) {
                        fprintf(stderr, "received partial message\n");
                        continue;
                    } else {
                        switch (msg.type) {
                            case TASK:
                                fprintf(stderr, "stupid client\n");
                                break;
                            case RESULT:
                                printf("Out[%d] = %d\n", msg.msg.result.taskid, msg.msg.result.result);
                                fflush(stdout);
                                break;
                            case PING:
                                break;
                            case PONG:
                                break;
                            case KILL:
                                close_client(ev.data.fd);
                                break;
                            case HELLO:
                                printf("%s connected\n", msg.msg.hello);
                                break;
                        }
                    }
                }
            }
        }
    }

    fprintf(stderr, "network thread finished event loop unexpectedly\n");
    return NULL;
}

void dispatch(task_t task) {
    msg_t msg;
    msg.type = TASK;
    msg.msg.task = task;

    int fd = random_client();
    if (fd == -1) {
        fprintf(stderr, "no clients\n");
        return;
    }

    if (send(fd, &msg, sizeof(msg), 0) == -1) {
        perror("failed sending task");
    }
}

void on_atexit() {
    if ((errno = pthread_cancel(network_thread)) == -1) {
        perror("failed to send signal to network thread");
    }

    if ((errno = pthread_join(network_thread, NULL)) == -1) {
        perror("failed joining network thread");
    }

    if (efd != -1) {
        if (close(efd) == -1) {
            perror("failed closing epoll fd");
        }

        efd = -1;
    }

    if (unix_socket != -1) {
        if (shutdown(unix_socket, SHUT_RDWR) == -1) {
            perror("failed shutting down unix socket");
        }

        if (close(unix_socket) == -1) {
            perror("failed closing unix socket");
        }

        if (unlink(unix_path) == -1) {
            perror("failed unlinking unix socket");
        }

        unix_socket = -1;
    }

    if (inet_socket != -1) {
        if (shutdown(inet_socket, SHUT_RDWR) == -1) {
            perror("failed shutting down internet socket");
        }

        if (close(inet_socket) == -1) {
            perror("failed closing internet socket");
        }

        inet_socket = -1;
    }

    pthread_mutex_destroy(&clientfds_mutex);
}

void on_sigint(int _signum) {
    on_atexit();
    exit(0);
}

int main(int argc, char *argv[]) {
    srand((unsigned int) time(NULL));

    atexit(on_atexit);
    signal(SIGINT, on_sigint);

    if (argc != 3) {
        help();
        return 0;
    }

    port = (in_port_t) atoi(argv[1]);
    strncpy(unix_path, argv[2], UNIX_PATH_MAX);
    unix_path[UNIX_PATH_MAX - 1] = '\0';

    if ((errno = pthread_create(&network_thread, NULL, network_thread_routine, NULL)) != 0) {
        perror("failed to create network thread!");
        return 1;
    }

    printf("1 a b => a + b\n2 a b => a - b\n3 a b => a * b\n4 a b => a / b\n5 - exit\n");
    fflush(stdout);

    int taskid = 0;
    for (;;) {
        task_t task;
        task.taskid = taskid;
        int task_code;

        printf("%d> ", taskid);
        fflush(stdout);
        if (scanf("%d", &task_code) == EOF) break;

        switch (task_code) {
            case 1:
                task.type = ADD;
                break;
            case 2:
                task.type = SUB;
                break;
            case 3:
                task.type = MUL;
                break;
            case 4:
                task.type = DIV;
                break;
            case 5:
                return 0;
            default:
                printf("co to za task\n");
                continue;
        }

        if (scanf("%d %d", &(task.a), &(task.b)) == EOF) break;

        dispatch(task);
        taskid++;
    }

    return 0;
}
