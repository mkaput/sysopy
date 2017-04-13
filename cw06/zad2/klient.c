#include <sys/msg.h>
#include "common.h"

static int cid = -1;
static char msqname[PATH_MAX];
static int msqid = -1;
static char srv_msqname[PATH_MAX];
static int srv_msqid = -1;

void close_msgq(void) {
    if (msqid != -1) {
        mq_close(msqid);
        mq_unlink(msqname);
    }

    if (srv_msqid != -1) {
        mq_close(srv_msqid);
    }
}

int main() {
    atexit(close_msgq);

    get_client_queue_name(msqname, PATH_MAX);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSGSIZE;
    attr.mq_curmsgs = 0;
    if ((msqid = mq_open(msqname, O_RDONLY | O_CREAT | O_EXCL, 0777, &attr)) == -1) {
        perror("failed to create client queue");
        exit(1);
    }

    get_server_queue_name(srv_msqname, PATH_MAX);

    if ((srv_msqid = mq_open(srv_msqname, O_WRONLY)) == -1) {
        perror("failed to connect to server queue");
        exit(1);
    }

    {
        msg_t msg;
        msg.mtype = MSG_HELLO;
        msg.mcid = 0;
        strncpy(msg.mbuff, msqname, MSGBUFFSIZE);
        if (mq_send(srv_msqid, (const char *) &msg, MSGSIZE, 0) == -1) {
            perror("failed to send hello message");
            exit(1);
        }

        if (mq_receive(msqid, (char *) &msg, MSGSIZE, NULL) == -1) {
            perror("failed to receive hello message");
            exit(1);
        }

        cid = msg.mcid;

        fprintf(stderr, "my id is: %d\n", cid);
    }

    printf("usage:\n"
                   "\techo <text>\n"
                   "\tupper <text>\n"
                   "\ttime\n"
                   "\texit\n"
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

        if (mq_send(srv_msqid, (const char *) &msg, MSGSIZE, 0) == -1) {
            perror("failed to send message");
            exit(1);
        }

        if (IFCLIENTWAIT(msg.mtype)) {
            if (mq_receive(msqid, (char *) &msg, MSGSIZE, NULL) == -1) {
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
