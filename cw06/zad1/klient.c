#include "common.h"

static int cid = -1;
static int msqid = -1;
static int srv_msqid = -1;

void close_msgq(void) {
    if (msqid != -1) {
        msgctl(msqid, IPC_RMID, 0);
    }
}

int main() {
    atexit(close_msgq);

    key_t key = ftok(PATHNAME(), getpid());
    if (key == -1) {
        perror("client key error");
        exit(1);
    }

    if ((msqid = msgget(key, ALL_PERM | IPC_CREAT | IPC_EXCL)) == -1) {
        perror("failed to create client queue");
        exit(1);
    }

    key = SERVERKEY();
    if (key == -1) {
        perror("server key error");
        exit(1);
    }

    if ((srv_msqid = msgget(key, ALL_PERM)) == -1) {
        perror("failed to connect to server queue");
        exit(1);
    }

    {
        msg_t msg;
        msg.mtype = MSG_HELLO;
        msg.mcid = msqid;
        if (msgsnd(srv_msqid, &msg, MSGSIZE, 0) == -1) {
            perror("failed to send hello message");
            exit(1);
        }

        if (msgrcv(msqid, &msg, MSGSIZE, MSG_HELLO, 0) == -1) {
            perror("failed to receive hello message");
            exit(1);
        }

        cid = msg.mcid;
    }

    printf("usage:\n"
                   "\techo <text>\n"
                   "\tupper <text>\n"
                   "\ttime\n"
                   "\tclose\n"
                   "\t:q - exit client\n");

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

        char *cmd = line;
        char *data = line;
        while (data[0] != '\0') {
            if (isblank(data[0])) {
                data[0] = '\0';
                data++;
                break;
            }
            data++;
        }
        for (size_t i = 0; cmd[i] != '\0'; i++) {
            if (isblank(cmd[i]) || cmd[i] == '\n' || cmd[i] == '\r') {
                cmd[i] = '\0';
            }
        }

        msg_t msg;
        msg.mcid = cid;
        strncpy(msg.mbuff, data, MSGBUFFSIZE);
        msg.mbuff[MSGBUFFSIZE - 1] = '\0';

        if (strcmp(cmd, "echo") == 0) {
            msg.mtype = MSG_ECHO;
        } else if (strcmp(cmd, "upper") == 0) {
            msg.mtype = MSG_UPCS;
        } else if (strcmp(cmd, "time") == 0) {
            msg.mtype = MSG_TIME;
        } else if (strcmp(cmd, "exit") == 0) {
            msg.mtype = MSG_EXIT;
        } else if (strcmp(cmd, ":q") == 0) {
            exit(0);
        } else {
            fprintf(stderr, "unknown message\n");
            continue;
        }

        if (msgsnd(srv_msqid, &msg, MSGSIZE, 0) == -1) {
            perror("failed to send message");
            exit(1);
        }

        if (IFCLIENTWAIT(msg.mtype)) {
            if (msgrcv(msqid, &msg, MSGSIZE, msg.mtype, 0) == -1) {
                perror("failed to receive message");
                exit(1);
            }

            if (IFCLIENTDISPLAY(msg.mtype)) {
                printf("%s\n", msg.mbuff);
            }
        }
    }

    free(line);
    return 0;
}
