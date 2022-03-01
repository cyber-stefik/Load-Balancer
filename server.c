/* Copyright 2021 <> */
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "Hashtable.h"

server_memory* init_server_memory() {
	server_memory* server = malloc(sizeof(server_memory));
	server->table = ht_create(420, hash_function_servers);
	return server;
}

void server_store(server_memory* server, char* key, char* value) {
	ht_put(server->table, key, strlen(key) + 1, value, strlen(value) + 1);
}

void server_remove(server_memory* server, char* key) {
	ht_remove_entry(server->table, key);
}

char* server_retrieve(server_memory* server, char* key) {
	return ht_get(server->table, key);
}

void free_server_memory(server_memory* server) {
	ht_free(server->table);
}
