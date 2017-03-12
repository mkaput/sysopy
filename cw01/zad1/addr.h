#ifndef ADDRBOOK_ADDR_H
#define ADDRBOOK_ADDR_H

#include <stdlib.h>

typedef struct Addr {
    int id;
    char *name;
    char *phone;
} Addr;

Addr *addr_new(int id, const char *name, const char *phone);

Addr *addr_copy(const Addr *addr);

void addr_free(Addr *addr);

int addr_compare_by_id(const void *a, const void *b);

int addr_compare_by_name(const void *a, const void *b);

int addr_compare_by_phone(const void *a, const void *b);

#endif //ADDRBOOK_ADDR_H
