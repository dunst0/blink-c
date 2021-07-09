/**
 * @file hashtable.h
 * @author rick
 * @date 23.07.20
 * @brief File for the HashTable implementation
 */

#ifndef WAITUI_HASHTABLE_H
#define WAITUI_HASHTABLE_H

#include <waitui/str.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Type representing a HashTable node.
 */
typedef struct waitui_hashtable_node waitui_hashtable_node;
struct waitui_hashtable_node {
    str key;
    void *value;
    int isStolen;
    waitui_hashtable_node *next;
};

/**
 * @brief Type for value destroy function.
 */
typedef void (*waitui_hashtable_value_destroy_fn)(void **value);

/**
 * @brief Type for value checking function.
 */
typedef int (*waitui_hashtable_value_check_fn)(void *value, void *arg);

/**
 * @brief Type representing a HashTable.
 */
typedef struct waitui_hashtable {
    waitui_hashtable_node **list;
    waitui_hashtable_value_destroy_fn valueDestroyFn;
    unsigned long int size;
} waitui_hashtable;


// -----------------------------------------------------------------------------
//  Public defines
// -----------------------------------------------------------------------------

#define INTERFACE_HASHTABLE_TYPEDEF(type)                                      \
    typedef waitui_hashtable type##_hashtable
#define IMPLEMENTATION_HASHTABLE_TYPEDEF(type)

#define INTERFACE_HASHTABLE_NEW(type)                                          \
    extern type##_hashtable *type##_hashtable_new(unsigned long int length)
#define IMPLEMENTATION_HASHTABLE_NEW(type)                                     \
    type##_hashtable *type##_hashtable_new(unsigned long int length) {         \
        return (type##_hashtable *) waitui_hashtable_new(                      \
                length, (waitui_hashtable_value_destroy_fn) type##_destroy);   \
    }

#define INTERFACE_HASHTABLE_NEW_CUSTOM(type, elem_destroy)                     \
    extern type##_hashtable *type##_hashtable_new(unsigned long int length)
#define IMPLEMENTATION_HASHTABLE_NEW_CUSTOM(type, elem_destroy)                \
    type##_hashtable *type##_hashtable_new(unsigned long int length) {         \
        return (type##_hashtable *) waitui_hashtable_new(                      \
                length, (waitui_hashtable_value_destroy_fn) (elem_destroy));   \
    }

#define INTERFACE_HASHTABLE_DESTROY(type)                                      \
    extern void type##_hashtable_destroy(type##_hashtable **this)
#define IMPLEMENTATION_HASHTABLE_DESTROY(type)                                 \
    void type##_hashtable_destroy(type##_hashtable **this) {                   \
        waitui_hashtable_destroy((waitui_hashtable **) this);                  \
    }

#define INTERFACE_HASHTABLE_INSERT_CHECK(type)                                 \
    extern int type##_hashtable_insert_check(                                  \
            type##_hashtable *this, str key, type *type##Element,              \
            waitui_hashtable_value_check_fn valueCheckFn, void *arg)
#define IMPLEMENTATION_HASHTABLE_INSERT_CHECK(type)                            \
    int type##_hashtable_insert_check(                                         \
            type##_hashtable *this, str key, type *type##Element,              \
            waitui_hashtable_value_check_fn valueCheckFn, void *arg) {         \
        return waitui_hashtable_insert_check(                                  \
                (waitui_hashtable *) this, key, (void *) type##Element,        \
                (waitui_hashtable_value_check_fn) valueCheckFn, arg);          \
    }

#define INTERFACE_HASHTABLE_LOOKUP_CHECK(type)                                 \
    extern type *type##_hashtable_lookup_check(                                \
            type##_hashtable *this, str key,                                   \
            waitui_hashtable_value_check_fn valueCheckFn, void *arg)
#define IMPLEMENTATION_HASHTABLE_LOOKUP_CHECK(type)                            \
    type *type##_hashtable_lookup_check(                                       \
            type##_hashtable *this, str key,                                   \
            waitui_hashtable_value_check_fn valueCheckFn, void *arg) {         \
        return (type *) waitui_hashtable_lookup_check(                         \
                (waitui_hashtable *) this, key,                                \
                (waitui_hashtable_value_check_fn) valueCheckFn, arg);          \
    }

#define INTERFACE_HASHTABLE_HAS_CHECK(type)                                    \
    extern int type##_hashtable_has_check(                                     \
            type##_hashtable *this, str key,                                   \
            waitui_hashtable_value_check_fn valueCheckFn, void *arg)
#define IMPLEMENTATION_HASHTABLE_HAS_CHECK(type)                               \
    int type##_hashtable_has_check(                                            \
            type##_hashtable *this, str key,                                   \
            waitui_hashtable_value_check_fn valueCheckFn, void *arg) {         \
        return waitui_hashtable_has_check(                                     \
                (waitui_hashtable *) this, key,                                \
                (waitui_hashtable_value_check_fn) valueCheckFn, arg);          \
    }

#define INTERFACE_HASHTABLE_MARK_STOLEN_CHECK(type)                            \
    extern int type##_hashtable_mark_stolen_check(                             \
            type##_hashtable *this, str key,                                   \
            waitui_hashtable_value_check_fn valueCheckFn, void *arg)
#define IMPLEMENTATION_HASHTABLE_MARK_STOLEN_CHECK(type)                       \
    int type##_hashtable_mark_stolen_check(                                    \
            type##_hashtable *this, str key,                                   \
            waitui_hashtable_value_check_fn valueCheckFn, void *arg) {         \
        return waitui_hashtable_mark_stolen_check(                             \
                (waitui_hashtable *) this, key,                                \
                (waitui_hashtable_value_check_fn) valueCheckFn, arg);          \
    }

#define INTERFACE_HASHTABLE_INSERT(type)                                       \
    extern int type##_hashtable_insert(type##_hashtable *this, str key,        \
                                       type *type##Element)
#define IMPLEMENTATION_HASHTABLE_INSERT(type)                                  \
    int type##_hashtable_insert(type##_hashtable *this, str key,               \
                                type *type##Element) {                         \
        return waitui_hashtable_insert((waitui_hashtable *) this, key,         \
                                       (void *) type##Element);                \
    }

#define INTERFACE_HASHTABLE_LOOKUP(type)                                       \
    extern type *type##_hashtable_lookup(type##_hashtable *this, str key)
#define IMPLEMENTATION_HASHTABLE_LOOKUP(type)                                  \
    type *type##_hashtable_lookup(type##_hashtable *this, str key) {           \
        return (type *) waitui_hashtable_lookup((waitui_hashtable *) this,     \
                                                key);                          \
    }

#define INTERFACE_HASHTABLE_HAS(type)                                          \
    extern int type##_hashtable_has(type##_hashtable *this, str key)
#define IMPLEMENTATION_HASHTABLE_HAS(type)                                     \
    int type##_hashtable_has(type##_hashtable *this, str key) {                \
        return waitui_hashtable_has((waitui_hashtable *) this, key);           \
    }

#define INTERFACE_HASHTABLE_MARK_STOLEN(type)                                  \
    extern int type##_hashtable_mark_stolen(type##_hashtable *this, str key)
#define IMPLEMENTATION_HASHTABLE_MARK_STOLEN(type)                             \
    int type##_hashtable_mark_stolen(type##_hashtable *this, str key) {        \
        return waitui_hashtable_mark_stolen((waitui_hashtable *) this, key);   \
    }


#define CREATE_HASHTABLE_TYPE(kind, type)                                      \
    kind##_HASHTABLE_TYPEDEF(type);                                            \
    kind##_HASHTABLE_NEW(type);                                                \
    kind##_HASHTABLE_DESTROY(type);                                            \
    kind##_HASHTABLE_INSERT_CHECK(type);                                       \
    kind##_HASHTABLE_INSERT(type);                                             \
    kind##_HASHTABLE_LOOKUP_CHECK(type);                                       \
    kind##_HASHTABLE_LOOKUP(type);                                             \
    kind##_HASHTABLE_HAS_CHECK(type);                                          \
    kind##_HASHTABLE_HAS(type);                                                \
    kind##_HASHTABLE_MARK_STOLEN_CHECK(type);                                  \
    kind##_HASHTABLE_MARK_STOLEN(type);

#define CREATE_HASHTABLE_TYPE_CUSTOM(kind, type, elem_destroy)                 \
    kind##_HASHTABLE_TYPEDEF(type);                                            \
    kind##_HASHTABLE_NEW_CUSTOM(type, elem_destroy);                           \
    kind##_HASHTABLE_DESTROY(type);                                            \
    kind##_HASHTABLE_INSERT_CHECK(type);                                       \
    kind##_HASHTABLE_INSERT(type);                                             \
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
 * @param[in] valueDestroyFn Function to call for value destruction
 * @return A pointer to waitui_hashtable or NULL if memory allocation failed
 */
extern waitui_hashtable *
waitui_hashtable_new(unsigned long int size,
                     waitui_hashtable_value_destroy_fn valueDestroyFn);

/**
 * @brief Destroy a HashTable.
 * @param[in,out] this The HashTable to destroy
 * @note This will free call for every value the valueDestroyFn
 */
extern void waitui_hashtable_destroy(waitui_hashtable **this);

/**
 * @brief Insert a value for the key into the HashTable.
 * @param[in,out] this The HashTable to insert the value
 * @param[in] key The key to insert the value for
 * @param[in] value The value to insert
 * @param[in] valueCheckFn The value checking function to call
 * @param[in] arg The value for the second parameter to the valueCheckFn
 * @retval 1 Ok
 * @retval 0 Memory allocation failed or key already exists
 */
extern int
waitui_hashtable_insert_check(waitui_hashtable *this, str key, void *value,
                              waitui_hashtable_value_check_fn valueCheckFn,
                              void *arg);

/**
 * @brief Search for the key inside the HashTable.
 * @param[in] this The HashTable to lookup the key
 * @param[in] key The key to search for in the HashTable
 * @param[in] valueCheckFn The value checking function to call
 * @param[in] arg The value for the second parameter to the valueCheckFn
 * @return The pointer to the value or NULL if not found
 */
extern void *
waitui_hashtable_lookup_check(waitui_hashtable *this, str key,
                              waitui_hashtable_value_check_fn valueCheckFn,
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
extern int
waitui_hashtable_has_check(waitui_hashtable *this, str key,
                           waitui_hashtable_value_check_fn valueCheckCallback,
                           void *arg);

/**
 * @brief Mark value for key as stolen in the HashTable.
 * @param[in] this The HashTable to check for the key
 * @param[in] key The key to look for in the HashTable
 * @param[in] valueCheckFn The value checking function to call
 * @param[in] arg The value for the second parameter to the valueCheckFn
 * @retval 1 The HashTable has the key
 * @retval 0 The HashTable does not have the key
 */
extern int
waitui_hashtable_mark_stolen_check(waitui_hashtable *this, str key,
                                   waitui_hashtable_value_check_fn valueCheckFn,
                                   void *arg);

/**
 * @brief Insert a value for the key into the HashTable.
 * @param[in,out] this The HashTable to insert the value
 * @param[in] key The key to insert the value for
 * @param[in] value The value to insert
 * @retval 1 Ok
 * @retval 0 Memory allocation failed or key already exists
 */
extern int waitui_hashtable_insert(waitui_hashtable *this, str key,
                                   void *value);

/**
 * @brief Search for the key inside the HashTable.
 * @param[in] this The HashTable to lookup the key
 * @param[in] key The key to search for in the HashTable
 * @return The pointer to the value or NULL if not found
 */
extern void *waitui_hashtable_lookup(waitui_hashtable *this, str key);

/**
 * @brief Test whether the HashTable has the key.
 * @param[in] this The HashTable to check for the key
 * @param[in] key The key to look for in the HashTable
 * @retval 1 The HashTable has the key
 * @retval 0 The HashTable does not have the key
 */
extern int waitui_hashtable_has(waitui_hashtable *this, str key);

/**
 * @brief Mark value for key as stolen in the HashTable.
 * @param[in] this The HashTable to check for the key
 * @param[in] key The key to look for in the HashTable
 * @retval 1 The HashTable has the key
 * @retval 0 The HashTable does not have the key
 */
extern int waitui_hashtable_mark_stolen(waitui_hashtable *this, str key);

#endif//WAITUI_HASHTABLE_H
