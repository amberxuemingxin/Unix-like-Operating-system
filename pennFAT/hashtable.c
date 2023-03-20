#include "hashtable.h"
#include <string.h>

static unsigned long hash(const char* str, size_t size) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % size;
}

HashTable* hashtable_create(size_t size) {
    HashTable* hashtable = malloc(sizeof(HashTable));
    hashtable->size = size;
    hashtable->nodes = calloc(size, sizeof(HashNode*));
    return hashtable;
}

void hashtable_destroy(HashTable* hashtable) {
    for (size_t i = 0; i < hashtable->size; i++) {
        HashNode* node = hashtable->nodes[i];
        if (node) {
            free(node->key);
            free(node);
        }
    }
    free(hashtable->nodes);
    free(hashtable);
}

int hashtable_get(HashTable* hashtable, const char* key) {
    unsigned long hash_value = hash(key, hashtable->size);
    HashNode* node = hashtable->nodes[hash_value];
    while (node && strcmp(node->key, key) != 0)
        node = node->next;
    return node ? node->value : 0;
}

void hashtable_set(HashTable* hashtable, const char* key, int value) {
    unsigned long hash_value = hash(key, hashtable->size);
    HashNode* node = hashtable->nodes[hash_value];
    while (node && strcmp(node->key, key) != 0)
        node = node->next;
    if (node) {
        node->value = value;
    } else {
        node = malloc(sizeof(HashNode));
        node->key = strdup(key);
        node->value = value;
        node->next = hashtable->nodes[hash_value];
        hashtable->nodes[hash_value] = node;
    }
}

void hashtable_remove(HashTable* hashtable, const char* key) {
    unsigned long hash_value = hash(key, hashtable->size);
    HashNode* node = hashtable->nodes[hash_value];
    HashNode* prev_node = NULL;
    while (node && strcmp(node->key, key) != 0) {
        prev_node = node;
        node = node->next;
    }
    if (node) {
        if (prev_node) {
            prev_node->next = node->next;
        } else {
            hashtable->nodes[hash_value] = node->next;
        }
        free(node->key);
        free(node);
    }
}
