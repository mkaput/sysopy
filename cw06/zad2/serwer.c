#include "common.h"

#define MAX_CLIENTS 512

static char msqname[PATH_MAX] = "";
static mqd_t msqid = -1;
static char cqmap[MAX_CLIENTS][PATH_MAX] = {""};
static int lastid = 0;

void onmsg_hello(msg_t *msg) {
    if (lastid == MAX_CLIENTS) {
        fprintf(stderr, "client limit exceeded\n");
        exit(2);
    }

    fprintf(stderr, "client %s connected, giving id %d\n", msg->mbuff, lastid);

    strncpy(cqmap[lastid], msg->mbuff, PATH_MAX);
    msg->mcid = lastid;
    lastid++;
}

void onmsg_echo(msg_t *msg) {
    // do nothing, the message will be sent back unchanged
}

void onmsg_upcs(msg_t *msg) {
    for (size_t i = 0; i < MSGBUFFSIZE && msg->mbuff[i] != '\0'; i++) {
        msg->mbuff[i] = (char) toupper(msg->mbuff[i]);
    }
}

void onmsg_time(msg_t *msg) {
    time_t t = time(NULL);
    ctime_r(&t, msg->mbuff);
}

void onmsg_exit(msg_t *msg) {
    exit(0);
}

static void (*(msghandlers[]))(msg_t *msg) = {
        NULL,
        onmsg_hello, /* MSG_HELLO 1 */
        onmsg_echo,  /* MSG_ECHO 2 */
        onmsg_upcs,  /* MSG_UPCS 3 */
        onmsg_time,  /* MSG_TIME 4 */
        onmsg_exit   /* MSG_EXIT 5 */
};

void close_msgq(void) {
    if (msqid != -1) {
        mq_close(msqid);
        mq_unlink(msqname);
    }
}

int main() {
    atexit(close_msgq);

    get_server_queue_name(msqname, PATH_MAX);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSGSIZE;
    attr.mq_curmsgs = 0;
    if ((msqid = mq_open(msqname, O_RDONLY | O_CREAT | O_EXCL, 0777, &attr)) == -1) {
        perror("failed to create new queue");
        exit(1);
    }

    for (;;) {
        msg_t msg;
        if (mq_receive(msqid, (char *) &msg, MSGSIZE, NULL) == -1) {
            perror("msgrcv failed");
            exit(1);
        }

        if (IFVALIDTYPE(msg.mtype)) {
            msghandlers[(size_t) msg.mtype](&msg);
            if (msg.mtype >= 0) {
                const char *qname;
                if (msg.mcid >= 0 && msg.mcid < MAX_CLIENTS) {
                    qname = cqmap[msg.mcid];
                } else {
                    fprintf(stderr, "unknown client id\n");
                    exit(1);
                }

                int qid;
                if ((qid = mq_open(qname, O_WRONLY)) == -1) {
                    perror("failed to open client queue");
                    exit(1);
                }

                if (mq_send(qid, (const char *) &msg, MSGSIZE, 0) == -1) {
                    perror("failed to reply");
                    exit(1);
                }

                mq_close(qid);
            }
        } else {
            fprintf(stderr, "received invalid message type: %d\n", msg.mtype);
        }
    }

    return 0;
}
