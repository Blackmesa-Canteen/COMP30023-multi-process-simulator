/**
 * Original Author: http://www.goldsborough.me/
 * Original source: https://github.com/goldsborough/hashtable
 * Edit By: Xiaotian
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

int ht_setup(HashTable* table,
             size_t key_size,
             size_t value_size,
             size_t capacity) {
    assert(table != NULL);

    if (table == NULL) return HT_ERROR;

    if (capacity < HT_MINIMUM_CAPACITY) {
        capacity = HT_MINIMUM_CAPACITY;
    }

    if (_ht_allocate(table, capacity) == HT_ERROR) {
        return HT_ERROR;
    }

    table->key_size = key_size;
    table->value_size = value_size;
    table->hash = _ht_default_hash;
    table->compare = _ht_default_compare;
    table->size = 0;

    return HT_SUCCESS;
}

int ht_destroy(HashTable* table) {
    HTNode* node;
    HTNode* next;
    size_t chain;

    assert(ht_is_initialized(table));
    if (!ht_is_initialized(table)) return HT_ERROR;

    for (chain = 0; chain < table->capacity; ++chain) {
        node = table->nodes[chain];
        while (node) {
            next = node->next;
            _ht_destroy_node(node);
            node = next;
        }
    }

    free(table->nodes);

    return HT_SUCCESS;
}

int ht_insert(HashTable* table, void* key, void* value) {
    size_t index;
    HTNode* node;

    assert(ht_is_initialized(table));
    assert(key != NULL);

    if (!ht_is_initialized(table)) return HT_ERROR;
    if (key == NULL) return HT_ERROR;

    if (_ht_should_grow(table)) {
        _ht_adjust_capacity(table);
    }

    index = _ht_hash(table, key);
    for (node = table->nodes[index]; node; node = node->next) {
        if (_ht_equal(table, key, node->key)) {
            memcpy(node->value, value, table->value_size);
            return HT_UPDATED;
        }
    }

    if (_ht_push_front(table, index, key, value) == HT_ERROR) {
        return HT_ERROR;
    }

    ++table->size;

    return HT_INSERTED;
}

int ht_contains(HashTable* table, void* key) {
    size_t index;
    HTNode* node;

    assert(ht_is_initialized(table));
    assert(key != NULL);

    if (!ht_is_initialized(table)) return HT_ERROR;
    if (key == NULL) return HT_ERROR;

    index = _ht_hash(table, key);
    for (node = table->nodes[index]; node; node = node->next) {
        if (_ht_equal(table, key, node->key)) {
            return HT_FOUND;
        }
    }

    return HT_NOT_FOUND;
}

void* ht_lookup(HashTable* table, void* key) {
    HTNode* node;
    size_t index;

    assert(table != NULL);
    assert(key != NULL);

    if (table == NULL) return NULL;
    if (key == NULL) return NULL;

    index = _ht_hash(table, key);
    for (node = table->nodes[index]; node; node = node->next) {
        if (_ht_equal(table, key, node->key)) {
            return node->value;
        }
    }

    return NULL;
}

int ht_clear(HashTable* table) {
    assert(table != NULL);
    assert(table->nodes != NULL);

    if (table == NULL) return HT_ERROR;
    if (table->nodes == NULL) return HT_ERROR;

    ht_destroy(table);
    _ht_allocate(table, HT_MINIMUM_CAPACITY);
    table->size = 0;

    return HT_SUCCESS;
}

int ht_is_empty(HashTable* table) {
    assert(table != NULL);
    if (table == NULL) return HT_ERROR;
    return table->size == 0;
}

bool ht_is_initialized(HashTable* table) {
    return table != NULL && table->nodes != NULL;
}

int ht_reserve(HashTable* table, size_t minimum_capacity) {
    assert(ht_is_initialized(table));
    if (!ht_is_initialized(table)) return HT_ERROR;

    /*
     * We expect the "minimum capacity" to be in elements, not in array indices.
     * This encapsulates the design.
     */
    if (minimum_capacity > table->threshold) {
        return _ht_resize(table, minimum_capacity / HT_LOAD_FACTOR);
    }

    return HT_SUCCESS;
}

/****************** PRIVATE ******************/

void _ht_int_swap(size_t* first, size_t* second) {
    size_t temp = *first;
    *first = *second;
    *second = temp;
}

void _ht_pointer_swap(void** first, void** second) {
    void* temp = *first;
    *first = *second;
    *second = temp;
}

int _ht_default_compare(void* first_key, void* second_key, size_t key_size) {
    return memcmp(first_key, second_key, key_size);
}

size_t _ht_default_hash(void* raw_key, size_t key_size) {
    // djb2 string hashing algorithm
    // sstp://www.cse.yorku.ca/~oz/hash.ssml
    size_t byte;
    size_t hash = 5381;
    char* key = raw_key;

    for (byte = 0; byte < key_size; ++byte) {
        // (hash << 5) + hash = hash * 33
        hash = ((hash << 5) + hash) ^ key[byte];
    }

    return hash;
}

size_t _ht_hash(const HashTable* table, void* key) {
#ifdef HT_USING_POWER_OF_TWO
    return table->hash(key, table->key_size) & table->capacity;
#else
    return table->hash(key, table->key_size) % table->capacity;
#endif
}

bool _ht_equal(const HashTable* table, void* first_key, void* second_key) {
    return table->compare(first_key, second_key, table->key_size) == 0;
}

bool _ht_should_grow(HashTable* table) {
    assert(table->size <= table->capacity);
    return table->size == table->capacity;
}

bool _ht_should_shrink(HashTable* table) {
    assert(table->size <= table->capacity);
    return table->size == table->capacity * HT_SHRINK_THRESHOLD;
}

HTNode*
_ht_create_node(HashTable* table, void* key, void* value, HTNode* next) {
    HTNode* node;

    assert(table != NULL);
    assert(key != NULL);
    assert(value != NULL);

    if ((node = malloc(sizeof *node)) == NULL) {
        return NULL;
    }
    if ((node->key = malloc(table->key_size)) == NULL) {
        return NULL;
    }
    if ((node->value = malloc(table->value_size)) == NULL) {
        return NULL;
    }

    memcpy(node->key, key, table->key_size);
    memcpy(node->value, value, table->value_size);
    node->next = next;

    return node;
}

int _ht_push_front(HashTable* table, size_t index, void* key, void* value) {
    table->nodes[index] = _ht_create_node(table, key, value, table->nodes[index]);
    return table->nodes[index] == NULL ? HT_ERROR : HT_SUCCESS;
}

void _ht_destroy_node(HTNode* node) {
    assert(node != NULL);

    free(node->key);
    free(node->value);
    free(node);
}

int _ht_adjust_capacity(HashTable* table) {
    return _ht_resize(table, table->size * HT_GROWTH_FACTOR);
}

int _ht_allocate(HashTable* table, size_t capacity) {
    if ((table->nodes = malloc(capacity * sizeof(HTNode*))) == NULL) {
        return HT_ERROR;
    }
    memset(table->nodes, 0, capacity * sizeof(HTNode*));

    table->capacity = capacity;
    table->threshold = capacity * HT_LOAD_FACTOR;

    return HT_SUCCESS;
}

int _ht_resize(HashTable* table, size_t new_capacity) {
    HTNode** old;
    size_t old_capacity;

    if (new_capacity < HT_MINIMUM_CAPACITY) {
        if (table->capacity > HT_MINIMUM_CAPACITY) {
            new_capacity = HT_MINIMUM_CAPACITY;
        } else {
            /* NO-OP */
            return HT_SUCCESS;
        }
    }

    old = table->nodes;
    old_capacity = table->capacity;
    if (_ht_allocate(table, new_capacity) == HT_ERROR) {
        return HT_ERROR;
    }

    _ht_rehash(table, old, old_capacity);

    free(old);

    return HT_SUCCESS;
}

void _ht_rehash(HashTable* table, HTNode** old, size_t old_capacity) {
    HTNode* node;
    HTNode* next;
    size_t new_index;
    size_t chain;

    for (chain = 0; chain < old_capacity; ++chain) {
        for (node = old[chain]; node;) {
            next = node->next;

            new_index = _ht_hash(table, node->key);
            node->next = table->nodes[new_index];
            table->nodes[new_index] = node;

            node = next;
        }
    }
}
