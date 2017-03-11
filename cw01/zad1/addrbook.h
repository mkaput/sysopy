#ifndef ADDRBOOK_ADDRBOOK_H
#define ADDRBOOK_ADDRBOOK_H

#include "addr.h"
#include "linked_list.h"
#include "bst.h"
#include "utils.h"

typedef enum AddrBookBackend {
    ABB_LinkedList, ABB_Bst
} AddrBookBackend;

typedef enum AddrBookSorting {
    ABS_ById, ABS_ByName, ABS_ByPhone
} AddrBookSorting;

typedef struct AddrBook {
    AddrBookBackend _backend;
    union {
        struct {
            LLLinkedList *_ll;
        };
        struct {
            Bst *_bst;
        };
    };
} AddrBook;

typedef struct AddrBookInterator {
    AddrBook *_book;
    union {
        struct {
            LLNode *_cur_ll;
        };
        struct {
            BstNode *_cur_bst;
        };
    };
} AddrBookInterator;

AddrBook *addr_book_new(AddrBookBackend backend);

void addr_book_free(AddrBook *book);

void addr_book_add(AddrBook *book, Addr *addr);

void addr_book_remove(AddrBook *book, Addr *addr);

Addr *addr_book_search_by_id(AddrBook *book, int id);

Addr *addr_book_search_by_name(AddrBook *book, char name[]);

Addr *addr_book_search_by_phone(AddrBook *book, char phone[]);

void addr_book_rebuild(AddrBook *book, AddrBookBackend backend, AddrBookSorting sorting);

AddrBookInterator addr_book_iter(AddrBook *book);

bool addr_book_has_next(AddrBookInterator *iter);

AddrBookInterator addr_book_next(AddrBookInterator iter);

Addr *addr_book_iter_get(AddrBookInterator *iter);

#endif //ADDRBOOK_ADDRBOOK_H
