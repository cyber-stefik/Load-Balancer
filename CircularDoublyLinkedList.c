// Copyright 2021 Stefanita Ionita
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CircularDoublyLinkedList.h"
#include "utils.h"

/*
 * Functie care trebuie apelata pentru alocarea si initializarea unei liste.
 * (Setare valori initiale pentru campurile specifice structurii LinkedList).
 */
doubly_linked_list_t*
dll_create(unsigned int data_size)
{
    doubly_linked_list_t* list = malloc(sizeof(doubly_linked_list_t));
	if (!list) {
        printf("Could not allocate.\n");
    }
	list->data_size = data_size;
	list->size = 0;
    list->head = NULL;
	return list;
}

/*
 * Functia intoarce un pointer la nodul de pe pozitia n din lista.
 * Pozitiile din lista sunt indexate incepand cu 0 (i.e. primul nod din lista se
 * afla pe pozitia n=0). Daca n >= nr_noduri, atunci se intoarce nodul de pe
 * pozitia rezultata daca am "cicla" (posibil de mai multe ori) pe lista si am
 * trece de la ultimul nod, inapoi la primul si am continua de acolo. Cum putem
 * afla pozitia dorita fara sa simulam intreaga parcurgere? Daca n < 0, eroare.
 */
dll_node_t*
get_nth_node(doubly_linked_list_t* list, int n)
{
    int i = 0;
	if (n >= list->size) {
		n = n % list->size;
	}
	dll_node_t* tmp = list->head;
	while (i < n) {
		tmp = tmp->next;
		i++;
	}
	return tmp;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Cand indexam pozitiile nu "ciclam" pe lista circulara ca la
 * get, ci consideram nodurile in ordinea de la head la ultimul (adica acel nod
 * care pointeaza la head ca nod urmator in lista). Daca n >= nr_noduri, atunci
 * adaugam nodul nou la finalul listei. Daca n < 0, eroare.
 */
void
add_nth_node(doubly_linked_list_t* list, int n, const void* data)
{
    dll_node_t *node = malloc(sizeof(dll_node_t));
    dll_node_t *tmp;
    node->data = malloc(list->data_size);
    memcpy(node->data, data, list->data_size);

    if (list->size == 0) {  // initializare head cand list->size e 0
        list->head = node;
        list->head->next = list->head;
        list->head->prev = list->head;
        list->size++;
        return;
    } else if (n == 0 && list->size != 0) {  // adaugare nod pe pozitia 0 in
                                    // cazul in care mai exista elemente in list
        tmp = list->head;
        node->prev = tmp->prev;
        node->next = tmp;
        node->next->prev = node;
        node->prev->next = node;
        list->head = node;
        list->size++;
        return;
    } else if (n == list->size && list->size != 0) {  // adaugare nod pe pozitia
                                            // n in in cazul in care mai exista
                                            // elemente in list
        tmp = list->head;
        node->prev = tmp->prev;
        node->next = tmp;
        node->prev->next = node;
        node->next->prev = node;
        list->size++;
        return;
    } else {  // adaugare nod pe pozitia n, cand n nu este pozitia head sau tail
        tmp = get_nth_node(list, n);
        node->prev = tmp->prev;
        node->next = tmp;
        node->next->prev = node;
        node->prev->next = node;
        list->size++;
    }
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Functia intoarce un pointer spre acest nod
 * proaspat eliminat din lista. Daca n >= nr_noduri - 1, se elimina nodul de la
 * finalul listei. Daca n < 0, eroare. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
dll_node_t*
remove_nth_node(doubly_linked_list_t* list, int n)
{
    dll_node_t *rmv, *tmp;
    if (list->size == 0) {  // cazul in care nu exista elemente in list
        return NULL;
    }
    tmp = list->head;
    if (list->size == 1) {  // cazul in care exista un singur element in list
        list->head = NULL;
        list->size--;
        return tmp;
    }
    if (n == 0) {  // stergerea capului de lista
        rmv = list->head;
        rmv->prev->next = rmv->next;
        rmv->next->prev = rmv->prev;
        list->head = rmv->next;
        list->size--;
        return rmv;
    }
    if (n == list->size - 1) {  // stergerea ultimului element din lista,
                                // complexitate O(1)
        rmv = tmp->prev;
        rmv->prev->next = rmv->next;
        rmv->next->prev = rmv->prev;
        list->size--;
        return rmv;
    } else {  // stergerea nodului de pe pozitia n, cand n nu este pozitia
            // lui head sau tail
        rmv = get_nth_node(list, n);
        rmv->prev->next = rmv->next;
        rmv->next->prev = rmv->prev;
        list->size--;
        return rmv;
    }
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
dll_get_size(doubly_linked_list_t* list)
{
    return list->size;
    /* TODO */
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista.
 */
void
dll_free(doubly_linked_list_t** pp_list)
{
    dll_node_t *node = (*pp_list)->head;
    dll_node_t *tmp;
    for (int i = 0; i < (*pp_list)->size; i++) {
        tmp = node->next;
        free(node->data);
        free(node);
        node = tmp;
    }
    free(*pp_list);
    *pp_list = NULL;
}
