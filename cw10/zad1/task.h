#ifndef CW10_TASK_H
#define CW10_TASK_H

#define MAX_NAME_LEN 10

typedef enum {
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3
} task_type_t;

typedef struct {
    task_type_t type;
    int taskid;
    int a;
    int b;
} task_t;

typedef struct {
    int taskid;
    int result;
} result_t;

typedef enum {
    TASK = 0,
    RESULT = 1,
    PING = 2,
    PONG = 3,
    KILL = 4,
    HELLO = 5
} msg_type_t;

typedef struct {
    msg_type_t type;
    union {
        task_t task;
        result_t result;
        char hello[MAX_NAME_LEN];
    } msg;
} msg_t;

#endif //CW10_TASK_H
