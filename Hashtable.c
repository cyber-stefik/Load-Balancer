// Copyright 2021 Stefanita Ionita
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#include "Hashtable.h"

#define MAX_BUCKET_SIZE 64

/*
 * Functii de comparare a cheilor:
 */
int
compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

/*
 * Functii de hashing:
 */
unsigned int
hash_function_int(void *a)
{
	/*
	 * Credits: https://stackoverflow.com/a/12996028/7883884
	 */
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int
hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	int hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*))
{
	hashtable_t* new_hash;
	new_hash = malloc(sizeof(hashtable_t));
	new_hash->size = 0;
	new_hash->hmax = hmax;
	new_hash->hash_function = hash_function;
	if (!new_hash) {
		printf("Could not allocate the hashtable.\n");
	}
	unsigned int i = 0;
	new_hash->buckets = malloc(hmax * sizeof(linked_list_t*));
	if (!new_hash->buckets) {
		printf("Could not allocate buckets.\n");
	}
	while (i < hmax) {
		new_hash->buckets[i] = ll_create(sizeof(linked_list_t));
		if (!new_hash->buckets[i]) {
			printf("Could not allocate buckets[i].\n");
		}
		i++;
	}
	return new_hash;

	/* TODO */
}

/*
 * Atentie! Desi cheia este trimisa ca un void pointer (deoarece nu se impune tipul ei), in momentul in care
 * se creeaza o noua intrare in hashtable (in cazul in care cheia nu se gaseste deja in ht), trebuie creata o copie
 * a valorii la care pointeaza key si adresa acestei copii trebuie salvata in structura info asociata intrarii din ht.
 * Pentru a sti cati octeti trebuie alocati si copiati, folositi parametrul key_size_bytes.
 *
 * Motivatie:
 * Este nevoie sa copiem valoarea la care pointeaza key deoarece dupa un apel put(ht, key_actual, value_actual),
 * valoarea la care pointeaza key_actual poate fi alterata (de ex: *key_actual++). Daca am folosi direct adresa
 * pointerului key_actual, practic s-ar modifica din afara hashtable-ului cheia unei intrari din hashtable.
 * Nu ne dorim acest lucru, fiindca exista riscul sa ajungem in situatia in care nu mai stim la ce cheie este
 * inregistrata o anumita valoare.
 */
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	unsigned int index = ht->hash_function(key);
	index %= ht->hmax;
	ll_node_t* tmp2 = ht->buckets[index]->head;
	ht->buckets[index]->data_size = sizeof(struct info);
	int ok = 0;
	unsigned int i;
	for (i = 0; i < ht->buckets[index]->size; i++) {
		if (compare_function_strings(((struct info*)tmp2->data)->key, key) == 0) {
			memcpy(((struct info*)tmp2->data)->value, value, value_size);
			ok = 1;
			break;
		}
		tmp2 = tmp2->next;
	}
	if (ok == 0) {
		struct info tmp;
		tmp.key = malloc(key_size);
		if (!tmp.key) {
			printf("Could not allocate key.");
		}
		tmp.value = malloc(value_size);
		if (!tmp.value) {
			printf("Could not allocate value.\n");
		}
		memcpy(tmp.key, key, key_size);
		memcpy(tmp.value, value, value_size);
		ll_add_nth_node(ht->buckets[index], ht->size, &tmp);
		ht->size++;
	}
}

void *
ht_get(hashtable_t *ht, void *key)
{
	unsigned int index = ht->hash_function(key);
	index %= ht->hmax;
	unsigned int i = 0;
	ll_node_t* tmp = ht->buckets[index]->head;
	while (i < ht->buckets[index]->size) {
		if (compare_function_strings(((struct info*)(tmp->data))->key, key) == 0) {
			return ((struct info*)(tmp->data))->value;
		}
		tmp = tmp->next;
		i++;
	}
	/* TODO */
	return NULL;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable folosind functia put
 * 0, altfel.
 */
int
ht_has_key(hashtable_t *ht, void *key)
{
	unsigned int index = ht->hash_function(key);
	index %= ht->hmax;
	unsigned int i = 0;
	ll_node_t* tmp = ht->buckets[index]->head;
	while (i < ht->buckets[index]->size) {
		if (compare_function_strings(((struct info*)(tmp->data))->key, key) == 0) {
			return 1;
		}
		tmp = tmp->next;
		i++;
	}
	/* TODO */
	return 0;
}

/*
 * Procedura care elimina din hashtable intrarea asociata cheii key.
 * Atentie! Trebuie avuta grija la eliberarea intregii memorii folosite pentru o intrare din hashtable (adica memoria
 * pentru copia lui key --vezi observatia de la procedura put--, pentru structura info si pentru structura Node din
 * lista inlantuita).
 */
void
ht_remove_entry(hashtable_t *ht, void *key)
{
	unsigned int index = ht->hash_function(key);
	index %= ht->hmax;
	unsigned int i = 0, pos = 0;
	ll_node_t* tmp = ht->buckets[index]->head;
	for (i = 0; i < ht->buckets[index]->size; i++) {
		if (compare_function_strings(((struct info*)(tmp->data))->key, key) == 0) {
			pos++;
		}
		tmp = tmp->next;
	}
	ll_node_t* rmv = ll_remove_nth_node(ht->buckets[index], pos);
	free(((struct info*)(rmv->data))->key);
	free(((struct info*)(rmv->data))->value);
	free(rmv->data);
	free(rmv);
	ht->size--;
}

/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable, dupa care elibereaza si memoria folosita
 * pentru a stoca structura hashtable.
 */
void
ht_free(hashtable_t *ht)
{
	for (unsigned int i = 0; i < ht->hmax; i++) {
		unsigned int size = ht->buckets[i]->size;
		for (unsigned int j = 0; j < size; j++) {
			ll_node_t* rmv = ll_remove_nth_node(ht->buckets[i], j);
			free(((struct info*)(rmv->data))->key);
			free(((struct info*)(rmv->data))->value);
			free(rmv->data);
			free(rmv);
		}
		free(ht->buckets[i]);
	}
	free(ht->buckets);
	free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
