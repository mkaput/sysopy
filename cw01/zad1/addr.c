#include "addr.h"

#include <string.h>

Addr *addr_new(int id, const char *name, const char *phone) {
    Addr *addr = malloc(sizeof(Addr));
    addr->id = id;
    addr->name = calloc(strlen(name) + 1, sizeof(char));
    strcpy(addr->name, name);
    addr->phone = calloc(strlen(phone) + 1, sizeof(char));
    strcpy(addr->phone, phone);
    return addr;
}

Addr *addr_copy(const Addr *source) {
    Addr *addr = malloc(sizeof(Addr));
    addr->id = source->id;
    addr->name = calloc(strlen(source->name) + 1, sizeof(char));
    strcpy(addr->name, source->name);
    addr->phone = calloc(strlen(source->phone) + 1, sizeof(char));
    strcpy(addr->phone, source->phone);
    return addr;
}

void addr_free(Addr *addr) {
    free(addr->name);
    free(addr->phone);
    free(addr);
}

int addr_compare_by_id(const void *a, const void *b) {
    Addr *addr_a = (Addr *) a;
    Addr *addr_b = (Addr *) b;
    return addr_a->id - addr_b->id;
}

int addr_compare_by_name(const void *a, const void *b) {
    Addr *addr_a = (Addr *) a;
    Addr *addr_b = (Addr *) b;
    return strcmp(addr_a->name, addr_b->name);
}

int addr_compare_by_phone(const void *a, const void *b) {
    Addr *addr_a = (Addr *) a;
    Addr *addr_b = (Addr *) b;
    return strcmp(addr_a->phone, addr_b->phone);
}
