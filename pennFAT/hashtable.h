#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>

typedef struct {
    char* key;
    int value;
} HashNode;

typedef struct {
    size_t size;
    HashNode** nodes;
} HashTable;

HashTable* hashtable_create(size_t size);
void hashtable_destroy(HashTable* hashtable);
int hashtable_get(HashTable* hashtable, const char* key);
void hashtable_set(HashTable* hashtable, const char* key, int value);
void hashtable_remove(HashTable* hashtable, const char* key);

#endif /* HASHTABLE_H */
