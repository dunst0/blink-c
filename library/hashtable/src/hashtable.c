/**
 * @file hashtable.c
 * @author rick
 * @date 23.07.20
 * @brief File for the HashTable implementation
 */

#include "blink/hashtable.h"

#include <stdlib.h>
#include <string.h>


// -----------------------------------------------------------------------------
// Local functions
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

    return hashValue % this->length;
}


// -----------------------------------------------------------------------------
// Public functions
// -----------------------------------------------------------------------------

hashtable *hashtable_new(unsigned long int length,
                         hashtable_value_destroy valueDestroyCallback) {
    hashtable *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->length               = length;
    this->valueDestroyCallback = valueDestroyCallback;
    this->list                 = calloc(length, sizeof(this->list));
    if (!this->list) {
        free(this);
        return NULL;
    } else {
        for (unsigned long int i = 0; i < this->length; ++i) {
            this->list[i] = NULL;
        }
    }

    return this;
}

void hashtable_destroy(hashtable **this) {
    if (!this || !(*this)) { return; }

    for (unsigned long int i = 0; i < (*this)->length; ++i) {
        while ((*this)->list[i]) {
            hashtable_node *temp = (*this)->list[i];
            (*this)->list[i]     = (*this)->list[i]->next;

            (*this)->valueDestroyCallback(&temp->value);
            free(temp->key.s);
            free(temp);
        }
    }

    free((*this)->list);

    free(*this);
    *this = NULL;
}

int hashtable_insert(hashtable *this, str key, void *value) {
    if (!this) { return 0; }

    unsigned long int hashSlot = hashtable_hash(this, key);
    hashtable_node *node       = this->list[hashSlot];
    str keyCopy                = STR_NULL;

    while (node && (key.len != node->key.len ||
                    memcmp(key.s, node->key.s, key.len) != 0)) {
        node = node->next;
    }

    if (node) { return 0; }

    node = calloc(1, sizeof(*node));
    if (!node) { return 0; }

    keyCopy.len = key.len;
    keyCopy.s   = calloc(keyCopy.len, sizeof(*keyCopy.s));
    if (!keyCopy.s) {
        free(node);
        return 0;
    }
    memcpy(keyCopy.s, key.s, key.len);

    node->key            = keyCopy;
    node->value          = value;
    node->next           = this->list[hashSlot];
    this->list[hashSlot] = node;

    return 1;
}

void *hashtable_lookup(hashtable *this, str key) {
    if (!this) { return NULL; }

    unsigned long int hashValue = hashtable_hash(this, key);
    hashtable_node *node        = this->list[hashValue];

    while (node && (key.len != node->key.len ||
                    memcmp(key.s, node->key.s, key.len) != 0)) {
        node = node->next;
    }

    if (!node) { return NULL; }

    return node->value;
}

int hashtable_has(hashtable *this, str key) {
    if (!this) { return 0; }

    unsigned long int hashValue = hashtable_hash(this, key);
    hashtable_node *node        = this->list[hashValue];

    while (node && (key.len != node->key.len ||
                    memcmp(key.s, node->key.s, key.len) != 0)) {
        node = node->next;
    }

    return node != NULL;
}
