//
// Created by Xiaotian on 2021/3/23.
//

/**
 *  Original base version source: https://github.com/goldsborough/hashtable
 *  Customized version edited By: Xiaotian
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myHashtable.h"

#define MINIMUM_CAPACITY 8

size_t defaultHash(void* key, size_t key_size);
int defaultCompare(void* first_key, void* second_key, size_t key_size);
size_t htHash(const HashTable* table, void* key);
int htEqual(const HashTable* table, void* first_key, void* second_key);
int htShouldGrow(HashTable* table);
TableNode* htCreateNode(HashTable* table, void* key, void* value, TableNode* next);
int htPushFront(HashTable* table, size_t index, void* key, void* value);
void htDestroyNode(TableNode* node);
int htAdjustCapacity(HashTable* table);
int htAllocate(HashTable* table, size_t capacity);
int htResize(HashTable* table, size_t new_capacity);
void htRehash(HashTable* table, TableNode** old, size_t old_capacity);

int InitHashtable(HashTable* table,
                  size_t keySize,
                  size_t valueSize,
                  size_t capacity) {
    if(table == NULL) {
        printf("Init hashtable error!\n");
        return -1;
    }

    // set initial capacity of hashtable
    if (capacity < MINIMUM_CAPACITY) {
        capacity = MINIMUM_CAPACITY;
    }

    if (htAllocate(table, capacity) == -1) {
        printf("Init allocate hashtable error!\n");
        return -1;
    }

    table->key_size = keySize;
    table->value_size = valueSize;

    table->hash = defaultHash;
    table->compare = defaultCompare;
    table->size = 0;

    return 0;
}

int reserveCapacityOfHashtable(HashTable* table, size_t minimumCapacity) {
    if (!HashtableIsInitialized(table)) {
        printf("Reserve capacity of hashtable error!\n");
        return -1;
    }
    if (minimumCapacity > table->threshold) {
        return htResize(table, minimumCapacity / 5);
    }

    return 0;
}
int HashtableIsInitialized(HashTable* table) {
    if(table != NULL && table->nodes != NULL) {
        return 1;
    }

    return 0;
}

int CleanAndDestroyHashtable(HashTable* table) {
    if (table == NULL || table->nodes == NULL) {
        printf("Destoy hashtable error!\n");
        return -1;
    }

    if(!HashtableIsInitialized(table)) {
        printf("Destoy hashtable error!\n");
        return -1;
    }

    TableNode* nodePtr;
    TableNode* nextPtr;
    size_t chain;

    // travel all linked list to free the table
    for (chain = 0; chain < table->capacity; chain++) {
        nodePtr = table->nodes[chain];
        while (nodePtr != NULL) {
            nextPtr = nodePtr->next;
            htDestroyNode(nodePtr);
            nodePtr = nextPtr;
        }
    }

    // TableNode**
    free(table->nodes);

    return 0;
}

int InsertHashtable(HashTable* table, void* key, void* value) {
    if(!HashtableIsInitialized(table) || key == NULL) {
        printf("Insert hashtable error!\n");
        return -1;
    }

    if (htShouldGrow(table)) {
        htAdjustCapacity(table);
    }

    size_t index = htHash(table, key);
    TableNode* node;

    for (node = table->nodes[index]; node; node = node->next) {
        // if has key, update
        if (htEqual(table, key, node->key)) {
            memcpy(node->value, value, table->value_size);

            // updated
            return 1;
        }
    }

    // if don't have this key
    // if failed to ad new node in the table
    if (htPushFront(table, index, key, value) == -1) {
        return -1;
    }
    table->size += 1;

    // add new key-value
    return 0;
}

int ContainsInHashtable(HashTable* table, void* key) {
    if (!HashtableIsInitialized(table) || key == NULL) {
        printf("detect contain in hashtable error!\n");
        return -1;
    }
    size_t index = htHash(table, key);
    TableNode* node;

    for (node = table->nodes[index]; node; node = node->next) {
        if (htEqual(table, key, node->key)) {

            // found the key
            return 1;
        }
    }

    // not found
    return 0;
}

void* FindFromHashtable(HashTable* table, void* key) {
    if(table == NULL || key == NULL) {
        return NULL;
    }

    size_t index = htHash(table, key);
    TableNode * node;

    // travel list and find the key-value, return value
    for (node = table->nodes[index]; node; node = node->next) {
        if (htEqual(table, key, node->key)) {
            return node->value;
        }
    }

    // not found
    return NULL;
}


size_t defaultHash(void* raw_key, size_t key_size) {
    // hash algorithm: www.cse.yorku.ca/~oz/hash.ssml
    size_t hash = 5381;
    size_t byte;
    char* key = raw_key;

    for (byte = 0; byte < key_size; ++byte) {
        hash = ((hash << 5) + hash) ^ key[byte];
    }

    return hash;
}
int defaultCompare(void* first_key, void* second_key, size_t key_size) {
    return memcmp(first_key, second_key, key_size);
}
size_t htHash(const HashTable* table, void* key) {
    size_t res = table->hash(key, table->key_size) % table->capacity;

    return res;
}
int htEqual(const HashTable* table, void* first_key, void* second_key) {
    int res = table->compare(first_key, second_key, table->key_size) == 0;
    return res;
}
int htShouldGrow(HashTable* table) {
    if(table->size > table->capacity) {
        printf("check increment of table-> size!\n");
        exit(1);
    }

    // if size reached capacity, return 1, means ht should grow
    return table->size == table->capacity;
}

TableNode* htCreateNode(HashTable* table, void* key, void* value, TableNode* next) {
    if(table == NULL || key == NULL || value == NULL) {
        printf("failed to create hashtable node \n");
        exit(1);
    }

    TableNode* newNode;
    if ((newNode = malloc(sizeof *newNode)) == NULL) {
        return NULL;
    }
    if ((newNode->key = malloc(table->key_size)) == NULL) {
        return NULL;
    }
    if ((newNode->value = malloc(table->value_size)) == NULL) {
        return NULL;
    }

    memcpy(newNode->key, key, table->key_size);
    memcpy(newNode->value, value, table->value_size);
    newNode->next = next;

    return newNode;
}

int htPushFront(HashTable* table, size_t index, void* key, void* value) {
    table->nodes[index] = htCreateNode(table, key, value, table->nodes[index]);

    // if managed push in, return 0
    return table->nodes[index] == NULL ? -1 : 0;
}
void htDestroyNode(TableNode* node) {
    if(node == NULL) {
        printf("failed to destroy hashtable node, node is NULL \n");
        exit(1);
    }

    free(node->key);
    free(node->value);
    free(node);
    node = NULL;
}
int htAdjustCapacity(HashTable* table) {
    // double size
    return htResize(table, table->size * 2);
}
int htAllocate(HashTable* table, size_t capacity) {
    if ((table->nodes = malloc(capacity * sizeof(TableNode *))) == NULL) {
        return -1;
    }
    memset(table->nodes, 0, capacity * sizeof(TableNode *));

    table->capacity = capacity;
    table->threshold = capacity * 5;

    return 0;
}
int htResize(HashTable* table, size_t new_capacity) {
    TableNode ** old;
    size_t old_capacity;

    if (new_capacity < 8) {
        if (table->capacity > 8) {
            new_capacity = 8;
        } else {
            /* min capacity is 8 */
            return 0;
        }
    }

    old = table->nodes;
    old_capacity = table->capacity;
    if (htAllocate(table, new_capacity) == -1) {
        return -1;
    }

    // rehash all key-value to new capacity of table
    htRehash(table, old, old_capacity);
    free(old);

    return 0;
}
void htRehash(HashTable* table, TableNode** old, size_t old_capacity) {
    TableNode* node;
    TableNode* next;
    size_t new_index;
    size_t chain;

    for (chain = 0; chain < old_capacity; ++chain) {
        for (node = old[chain]; node;) {
            next = node->next;

            new_index = htHash(table, node->key);
            node->next = table->nodes[new_index];
            table->nodes[new_index] = node;

            node = next;
        }
    }
}
