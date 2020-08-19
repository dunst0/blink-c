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
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Type representing a HashTable node.
 */
typedef struct hashtable_node hashtable_node;
struct hashtable_node {
    str key;
    void *value;
    int isStolen;
    hashtable_node *next;
};

/**
 * @brief Type for value destroy function.
 */
typedef void (*hashtable_value_destroy)(void **value);

/**
 * @brief Type for value checking function.
 */
typedef int (*hashtable_value_check)(void *value, void *arg);

/**
 * @brief Type representing a HashTable.
 */
typedef struct hashtable {
    hashtable_node **list;
    hashtable_value_destroy valueDestroyCallback;
    unsigned long int size;
} hashtable;


// -----------------------------------------------------------------------------
//  Public defines
// -----------------------------------------------------------------------------

#define INTERFACE_HASHTABLE_TYPEDEF(type) typedef hashtable type##_hashtable
#define IMPLEMENTATION_HASHTABLE_TYPEDEF(type)

#define INTERFACE_HASHTABLE_NEW(type)                                          \
    extern type##_hashtable *type##_hashtable_new(unsigned long int length)
#define IMPLEMENTATION_HASHTABLE_NEW(type)                                     \
    type##_hashtable *type##_hashtable_new(unsigned long int length) {         \
        return (type##_hashtable *) hashtable_new(                             \
                length, (hashtable_value_destroy) type##_destroy);             \
    }

#define INTERFACE_HASHTABLE_DESTROY(type)                                      \
    extern void type##_hashtable_destroy(type##_hashtable **this)
#define IMPLEMENTATION_HASHTABLE_DESTROY(type)                                 \
    void type##_hashtable_destroy(type##_hashtable **this) {                   \
        hashtable_destroy((hashtable **) this);                                \
    }

#define INTERFACE_HASHTABLE_INSERT_CHECK(type, elem)                           \
    extern int type##_hashtable_insert_check(                                  \
            type##_hashtable *this, str key, type *elem,                       \
            hashtable_value_check valueCheckCallback, void *arg)
#define IMPLEMENTATION_HASHTABLE_INSERT_CHECK(type, elem)                      \
    int type##_hashtable_insert_check(                                         \
            type##_hashtable *this, str key, type *elem,                       \
            hashtable_value_check valueCheckCallback, void *arg) {             \
        return hashtable_insert_check((hashtable *) this, key, (void *) elem,  \
                                      valueCheckCallback, arg);                \
    }

#define INTERFACE_HASHTABLE_LOOKUP_CHECK(type)                                 \
    extern type *type##_hashtable_lookup_check(                                \
            type##_hashtable *this, str key,                                   \
            hashtable_value_check valueCheckCallback, void *arg)
#define IMPLEMENTATION_HASHTABLE_LOOKUP_CHECK(type)                            \
    type *type##_hashtable_lookup_check(                                       \
            type##_hashtable *this, str key,                                   \
            hashtable_value_check valueCheckCallback, void *arg) {             \
        return (type *) hashtable_lookup_check((hashtable *) this, key,        \
                                               valueCheckCallback, arg);       \
    }

#define INTERFACE_HASHTABLE_HAS_CHECK(type)                                    \
    extern int type##_hashtable_has_check(                                     \
            type##_hashtable *this, str key,                                   \
            hashtable_value_check valueCheckCallback, void *arg)
#define IMPLEMENTATION_HASHTABLE_HAS_CHECK(type)                               \
    int type##_hashtable_has_check(type##_hashtable *this, str key,            \
                                   hashtable_value_check valueCheckCallback,   \
                                   void *arg) {                                \
        return hashtable_has_check((hashtable *) this, key,                    \
                                   valueCheckCallback, arg);                   \
    }

#define INTERFACE_HASHTABLE_MARK_STOLEN_CHECK(type)                            \
    extern int type##_hashtable_mark_stolen_check(                             \
            type##_hashtable *this, str key,                                   \
            hashtable_value_check valueCheckCallback, void *arg)
#define IMPLEMENTATION_HASHTABLE_MARK_STOLEN_CHECK(type)                       \
    int type##_hashtable_mark_stolen_check(                                    \
            type##_hashtable *this, str key,                                   \
            hashtable_value_check valueCheckCallback, void *arg) {             \
        return hashtable_mark_stolen_check((hashtable *) this, key,            \
                                           valueCheckCallback, arg);           \
    }

#define INTERFACE_HASHTABLE_INSERT(type, elem)                                 \
    extern int type##_hashtable_insert(type##_hashtable *this, str key,        \
                                       type *elem)
#define IMPLEMENTATION_HASHTABLE_INSERT(type, elem)                            \
    int type##_hashtable_insert(type##_hashtable *this, str key, type *elem) { \
        return hashtable_insert((hashtable *) this, key, (void *) elem);       \
    }

#define INTERFACE_HASHTABLE_LOOKUP(type)                                       \
    extern type *type##_hashtable_lookup(type##_hashtable *this, str key)
#define IMPLEMENTATION_HASHTABLE_LOOKUP(type)                                  \
    type *type##_hashtable_lookup(type##_hashtable *this, str key) {           \
        return (type *) hashtable_lookup((hashtable *) this, key);             \
    }

#define INTERFACE_HASHTABLE_HAS(type)                                          \
    extern int type##_hashtable_has(type##_hashtable *this, str key)
#define IMPLEMENTATION_HASHTABLE_HAS(type)                                     \
    int type##_hashtable_has(type##_hashtable *this, str key) {                \
        return hashtable_has((hashtable *) this, key);                         \
    }

#define INTERFACE_HASHTABLE_MARK_STOLEN(type)                                  \
    extern int type##_hashtable_mark_stolen(type##_hashtable *this, str key)
#define IMPLEMENTATION_HASHTABLE_MARK_STOLEN(type)                             \
    int type##_hashtable_mark_stolen(type##_hashtable *this, str key) {        \
        return hashtable_mark_stolen((hashtable *) this, key);                 \
    }


#define CREATE_HASHTABLE_TYPE(kind, type, elem)                                \
    kind##_HASHTABLE_TYPEDEF(type);                                            \
    kind##_HASHTABLE_NEW(type);                                                \
    kind##_HASHTABLE_DESTROY(type);                                            \
    kind##_HASHTABLE_INSERT_CHECK(type, elem);                                 \
    kind##_HASHTABLE_INSERT(type, elem);                                       \
    kind##_HASHTABLE_LOOKUP_CHECK(type);                                       \
    kind##_HASHTABLE_LOOKUP(type);                                             \
    kind##_HASHTABLE_HAS_CHECK(type);                                          \
    kind##_HASHTABLE_HAS(type);                                                \
    kind##_HASHTABLE_MARK_STOLEN_CHECK(type);                                  \
    kind##_HASHTABLE_MARK_STOLEN(type);


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create a HashTable.
 * @param[in] size The HashTable size
 * @param[in] valueDestroyCallback Function to call for value destruction
 * @return A pointer to hashtable or NULL if memory allocation failed
 */
extern hashtable *hashtable_new(unsigned long int size,
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
 * @param[in] key The key to insert the value for
 * @param[in] value The value to insert
 * @param[in] valueCheckCallback The value checking function to call
 * @param[in] arg The value for the second parameter to the valueCheckCallback
 * @retval 1 Ok
 * @retval 0 Memory allocation failed or key already exists
 */
extern int hashtable_insert_check(hashtable *this, str key, void *value,
                                  hashtable_value_check valueCheckCallback,
                                  void *arg);

/**
 * @brief Search for the key inside the HashTable.
 * @param[in] this The HashTable to lookup the key
 * @param[in] key The key to search for in the HashTable
 * @param[in] valueCheckCallback The value checking function to call
 * @param[in] arg The value for the second parameter to the valueCheckCallback
 * @return The pointer to the value or NULL if not found
 */
extern void *hashtable_lookup_check(hashtable *this, str key,
                                    hashtable_value_check valueCheckCallback,
                                    void *arg);

/**
 * @brief Test whether the HashTable has the key.
 * @param[in] this The HashTable to check for the key
 * @param[in] key The key to look for in the HashTable
 * @param[in] valueCheckCallback The value checking function to call
 * @param[in] arg The value for the second parameter to the valueCheckCallback
 * @retval 1 The HashTable has the key
 * @retval 0 The HashTable does not have the key
 */
extern int hashtable_has_check(hashtable *this, str key,
                               hashtable_value_check valueCheckCallback,
                               void *arg);

/**
 * @brief Mark value for key as stolen in the HashTable.
 * @param[in] this The HashTable to check for the key
 * @param[in] key The key to look for in the HashTable
 * @param[in] valueCheckCallback The value checking function to call
 * @param[in] arg The value for the second parameter to the valueCheckCallback
 * @retval 1 The HashTable has the key
 * @retval 0 The HashTable does not have the key
 */
extern int hashtable_mark_stolen_check(hashtable *this, str key,
                                       hashtable_value_check valueCheckCallback,
                                       void *arg);

/**
 * @brief Insert a value for the key into the HashTable.
 * @param[in,out] this The HashTable to insert the value
 * @param[in] key The key to insert the value for
 * @param[in] value The value to insert
 * @retval 1 Ok
 * @retval 0 Memory allocation failed or key already exists
 */
extern int hashtable_insert(hashtable *this, str key, void *value);

/**
 * @brief Search for the key inside the HashTable.
 * @param[in] this The HashTable to lookup the key
 * @param[in] key The key to search for in the HashTable
 * @return The pointer to the value or NULL if not found
 */
extern void *hashtable_lookup(hashtable *this, str key);

/**
 * @brief Test whether the HashTable has the key.
 * @param[in] this The HashTable to check for the key
 * @param[in] key The key to look for in the HashTable
 * @retval 1 The HashTable has the key
 * @retval 0 The HashTable does not have the key
 */
extern int hashtable_has(hashtable *this, str key);

/**
 * @brief Mark value for key as stolen in the HashTable.
 * @param[in] this The HashTable to check for the key
 * @param[in] key The key to look for in the HashTable
 * @retval 1 The HashTable has the key
 * @retval 0 The HashTable does not have the key
 */
extern int hashtable_mark_stolen(hashtable *this, str key);

#endif//BLINK_HASHTABLE_H
