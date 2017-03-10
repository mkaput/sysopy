#ifndef SYSOPY_KSIAZKA_ADRESOWA_LINKED_LIST_H
#define SYSOPY_KSIAZKA_ADRESOWA_LINKED_LIST_H

#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

typedef struct LLNode {
    void *value;
    struct LLNode *_next;
    struct LLNode *_prev;
} LLNode;

typedef struct LLLinkedList {
    LLNode *_head;
    LLNode *_last;
    size_t _size;
} LLLinkedList;

LLLinkedList *ll_new();

void ll_free(LLLinkedList *list);

size_t ll_size(LLLinkedList *list);

bool ll_contains(LLLinkedList *list, void *value);

LLNode *ll_head_node(LLLinkedList *list);

LLNode *ll_last_node(LLLinkedList *list);

LLNode *ll_get_node(LLLinkedList *list, void *value);

void ll_prepend(LLLinkedList *list, void *value);

void ll_append(LLLinkedList *list, void *value);

void ll_remove(LLLinkedList *list, void *value);

void ll_remove_node(LLLinkedList *list, LLNode *node);

LLNode *ll_next(LLNode *node);

LLNode *ll_prev(LLNode *node);

void ll_sort(LLLinkedList *list, ValueComparator cmp);

#endif //SYSOPY_KSIAZKA_ADRESOWA_LINKED_LIST_H
