// Copyright 2021 Stefanita Ionita
#ifndef CIRCULARDOUBLYLINKEDLIST_H_
#define CIRCULARDOUBLYLINKEDLIST_H_

typedef struct dll_node_t dll_node_t;
struct dll_node_t
{
    void* data;  // Pentru ca datele stocate sa poata avea orice tip, folosim un
                // pointer la void.
    dll_node_t *prev, *next;
};

typedef struct doubly_linked_list_t doubly_linked_list_t;
struct doubly_linked_list_t
{
    dll_node_t* head;
    unsigned int data_size;
    int size;
};

/*
 * Acestea sunt functiile pe care trebuie sa le implementam.
 * Implementarea acestora se va face in LinkedList.c .
 */

doubly_linked_list_t*
dll_create(unsigned int data_size);

dll_node_t*
get_nth_node(doubly_linked_list_t* list, int n);

void
add_nth_node(doubly_linked_list_t* list, int n, const void* data);

dll_node_t*
remove_nth_node(doubly_linked_list_t* list, int n);

unsigned int
dll_get_size(doubly_linked_list_t* list);

void
dll_free(doubly_linked_list_t** pp_list);
#endif  // CIRCULARDOUBLYLINKEDLIST_H_
