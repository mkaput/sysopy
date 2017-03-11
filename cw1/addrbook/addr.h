#ifndef ADDRBOOK_ADDR_H
#define ADDRBOOK_ADDR_H

#include <stdlib.h>

typedef struct Addr {
    int id;
    char *name;
    char *phone;
} Addr;

Addr *addr_new(int id, char name[], char phone[]);

Addr *addr_copy(Addr *addr);

void addr_free(Addr *addr);

int addr_compare_by_id(void *a, void *b);

int addr_compare_by_name(void *a, void *b);

int addr_compare_by_phone(void *a, void *b);

#endif //ADDRBOOK_ADDR_H
