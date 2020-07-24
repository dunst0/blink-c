/**
 * @file hashtable.h
 * @author rick
 * @date 23.07.20
 * @brief File for the HashTable implementation
 */

#ifndef BLINK_HASHTABLE_H
#define BLINK_HASHTABLE_H

#include <blink/str.h>


// -----------------------------------------------------------------------------
//  Types
// -----------------------------------------------------------------------------

/**
 * @brief Type representing a HashTable node.
 */
typedef struct hashtable_node hashtable_node;
struct hashtable_node {
    str key;
    void *value;
    hashtable_node *next;
};

/**
 * @brief Type for element destroy function.
 */
typedef void (*hashtable_value_destroy)(void **value);

/**
 * @brief Type representing a HashTable.
 */
typedef struct hashtable {
    hashtable_node **list;
    hashtable_value_destroy valueDestroyCallback;
    unsigned long int length;
} hashtable;


// -----------------------------------------------------------------------------
// Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create a HashTable.
 * @param[in] length The HashTable size
 * @param[in] valueDestroyCallback Function to call for value destruction
 * @return A pointer to hashtable or NULL if memory allocation failed
 */
extern hashtable *hashtable_new(unsigned long int length,
                                hashtable_value_destroy valueDestroyCallback);

/**
 * @brief Destroy a HashTable.
 * @param[in,out] this The HashTable to destroy
 * @note This will free call for every value the valueDestroyCallback
 */
extern void hashtable_destroy(hashtable **this);

/**
 * @brief Insert a value for the key into the HashTable.
 * @param[in,out] this The HashTable to insert the value
 * @param key The key to insert the value for
 * @param value The value to insert
 * @retval 1 Ok
 * @retval 0 Memory allocation failed or key already exists
 */
extern int hashtable_insert(hashtable *this, str key, void *value);

/**
 * @brief Search for the key inside the HashTable.
 * @param[in] this The HashTable to lookup the key
 * @param key The key to search for in the HashTable
 * @return The pointer to the value or NULL if not found
 */
extern void *hashtable_lookup(hashtable *this, str key);

/**
 * @brief Test whether the HashTable has the key.
 * @param[in] this The HashTable to check for the key
 * @param key The key to look for in the HashTable
 * @retval 1 The HashTable has the key
 * @retval 0 The HashTable does not have the key
 */
extern int hashtable_has(hashtable *this, str key);

#endif//BLINK_HASHTABLE_H
