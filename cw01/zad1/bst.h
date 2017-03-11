#ifndef ADDRBOOK_BST_H
#define ADDRBOOK_BST_H

#include "stdlib.h"
#include "utils.h"

typedef struct BstNode {
    void *value;
    struct BstNode *_parent;
    struct BstNode *_left;
    struct BstNode *_right;
} BstNode;

typedef struct Bst {
    BstNode *_root;
    size_t _size;
    ValueComparator _comparator;
} Bst;

Bst *bst_new(ValueComparator comparator);

void bst_free(Bst *bst);

int bst_compare(Bst *bst, void *a, void *b);

size_t bst_size(Bst *bst);

bool bst_contains(Bst *bst, void *value);

BstNode *bst_root_node(Bst *bst);

BstNode *bst_minimum(Bst *bst);

BstNode *bst_maximum(Bst *bst);

BstNode *bst_sub_minimum(BstNode *node);

BstNode *bst_sub_maximum(BstNode *node);

BstNode *bst_get_node(Bst *bst, void *value);

BstNode *bst_add(Bst *bst, void *value);

void bst_remove(Bst *bst, void *value);

void bst_remove_node(Bst *bst, BstNode *node);

BstNode *bst_next(BstNode *node);

BstNode *bst_prev(BstNode *node);

#endif //ADDRBOOK_BST_H