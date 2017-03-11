#include "linked_list.h"
#include "utils.h"

LLLinkedList *ll_new(void) {
    LLLinkedList *list = malloc(sizeof(LLLinkedList));
    list->_head = NULL;
    list->_last = NULL;
    list->_size = 0;
    return list;
}

void ll_free(LLLinkedList *list) {
    LLNode *node = ll_head_node(list);
    while (node != NULL) {
        LLNode *next_node = ll_next(node);
        free(node);
        node = next_node;
    }

    free(list);
}

size_t ll_size(LLLinkedList *list) {
    return list->_size;
}

bool ll_contains(LLLinkedList *list, void *value) {
    return ll_get_node(list, value) != NULL;
}

LLNode *ll_head_node(LLLinkedList *list) {
    return list != NULL ? list->_head : NULL;
}

LLNode *ll_last_node(LLLinkedList *list) {
    return list != NULL ? list->_last : NULL;
}

LLNode *ll_get_node(LLLinkedList *list, void *value) {
    LLNode *node = ll_head_node(list);
    while (node != NULL) {
        if (node->value == value) return node;
        node = ll_next(node);
    }
    return NULL;
}

void ll_prepend(LLLinkedList *list, void *value) {
    LLNode *node = malloc(sizeof(LLNode));
    node->value = value;
    node->_next = ll_head_node(list);
    node->_prev = NULL;

    list->_head->_prev = node;
    list->_head = node;
    list->_size += 1;

    if (list->_last == NULL) list->_last = node;
}

void ll_append(LLLinkedList *list, void *value) {
    LLNode *node = malloc(sizeof(LLNode));
    node->value = value;
    node->_next = NULL;
    node->_prev = ll_last_node(list);

    list->_last->_next = node;
    list->_last = node;
    list->_size += 1;

    if (list->_head == NULL) list->_head = node;
}

void ll_remove(LLLinkedList *list, void *value) {
    LLNode *node = ll_get_node(list, value);
    if (node != NULL) ll_remove_node(list, node);
}

void ll_remove_node(LLLinkedList *list, LLNode *node) {
    if (list->_head == node) list->_head = node->_next;
    if (list->_last == node) list->_last = node->_prev;
    if (node->_prev != NULL) node->_prev->_next = node->_next;
    if (node->_next != NULL) node->_next->_prev = node->_prev;
    free(node);
    list->_size -= 1;
}

LLNode *ll_next(LLNode *node) {
    return node != NULL ? node->_next : NULL;
}

LLNode *ll_prev(LLNode *node) {
    return node != NULL ? node->_prev : NULL;
}

void ll_sort(LLLinkedList *list, ValueComparator cmp) {
    bool swapped;
    do {
        swapped = false;

        LLNode *node = ll_head_node(list);
        while (node != NULL && node->_next != NULL) {
            if (cmp(node->value, node->_next->value) > 0) {
                void *tmp = node->value;
                node->value = node->_next->value;
                node->_next->value = tmp;
                swapped = true;
            }
        }
    } while (swapped);
}
