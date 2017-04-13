#include "common.h"

#define MAX_CLIENTS 512

static bool working = true;
static int msqid = -1;
static int cqmap[MAX_CLIENTS] = {-1};
static int lastid = 0;

void onmsg_hello(msg_t *msg) {
    if (lastid == MAX_CLIENTS) {
        fprintf(stderr, "client limit exceeded\n");
        exit(2);
    }

    fprintf(stderr, "client %d connected, giving id %d\n", msg->mcid, lastid);

    cqmap[lastid] = msg->mcid;
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
    working = false;
    msg->mtype = -1;
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
        msgctl(msqid, IPC_RMID, 0);
    }
}

int main() {
    atexit(close_msgq);

    key_t key = SERVERKEY();
    if (key == -1) {
        perror("key error");
        exit(1);
    }

    if ((msqid = msgget(key, ALL_PERM | IPC_CREAT | IPC_EXCL)) == -1) {
        perror("failed to create new queue");
        exit(1);
    }

    for (;;) {
        msg_t msg;
        if (msgrcv(msqid, &msg, MSGSIZE, 0, IPC_NOWAIT) == -1) {
            if (errno == ENOMSG) {
                if (!working) break;
            } else {
                perror("msgrcv failed");
                exit(1);
            }
        } else {
            if (IFVALIDTYPE(msg.mtype)) {
                msghandlers[msg.mtype](&msg);
                if (msg.mtype >= 0) {
                    int qid;
                    if (msg.mcid >= 0 && msg.mcid < MAX_CLIENTS && cqmap[msg.mcid] != -1) {
                        qid = cqmap[msg.mcid];
                    } else {
                        fprintf(stderr, "unknown client id\n");
                        exit(1);
                    }

                    if (msgsnd(qid, &msg, MSGSIZE, 0) == -1) {
                        perror("failed to reply");
                        exit(1);
                    }
                }
            } else {
                fprintf(stderr, "received invalid message type: %ld\n", msg.mtype);
            }
        }
    }

    return 0;
}
