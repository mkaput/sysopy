#include "bst.h"

Bst *bst_new(ValueComparator comparator) {
    Bst *bst = malloc(sizeof(Bst));
    bst->_root = NULL;
    bst->_size = 0;
    bst->_comparator = comparator;
    return bst;
}

void bst_free(Bst *bst) {
    BstNode *node = bst_minimum(bst);
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

BstNode *bst_minimum(Bst *bst) {
    return bst_sub_minimum(bst_root_node(bst));
}

BstNode *bst_maximum(Bst *bst) {
    return bst_sub_maximum(bst_root_node(bst));
}

BstNode *bst_sub_minimum(BstNode *node) {
    if (node == NULL) return NULL;
    while (node->_left != NULL) {
        node = node->_left;
    }
    return node;
}

BstNode *bst_sub_maximum(BstNode *node) {
    if (node == NULL) return NULL;
    while (node->_right != NULL) {
        node = node->_right;
    }
    return node;
}

BstNode *bst_get_node(Bst *bst, void *value) {
    BstNode *node = bst_root_node(bst);
    while (node != NULL) {
        int cmp = bst_compare(bst, value, node->value);

        if (cmp == 0) {
            return node;
        } else if (cmp < 0) {
            node = node->_left;
        } else {
            node = node->_right;
        }
    }
    return NULL;
}

BstNode *bst_add(Bst *bst, void *value) {
    BstNode *new_node = malloc(sizeof(BstNode));
    new_node->value = value;
    new_node->_parent = NULL;
    new_node->_left = NULL;
    new_node->_right = NULL;

    BstNode *node = bst_root_node(bst);
    while (node != NULL) {
        new_node->_parent = node;
        if (bst_compare(bst, value, node->value) < 0) {
            node = node->_left;
        } else {
            node = node->_right;
        }
    }

    if (new_node->_parent == NULL) {
        bst->_root = new_node;
    } else if (bst_compare(bst, value, new_node->_parent->value) < 0) {
        new_node->_parent->_left = new_node;
    } else {
        new_node->_parent->_right = new_node;
    }

    return new_node;
}

void bst_remove(Bst *bst, void *value) {
    BstNode *node = bst_get_node(bst, value);
    bst_remove_node(bst, node);
}

void _bst_transplant(Bst *bst, BstNode *u, BstNode *v) {
    if (u->_parent == NULL) {
        bst->_root = v;
    } else if (u == u->_parent->_left) {
        u->_parent->_left = v;
    } else {
        u->_parent->_right = v;
    }

    if (v != NULL) {
        v->_parent = u->_parent;
    }
}

void bst_remove_node(Bst *bst, BstNode *node) {
    if (node->_left == NULL) {
        _bst_transplant(bst, node, node->_right);
    } else if (node->_right == NULL) {
        _bst_transplant(bst, node, node->_left);
    } else {
        BstNode *x = bst_sub_minimum(node->_right);
        if (x->_parent != node) {
            _bst_transplant(bst, x, x->_right);
            x->_right = node->_right;
            x->_right->_parent = x;
        }
        _bst_transplant(bst, node, x);
        x->_left = node->_left;
        x->_left->_parent = x;
    }
}

BstNode *bst_next(BstNode *node) {
    if (node == NULL) return NULL;

    if (node->_right != NULL) {
        return bst_sub_minimum(node->_right);
    } else {
        BstNode *x = node->_parent;
        while (x != NULL && node == x->_right) {
            node = x;
            x = x->_parent;
        }
        return x;
    }
}

BstNode *bst_prev(BstNode *node) {
    if (node == NULL) return NULL;

    if (node->_left != NULL) {
        return bst_sub_maximum(node->_left);
    } else {
        BstNode *x = node->_parent;
        while (x != NULL && node == x->_left) {
            node = x;
            x = x->_parent;
        }
        return x;
    }
}
