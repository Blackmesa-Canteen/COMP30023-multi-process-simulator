
#ifndef COMP30023_2021_PROJECT_1_MYHASHTABLE_H
#define COMP30023_2021_PROJECT_1_MYHASHTABLE_H

/**
 *  Original base version source: https://github.com/goldsborough/hashtable
 *  Customized version edited By: Xiaotian
 */

#include <stddef.h>

typedef int (*comparison_t)(void*, void*, size_t);
typedef size_t (*hash_t)(void*, size_t);

typedef struct TableNode {
    void* key;
    void* value;
    struct TableNode* next;

} TableNode;

typedef struct HashTable {
    size_t size;
    size_t threshold;
    size_t capacity;

    size_t key_size;
    size_t value_size;

    comparison_t compare;
    hash_t hash;

    TableNode** nodes;

} HashTable;


int InitHashtable(HashTable* table,
             size_t key_size,
             size_t value_size,
             size_t capacity);
int reserveCapacityOfHashtable(HashTable* table, size_t minimum_capacity);
int HashtableIsInitialized(HashTable* table);

int CleanAndDestroyHashtable(HashTable* table);

int InsertHashtable(HashTable* table, void* key, void* value);

int ContainsInHashtable(HashTable* table, void* key);
void* FindFromHashtable(HashTable* table, void* key);

#endif //COMP30023_2021_PROJECT_1_MYHASHTABLE_H
