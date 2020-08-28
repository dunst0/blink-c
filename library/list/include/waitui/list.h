/**
 * @file list.h
 * @author rick
 * @date 22.07.20
 * @brief File for the List implementation
 */

#ifndef WAITUI_LIST_H
#define WAITUI_LIST_H


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Type representing a List node.
 */
typedef struct list_node list_node;
struct list_node {
    void *element;
    list_node *next;
    list_node *prev;
};

/**
 * @brief Type for element destroy function.
 */
typedef void (*list_element_destroy)(void **element);

/**
 * @brief Type representing a List.
 */
typedef struct list {
    list_node *head;
    list_node *tail;
    list_element_destroy elementDestroyCallback;
    unsigned long long length;
} list;


// -----------------------------------------------------------------------------
//  Public defines
// -----------------------------------------------------------------------------

#define INTERFACE_LIST_TYPEDEF(type) typedef list type##_list
#define IMPLEMENTATION_LIST_TYPEDEF(type)

#define INTERFACE_LIST_NEW(type) extern type##_list *type##_list_new()
#define IMPLEMENTATION_LIST_NEW(type)                                          \
    type##_list *type##_list_new() {                                           \
        return (type##_list *) list_new(                                       \
                (list_element_destroy) type##_destroy);                        \
    }

#define INTERFACE_LIST_NEW_CUSTOM(type, element_destroy)                       \
    extern type##_list *type##_list_new()
#define IMPLEMENTATION_LIST_NEW_CUSTOM(type, element_destroy)                  \
    type##_list *type##_list_new() {                                           \
        return (type##_list *) list_new(                                       \
                (list_element_destroy)(element_destroy));                      \
    }

#define INTERFACE_LIST_DESTROY(type)                                           \
    extern void type##_list_destroy(type##_list **this)
#define IMPLEMENTATION_LIST_DESTROY(type)                                      \
    void type##_list_destroy(type##_list **this) {                             \
        list_destroy((list **) this);                                          \
    }

#define INTERFACE_LIST_PUSH(type, elem)                                        \
    extern int type##_list_push(type##_list *this, type *elem)
#define IMPLEMENTATION_LIST_PUSH(type, elem)                                   \
    int type##_list_push(type##_list *this, type *elem) {                      \
        return list_push((list *) this, (void *) elem);                        \
    }

#define INTERFACE_LIST_POP(type) extern type *type##_list_pop(type##_list *this)
#define IMPLEMENTATION_LIST_POP(type)                                          \
    type *type##_list_pop(type##_list *this) {                                 \
        return (type *) list_pop((list *) this);                               \
    }

#define INTERFACE_LIST_UNSHIFT(type, elem)                                     \
    extern int type##_list_unshift(type##_list *this, type *elem)
#define IMPLEMENTATION_LIST_UNSHIFT(type, elem)                                \
    int type##_list_unshift(type##_list *this, type *elem) {                   \
        return list_unshift((list *) this, (void *) elem);                     \
    }

#define INTERFACE_LIST_SHIFT(type)                                             \
    extern type *type##_list_shift(type##_list *this)
#define IMPLEMENTATION_LIST_SHIFT(type)                                        \
    type *type##_list_shift(type##_list *this) {                               \
        return (type *) list_shift((list *) this);                             \
    }

#define CREATE_LIST_TYPE(kind, type, elem)                                     \
    kind##_LIST_TYPEDEF(type);                                                 \
    kind##_LIST_NEW(type);                                                     \
    kind##_LIST_DESTROY(type);                                                 \
    kind##_LIST_PUSH(type, elem);                                              \
    kind##_LIST_POP(type);                                                     \
    kind##_LIST_UNSHIFT(type, elem);                                           \
    kind##_LIST_SHIFT(type);


#define CREATE_LIST_TYPE_CUSTOM(kind, type, elem, element_destroy)             \
    kind##_LIST_TYPEDEF(type);                                                 \
    kind##_LIST_NEW_CUSTOM(type, element_destroy);                             \
    kind##_LIST_DESTROY(type);                                                 \
    kind##_LIST_PUSH(type, elem);                                              \
    kind##_LIST_POP(type);                                                     \
    kind##_LIST_UNSHIFT(type, elem);                                           \
    kind##_LIST_SHIFT(type);

// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create a List.
 * @param[in] elementDestroyCallback Function to call for element destruction
 * @return A pointer to list or NULL if memory allocation failed
 */
extern list *list_new(list_element_destroy elementDestroyCallback);

/**
 * @brief Destroy a List.
 * @param[in,out] this The List to destroy
 * @note This will free call for every element the elementDestroyCallback
 */
extern void list_destroy(list **this);

/**
 * @brief Add the element to the end of the List.
 * @param[in,out] this The List to add the element at the end
 * @param[in] element The element to add
 * @note This function does steel the pointer to the element.
 * @retval 1 Ok
 * @retval 0 Memory allocation failed
 */
extern int list_push(list *this, void *element);

/**
 * @brief Remove the element from the end of the List and return it.
 * @param[in,out] this The List to remove the element from
 * @note The caller has to destroy element on its own.
 * @return The element or NULL if list is empty
 */
extern void *list_pop(list *this);

/**
 * @brief Add the element to the beginning of the List.
 * @param[in,out] this The List to add the element at the beginning
 * @param[in] element The element to add
 * @note This function does steel the pointer to the element.
 * @retval 1 Ok
 * @retval 0 Memory allocation failed
 */
extern int list_unshift(list *this, void *element);

/**
 * @brief Remove the element from the beginning of the List and return it.
 * @param[in,out] this The List to remove the element from
 * @note The caller has to destroy element on its own.
 * @return The element or NULL if list is empty
 */
extern void *list_shift(list *this);

#endif//WAITUI_LIST_H
