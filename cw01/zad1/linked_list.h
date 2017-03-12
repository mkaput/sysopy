#ifndef ADDRBOOK_LINKED_LIST_H
#define ADDRBOOK_LINKED_LIST_H

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

LLLinkedList *ll_new(void);

void ll_free(LLLinkedList *list);

size_t ll_size(const LLLinkedList *list);

bool ll_contains(const LLLinkedList *list, const void *value);

LLNode *ll_head_node(const LLLinkedList *list);

LLNode *ll_last_node(const LLLinkedList *list);

LLNode *ll_get_node(const LLLinkedList *list, const void *value);

void ll_prepend(LLLinkedList *list, void *value);

void ll_append(LLLinkedList *list, void *value);

void ll_remove(LLLinkedList *list, void *value);

void ll_remove_node(LLLinkedList *list, LLNode *node);

LLNode *ll_next(const LLNode *node);

LLNode *ll_prev(const LLNode *node);

void ll_sort(LLLinkedList *list, ValueComparator cmp);

#endif //ADDRBOOK_LINKED_LIST_H
