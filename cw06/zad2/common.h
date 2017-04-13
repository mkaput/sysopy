#ifndef CW06_ZAD2_COMMON_H
#define CW06_ZAD2_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <mqueue.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>

void get_server_queue_name(char *buff, size_t buff_len) {
    snprintf(buff, buff_len, "/queue_server");
}

void get_client_queue_name(char *buff, size_t buff_len) {
    snprintf(buff, buff_len, "/queue_%d", getpid());
}

#define MSG_HELLO 1
#define MSG_ECHO 2
#define MSG_UPCS 3
#define MSG_TIME 4
#define MSG_EXIT 5
#define IFCLIENTWAIT(MTYPE) ((MTYPE) >= (MSG_HELLO) && (MTYPE) <= (MSG_TIME))
#define IFCLIENTDISPLAY(MTYPE) ((MTYPE) >= (MSG_ECHO) && (MTYPE) <= (MSG_TIME))
#define IFVALIDTYPE(MTYPE) ((MSG_HELLO) <= (MTYPE) && (MTYPE) <= (MSG_EXIT))

#define MSGSIZE (sizeof(msg_t))
#define MSGBUFFSIZE 512

typedef struct {
    char mtype;
    int mcid;
    char mbuff[MSGBUFFSIZE];
} msg_t;

#endif //CW06_ZAD2_COMMON_H
