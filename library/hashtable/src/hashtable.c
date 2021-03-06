/**
 * @file hashtable.c
 * @author rick
 * @date 23.07.20
 * @brief File for the HashTable implementation
 */

#include "waitui/hashtable.h"

#include <stdlib.h>
#include <string.h>


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

/**
 * @brief Calculate the HashTable slot for the given key.
 * @param[in] this The HashTable to calculate the slot for
 * @param[in] key The key to calculate the slot for
 * @return The slot in the HashTable for the given key
 */
unsigned long int hashtable_hash(hashtable *this, str key) {
    unsigned long int hashValue = 0;

    if (key.len < 1 || !key.s) { return 0; }

    for (unsigned long int i = 0; i < key.len; ++i) { hashValue += key.s[i]; }
    hashValue += key.s[0] % 11 + (((unsigned char) key.s[0]) << 3U) - key.s[0];

    return hashValue % this->size;
}


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

hashtable *hashtable_new(unsigned long int size,
                         hashtable_value_destroy valueDestroyCallback) {
    hashtable *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->size                 = size;
    this->valueDestroyCallback = valueDestroyCallback;
    this->list                 = calloc(size, sizeof(this->list));
    if (!this->list) {
        hashtable_destroy(&this);
        return NULL;
    }

    for (unsigned long int i = 0; i < this->size; ++i) { this->list[i] = NULL; }

    return this;
}

void hashtable_destroy(hashtable **this) {
    if (!this || !(*this)) { return; }

    if ((*this)->list) {
        for (unsigned long int i = 0; i < (*this)->size; ++i) {
            while ((*this)->list[i]) {
                hashtable_node *temp = (*this)->list[i];
                (*this)->list[i]     = (*this)->list[i]->next;

                if (!temp->isStolen) {
                    (*this)->valueDestroyCallback(&temp->value);
                }

                STR_FREE(&temp->key);

                free(temp);
            }
        }
        free((*this)->list);
    }

    free(*this);
    *this = NULL;
}

int hashtable_insert_check(hashtable *this, str key, void *value,
                           hashtable_value_check valueCheckCallback,
                           void *arg) {
    if (!this) { return 0; }

    unsigned long int hashSlot = hashtable_hash(this, key);
    hashtable_node *node       = this->list[hashSlot];
    str keyCopy                = STR_NULL_INIT;

    while (node &&
           (key.len != node->key.len ||
            memcmp(key.s, node->key.s, key.len) != 0 ||
            (valueCheckCallback && !valueCheckCallback(node->value, arg)))) {
        node = node->next;
    }

    if (node) { return 0; }

    node = calloc(1, sizeof(*node));
    if (!node) { return 0; }

    STR_COPY(&keyCopy, &key);
    if (!keyCopy.s) {
        free(node);
        return 0;
    }

    node->key            = keyCopy;
    node->value          = value;
    node->next           = this->list[hashSlot];
    this->list[hashSlot] = node;

    return 1;
}

void *hashtable_lookup_check(hashtable *this, str key,
                             hashtable_value_check valueCheckCallback,
                             void *arg) {
    if (!this) { return NULL; }

    unsigned long int hashValue = hashtable_hash(this, key);
    hashtable_node *node        = this->list[hashValue];

    while (node &&
           (key.len != node->key.len ||
            memcmp(key.s, node->key.s, key.len) != 0 ||
            (valueCheckCallback && !valueCheckCallback(node->value, arg)))) {
        node = node->next;
    }

    if (!node) { return NULL; }

    return node->value;
}

int hashtable_has_check(hashtable *this, str key,
                        hashtable_value_check valueCheckCallback, void *arg) {
    if (!this) { return 0; }

    unsigned long int hashValue = hashtable_hash(this, key);
    hashtable_node *node        = this->list[hashValue];

    while (node &&
           (key.len != node->key.len ||
            memcmp(key.s, node->key.s, key.len) != 0 ||
            (valueCheckCallback && !valueCheckCallback(node->value, arg)))) {
        node = node->next;
    }

    return node != NULL;
}

int hashtable_mark_stolen_check(hashtable *this, str key,
                                hashtable_value_check valueCheckCallback,
                                void *arg) {
    if (!this) { return 0; }

    unsigned long int hashValue = hashtable_hash(this, key);
    hashtable_node *node        = this->list[hashValue];

    while (node &&
           (key.len != node->key.len ||
            memcmp(key.s, node->key.s, key.len) != 0 ||
            (valueCheckCallback && !valueCheckCallback(node->value, arg)))) {
        node = node->next;
    }

    if (!node) { return 0; }

    node->isStolen = 1;

    return 1;
}

int hashtable_insert(hashtable *this, str key, void *value) {
    return hashtable_insert_check(this, key, value, NULL, NULL);
}

void *hashtable_lookup(hashtable *this, str key) {
    return hashtable_lookup_check(this, key, NULL, NULL);
}

int hashtable_has(hashtable *this, str key) {
    return hashtable_has_check(this, key, NULL, NULL);
}

int hashtable_mark_stolen(hashtable *this, str key) {
    return hashtable_mark_stolen_check(this, key, NULL, NULL);
}
