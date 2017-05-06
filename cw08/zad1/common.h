#ifndef COMMON_H
#define COMMON_H

#define _DEFAULT_SOURCE
#include <sys/syscall.h>
#include <unistd.h>

typedef struct {
    int id;
    char text[1020];
} record_t;

long gettid() {
    return syscall(SYS_gettid);
}

#endif //COMMON_H
