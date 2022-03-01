/* Copyright 2021 <> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "load_balancer.h"
#include "CircularDoublyLinkedList.h"

struct load_balancer {
    doubly_linked_list_t* hashring;
};

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

load_balancer* init_load_balancer() {
    // aloc memorie pentru laod_balancer
	load_balancer* load_balancer = malloc(sizeof(load_balancer));
    if (!load_balancer) {
        printf("Could not allocate the load balancer    .\n");
    }
    // creez hashring-ul
    load_balancer->hashring = dll_create(sizeof(server_memory));
    if (!load_balancer->hashring) {
        printf("Could not allocate the hashring.\n");
    }
    return load_balancer;
}

void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
    unsigned int k_hash = hash_function_key(key);
    dll_node_t* tmp = main->hashring->head;
    // verific daca k_hash e mai mare decat hash-ul ultimului server din lista
    if (k_hash > ((server_memory *)tmp->prev->data)->hash) {
        *server_id = ((server_memory *)main->hashring->head->data)->id
                    % 100000;
        // apelez functia server_store pentru a stoca obiectul pe capul listei
        server_store((server_memory *)main->hashring->head->data,
                            key, value);
        return;
    }
    // parcurg hashring-ul
    for (int i = 0; i < main->hashring->size; i++) {
        // conditie pentru primul server cu hash-ul mai mare decat k_hash
        if (k_hash < ((server_memory *)tmp->data)->hash) {
            *server_id = ((server_memory *)tmp->data)->id % 100000;
            // stochez obiectul pe serverul corespunzator
            server_store((server_memory *)tmp->data, key, value);
            return;
        }
        tmp = tmp->next;
    }
}

char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
	unsigned int k_hash = hash_function_key(key);
    dll_node_t* tmp = main->hashring->head;
    // verific daca k_hash e mai mare decat hash-ul ultimului server din lista
    if (k_hash > ((server_memory *)tmp->prev->data)->hash) {
        *server_id = ((server_memory *)main->hashring->head->data)->id
                    % 100000;
        // apelez functia server_retrieve pentru a obtine obiectul cerut
        return server_retrieve((server_memory *)main->hashring->head->data,
                            key);
    }
    // parcurg hashring-ul
    for (int i = 0; i < main->hashring->size; i++) {
        // conditie pentru primul server cu hash-ul mai mare decat k_hash
        if (k_hash < ((server_memory *)tmp->data)->hash) {
            *server_id = ((server_memory *)tmp->data)->id % 100000;
            // apelez functia server_retrieve pentru a obtine obiectul cerut
            return server_retrieve((server_memory *)tmp->data, key);
        }
        tmp = tmp->next;
    }
    return NULL;
}

server_memory* copy_server(server_memory* server, int server_id, int no) {
    server->id = no * pow(10, 5) + server_id;
    server->hash = hash_function_servers(&server->id);
    return server;
}

void add_on_hashring(load_balancer* main, server_memory* copy) {
    // cazul in care hashring-ul e gol
    if (main->hashring->size == 0) {
        add_nth_node(main->hashring, 0, copy);
        return;
    }
    dll_node_t* tmp = main->hashring->head;
    // parcurg hashring-ul
    for (int i = 0; i < main->hashring->size; i++) {
        // conditia pentru primul server mai mare decat hash-ul copiei
        if (copy->hash < ((server_memory *)tmp->data)->hash) {
            add_nth_node(main->hashring, i, copy);
            return;
        } else if (i == main->hashring->size - 1) {
            // cazul in care hash-ul e mai mare decat hash-ul ultimului server
            add_nth_node(main->hashring, main->hashring->size, copy);
            return;
        }
        tmp = tmp->next;
    }
}

void remap(load_balancer* main, server_memory* copy) {
    dll_node_t* tmp = main->hashring->head;
    struct info data;
    unsigned int object_hash;
    // caut serverul cu id-ul corespunzator
    while (((server_memory*)tmp->data)->id != copy->id)
        tmp = tmp->next;
    ll_node_t* tmp2;
    unsigned int i;
    int pl;
    // parcurg elementele serverului urmator pentru a le copia
    for (i = 0; i < ((server_memory*)tmp->next->data)->table->hmax; i++) {
        tmp2 = ((server_memory*)tmp->next->data)->table->buckets[i]->head;
        while (tmp2) {
            object_hash = hash_function_key(((struct info*)tmp2->data)->key);
            // cazul in care hash-ul obiectului e mai mic decat hash-ul
            // serverului nou adaugat
            if (object_hash < copy->hash) {
                data.key = malloc(strlen(((struct info*)tmp2->data)->key) + 1);
                memcpy(data.key, ((struct info*)tmp2->data)->key,
                    strlen(((struct info*)tmp2->data)->key) + 1);
                data.value = malloc(strlen(((struct info*)tmp2->data)->value)
                            + 1);
                memcpy(data.value, ((struct info*)tmp2->data)->value,
                    strlen(((struct info*)tmp2->data)->value) + 1);
                // stochez obiectul, apeland din nou loader_store
                loader_store(main, data.key, data.value, &pl);
                // eliberez memoria alocata cheii si valorii
                free(data.key);
                free(data.value);
            } else if (tmp == main->hashring->head &&
                hash_function_key(((struct info*)tmp2->data)->key) >=
                ((server_memory *)tmp->prev->data)->hash) {
                    // cazul in care hash-ul obiectului e mai mare decat hash-ul
                    // ultimului server
                    data.key = malloc(strlen(((struct info*)tmp2->data)->key)
                            + 1);
                    memcpy(data.key, ((struct info*)tmp2->data)->key,
                        strlen(((struct info*)tmp2->data)->key) + 1);
                    data.value = malloc(strlen(((struct info*)tmp2->data)
                    ->value) + 1);
                    memcpy(data.value, ((struct info*)tmp2->data)->value,
                        strlen(((struct info*)tmp2->data)->value) + 1);
                    loader_store(main, data.key, data.value, &pl);
                    free(data.key);
                    free(data.value);
                }
            tmp2 = tmp2->next;
        }
    }
}

void loader_add_server(load_balancer* main, int server_id) {
	server_memory *original_server, *copy1, *copy2;
    // initializez memoria pentru serverul original
    original_server = init_server_memory();
    // aloc memorie pentru replici
    copy1 = malloc(sizeof(server_memory));
    copy2 = malloc(sizeof(server_memory));
    // creez replica care trebuie adaugata pe hashring
    original_server = copy_server(original_server, server_id, 0);

    // copiez memoria din original_server pentru ambele copii
    memcpy(copy1, original_server, sizeof(server_memory));
    copy1 = copy_server(copy1, server_id, 1);

    memcpy(copy2, original_server, sizeof(server_memory));
    copy2 = copy_server(copy2, server_id, 2);
    // adaug replicile pe hashring si remapez obiectele
    add_on_hashring(main, original_server);
    remap(main, original_server);

    add_on_hashring(main, copy1);
    remap(main, copy1);

    add_on_hashring(main, copy2);
    remap(main, copy2);

    free(original_server);
    free(copy1);
    free(copy2);
}

void delete_server(load_balancer* main, int server_id) {
    int searching_id;
    // for de la 0 la 3 pentru ca stiu ca exista 3 replici pe hashring
    for (int j = 0; j < 3; j++) {
        dll_node_t* tmp = main->hashring->head;
    // parcurg hashring-ul
    for (int i = 0; i < main->hashring->size; i++) {
        searching_id = ((server_memory *)tmp->data)->id;
        // server gasit
        if (searching_id % 100000 == server_id) {
            // apelez remove_nth_node pentru a sterge copia de pe hashring
            dll_node_t* rmv = remove_nth_node(main->hashring, i);
            // daca id-ul e mai mic decat 100000 inseamna ca e serverul original
            // si trebuie sa folosesc functia de free_server_memory
            if (searching_id < 100000) {
                free_server_memory((server_memory *)rmv->data);
            }
            // eliberez memoria alocata
            free(rmv->data);
            free(rmv);
            break;
        }
        tmp = tmp->next;
    }
    }
}

void loader_remove_server(load_balancer* main, int server_id) {
    dll_node_t* tmp = main->hashring->head;
    struct info data[69420];  // cn stie cunoaste
    unsigned int no = 0, i;
    int dummy;
    // caut serverul
    while (((server_memory*)tmp->data)->id % 100000 != server_id)
        tmp = tmp->next;
    ll_node_t* tmp2;
    // parcurg obiectele serverului
    for (i = 0; i < ((server_memory*)tmp->data)->table->hmax; i++) {
        tmp2 = ((server_memory*)tmp->data)->table->buckets[i]->head;
        while (tmp2) {
            // copiez obiectele intr-un array
            data[no].key = malloc(strlen(((struct info*)tmp2->data)->key)
                            + 1);
            memcpy(data[no].key, ((struct info*)tmp2->data)->key,
                    strlen(((struct info*)tmp2->data)->key) + 1);
            data[no].value = malloc(strlen(((struct info*)tmp2->data)->value)
                                + 1);
            memcpy(data[no].value, ((struct info*)tmp2->data)->value,
                    strlen(((struct info*)tmp2->data)->value) + 1);
            tmp2 = tmp2->next;
            no++;
        }
    }
    // sterg serverul
    delete_server(main, server_id);
    // stochez din nou datele folosind loader_store si eliberez memoria alocata
    // in array
    for (i = 0; i < no; i++) {
        loader_store(main, data[i].key, data[i].value, &dummy);
        free(data[i].value);
        free(data[i].key);
    }
}

void free_load_balancer(load_balancer* main) {
    dll_node_t* tmp = main->hashring->head;
    // parcurg hashring-ul
    for (int i = 0; i < main->hashring->size; i++) {
        if (((server_memory *)tmp->data)->id < 100000)
            free_server_memory((server_memory *)tmp->data);
        tmp = tmp->next;
    }
    // eliberez hashring-ul
    dll_free(&main->hashring);
    // eliberez load_balancer
    free(main);
}
