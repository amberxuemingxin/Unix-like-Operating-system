#include "hashtable.h"
int main(int argc, char *argv[]) {
    HashTable* ht = hashtable_create(3);
    hashtable_set(ht, '1',15);
    hashtable_set(ht, '2',22);
    hashtable_set(ht, '3',33);
    hashtable_set(ht, '1',11);

    printf("%d\n", hashtable_get(ht,'1'));
    printf("%d\n", hashtable_get(ht,'2'));
    printf("%d\n", hashtable_get(ht,"3"));
    return EXIT_SUCCESS;
}