#ifndef CW06_ZAD1_COMMON_H
#define CW06_ZAD1_COMMON_H

#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>

#define ALL_PERM 0777

#define PATHNAME() (getenv("HOME"))
#define PROJECT_ID ((int)('M' ^ 'K'))
#define SERVERKEY() (ftok(PATHNAME(), PROJECT_ID))

#define MSG_HELLO 1
#define MSG_ECHO 2
#define MSG_UPCS 3
#define MSG_TIME 4
#define MSG_EXIT 5
#define IFCLIENTWAIT(MTYPE) ((MTYPE) >= (MSG_HELLO) && (MTYPE) <= (MSG_TIME))
#define IFCLIENTDISPLAY(MTYPE) ((MTYPE) >= (MSG_ECHO) && (MTYPE) <= (MSG_TIME))
#define IFVALIDTYPE(MTYPE) ((MSG_HELLO) <= (MTYPE) && (MTYPE) <= (MSG_EXIT))

#define MSGBUFFSIZE 512
#define MSGSIZE (sizeof(int) / sizeof(char) + (MSGBUFFSIZE))

typedef struct {
    long mtype;
    int mcid;
    char mbuff[MSGBUFFSIZE];
} msg_t;

#endif //CW06_ZAD1_COMMON_H
