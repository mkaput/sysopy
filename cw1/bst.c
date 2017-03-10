#include "bst.h"

Bst *bst_new(ValueComparator comparator) {
    Bst *bst = malloc(sizeof(Bst));
    bst->_root = NULL;
    bst->_size = 0;
    bst->_comparator = comparator;
    return bst;
}

void bst_free(Bst *bst) {
    BstNode *node = bst_smallest_node(bst);
    while (node != NULL) {
        BstNode *next = bst_next(node);
        free(node);
        node = next;
    }
    free(bst);
}

int bst_compare(Bst *bst, void *a, void *b) {
    return bst->_comparator(a, b);
}

size_t bst_size(Bst *bst) {
    return bst->_size;
}

bool bst_contains(Bst *bst, void *value) {
    return bst_get_node(bst, value) != NULL;
}

BstNode *bst_root_node(Bst *bst) {
    return bst != NULL ? bst->_root : NULL;
}

BstNode *bst_smallest_node(Bst *bst) {
    BstNode *node = bst_root_node(bst);
    while (node != NULL) {
        if (node->_left == NULL) return node;
        node = node->_left;
    }
    return NULL;
}

BstNode *bst_largest_node(Bst *bst) {
    BstNode *node = bst_root_node(bst);
    while (node != NULL) {
        if (node->_right == NULL) return node;
        node = node->_right;
    }
    return NULL;
}

BstNode *bst_get_node(Bst *bst, void *value) {
    BstNode *node = bst_root_node(bst);
    while (node != NULL) {
        int cmp = bst_compare(bst, value, node->value);

        if (cmp == 0) {
            return node;
        } else if (cmp < 0) {
            node = bst_next(node);
        } else {
            node = bst_prev(node);
        }
    }
    return NULL;
}

BstNode *bst_add(Bst *bst, void *value) {
    BstNode *node = malloc(sizeof(BstNode));
    node->value = value;
    node->_parent = NULL;
    node->_left = NULL;
    node->_right = NULL;
}

void bst_remove(Bst *bst, void *value) {
    BstNode *node = bst_get_node(bst, value);
    bst_remove_node(bst, node);
}

void bst_remove_node(Bst *bst, BstNode *node) {

}

BstNode *bst_next(BstNode *node) {
    return NULL;
}

BstNode *bst_prev(BstNode *node) {
    return NULL;
}

