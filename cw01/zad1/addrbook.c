#include "addrbook.h"

#include <string.h>
#include <stdio.h>

AddrBook *addr_book_new(AddrBookBackend backend) {
    if (backend == ABB_LinkedList) {
        AddrBook *book = malloc(sizeof(AddrBook));
        book->_backend = backend;
        book->_ll = ll_new();
        return book;
    } else if (backend == ABB_Bst) {
        AddrBook *book = malloc(sizeof(AddrBook));
        book->_backend = backend;
        book->_bst = bst_new(addr_compare_by_id);
        return book;
    } else {
        perror("unknown backend");
        return NULL;
    }
}

void addr_book_free(AddrBook *book) {
    for (AddrBookIterator iter = addr_book_iter(book); addr_book_has_next(&iter); iter = addr_book_next(iter)) {
        addr_free(addr_book_iter_get(&iter));
    }

    if (book->_backend == ABB_LinkedList) {
        ll_free(book->_ll);
    } else if (book->_backend == ABB_Bst) {
        bst_free(book->_bst);
    } else {
        perror("unknown backend");
        abort();
    }

    free(book);
}

void addr_book_add(AddrBook *book, Addr *addr) {
    if (book->_backend == ABB_LinkedList) {
        ll_append(book->_ll, addr);
    } else if (book->_backend == ABB_Bst) {
        bst_add(book->_bst, addr);
    } else {
        perror("unknown backend");
        abort();
    }
}

void addr_book_remove(AddrBook *book, Addr *addr) {
    if (book->_backend == ABB_LinkedList) {
        ll_remove(book->_ll, addr);
    } else if (book->_backend == ABB_Bst) {
        bst_remove(book->_bst, addr);
    } else {
        perror("unknown backend");
        abort();
    }
    addr_free(addr);
}

Addr *addr_book_search_by_id(AddrBook *book, int id) {
    if (book->_backend == ABB_Bst && book->_bst->_comparator == addr_compare_by_id) {
        Addr fake_addr;
        fake_addr.id = id;
        fake_addr.name = NULL;
        fake_addr.phone = NULL;

        BstNode *node = bst_get_node(book->_bst, &fake_addr);
        return node != NULL ? node->value : NULL;
    }

    for (AddrBookIterator iter = addr_book_iter(book); addr_book_has_next(&iter); iter = addr_book_next(iter)) {
        Addr *addr = addr_book_iter_get(&iter);
        if (addr->id == id) {
            return addr;
        }
    }

    return NULL;
}

Addr *addr_book_search_by_name(AddrBook *book, char name[]) {
    if (book->_backend == ABB_Bst && book->_bst->_comparator == addr_compare_by_name) {
        Addr fake_addr;
        fake_addr.id = 0;
        fake_addr.name = name;
        fake_addr.phone = NULL;

        BstNode *node = bst_get_node(book->_bst, &fake_addr);
        return node != NULL ? node->value : NULL;
    }

    for (AddrBookIterator iter = addr_book_iter(book); addr_book_has_next(&iter); iter = addr_book_next(iter)) {
        Addr *addr = addr_book_iter_get(&iter);
        if (strcmp(addr->name, name) == 0) {
            return addr;
        }
    }

    return NULL;
}

Addr *addr_book_search_by_phone(AddrBook *book, char phone[]) {
    if (book->_backend == ABB_Bst && book->_bst->_comparator == addr_compare_by_phone) {
        Addr fake_addr;
        fake_addr.id = 0;
        fake_addr.name = NULL;
        fake_addr.phone = phone;

        BstNode *node = bst_get_node(book->_bst, &fake_addr);
        return node != NULL ? node->value : NULL;
    }

    for (AddrBookIterator iter = addr_book_iter(book); addr_book_has_next(&iter); iter = addr_book_next(iter)) {
        Addr *addr = addr_book_iter_get(&iter);
        if (strcmp(addr->phone, phone) == 0) {
            return addr;
        }
    }

    return NULL;
}

ValueComparator _get_comparator(AddrBookSorting sorting) {
    if (sorting == ABS_ById) {
        return addr_compare_by_id;
    } else if (sorting == ABS_ByName) {
        return addr_compare_by_name;
    } else if (sorting == ABS_ByPhone) {
        return addr_compare_by_phone;
    } else {
        perror("unknown backend");
        abort();
        return NULL;
    }
}

void addr_book_rebuild(AddrBook *book, AddrBookBackend backend, AddrBookSorting sorting) {
    ValueComparator cmp = _get_comparator(sorting);

    // If we are only changing sorting strategy for linked list, we only have to sort it.
    if (backend == ABB_LinkedList && book->_backend == ABB_LinkedList) {
        ll_sort(book->_ll, cmp);
        return;
    }

    // Otherwise we have to rebuild our inner data structure from scratch.
    AddrBook tmp;
    tmp._backend = backend;
    if (tmp._backend == ABB_LinkedList) {
        tmp._ll = ll_new();
    } else if (tmp._backend == ABB_Bst) {
        tmp._bst = bst_new(cmp);
    } else {
        perror("unknown backend");
        abort();
    }

    for (AddrBookIterator iter = addr_book_iter(book); addr_book_has_next(&iter); iter = addr_book_next(iter)) {
        addr_book_add(&tmp, addr_book_iter_get(&iter));
    }

    if (tmp._backend == ABB_LinkedList) {
        ll_sort(tmp._ll, cmp);
    }

    // Free existing data structure. It doesn't own its elements so freeing it is safe.
    if (book->_backend == ABB_LinkedList) {
        ll_free(book->_ll);
    } else if (book->_backend == ABB_Bst) {
        bst_free(book->_bst);
    } else {
        perror("unknown backend");
        abort();
    }

    // Now move tmp backend to book
    book->_backend = backend;
    if (book->_backend == ABB_LinkedList) {
        book->_ll = tmp._ll;
    } else if (book->_backend == ABB_Bst) {
        book->_bst = tmp._bst;
    } else {
        perror("unknown backend");
        abort();
    }
}

AddrBookIterator addr_book_iter(AddrBook *book) {
    AddrBookIterator result;
    result._book = book;
    if (result._book->_backend == ABB_LinkedList) {
        result._cur_ll = ll_head_node(result._book->_ll);
    } else if (result._book->_backend == ABB_Bst) {
        result._cur_bst = bst_minimum(result._book->_bst);
    } else {
        perror("unknown backend");
        abort();
    }
    return result;
}

bool addr_book_has_next(AddrBookIterator *iter) {
    if (iter->_book->_backend == ABB_LinkedList) {
        return iter->_cur_ll != NULL;
    } else if (iter->_book->_backend == ABB_Bst) {
        return iter->_cur_bst != NULL;
    } else {
        perror("unknown backend");
        abort();
        return false;
    }
}

AddrBookIterator addr_book_next(AddrBookIterator iter) {
    AddrBookIterator result;
    result._book = iter._book;
    if (result._book->_backend == ABB_LinkedList) {
        result._cur_ll = ll_next(iter._cur_ll);
    } else if (result._book->_backend == ABB_Bst) {
        result._cur_bst = bst_next(iter._cur_bst);
    } else {
        perror("unknown backend");
        abort();
    }
    return result;
}

Addr *addr_book_iter_get(AddrBookIterator *iter) {
    if (iter->_book->_backend == ABB_LinkedList) {
        return (Addr *) iter->_cur_ll->value;
    } else if (iter->_book->_backend == ABB_Bst) {
        return (Addr *) iter->_cur_bst->value;
    } else {
        perror("unknown backend");
        abort();
        return NULL;
    }
}
