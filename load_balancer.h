// Copyright 2021 Stefanita Ionita
#ifndef LOAD_BALANCER_H_
#define LOAD_BALANCER_H_

#include "server.h"

struct load_balancer;
typedef struct load_balancer load_balancer;

load_balancer* init_load_balancer();
unsigned int hash_function_key(void *a);
void free_load_balancer(load_balancer* main);

/**
 * load_store() - Stores the key-value pair inside the system.
 * @arg1: Load balancer which distributes the work.
 * @arg2: Key represented as a string.
 * @arg3: Value represented as a string.
 * @arg4: This function will RETURN via this parameter
 *        the server ID which stores the object.
 *
 * The load balancer will use Consistent Hashing to distribute the
 * load across the servers. The chosen server ID will be returned
 * using the last parameter.
 */
void loader_store(load_balancer* main, char* key, char* value, int* server_id);

/**
 * load_retrieve() - Gets a value associated with the key.
 * @arg1: Load balancer which distributes the work.
 * @arg2: Key represented as a string.
 * @arg3: This function will RETURN the server ID
          which stores the value via this parameter.
 *
 * The load balancer will search for the server which should posess the
 * value associated to the key. The server will return NULL in case
 * the key does NOT exist in the system.
 */
char* loader_retrieve(load_balancer* main, char* key, int* server_id);

/**
 * load_add_server() - Adds a new server to the system.
 * @arg1: Load balancer which distributes the work.
 * @arg2: ID of the new server.
 *
 * The load balancer will generate 3 replica TAGs and it will
 * place them inside the hash ring. The neighbor servers will
 * distribute some the objects to the added server.
 */
void loader_add_server(load_balancer* main, int server_id);

/**
 * load_remove_server() - Removes a specific server from the system.
 * @arg1: Load balancer which distributes the work.
 * @arg2: ID of the removed server.
 *
 * The load balancer will distribute ALL objects stored on the
 * removed server and will delete ALL replicas from the hash ring.
 */
void loader_remove_server(load_balancer* main, int server_id);

/**
 * Creeaza replicile serverului original cu id-ul server_id in functie de no,
 * reprezentand numarul replicii.
 */
server_memory* copy_server(server_memory* server, int server_id, int no);

/**
 * Adauga replica copy pe hashring.
 */
void add_on_hashring(load_balancer* main, server_memory* copy);

/**
 * Rebalanseaza obiectele dupa ce este adaugat un server.
 */
void remap(load_balancer* main, server_memory* copy);

/**
 * Elibereaza memoria alocata serverului cu id-ul server_id.
 */
void delete_server(load_balancer* main, int server_id);

#endif  // LOAD_BALANCER_H_
