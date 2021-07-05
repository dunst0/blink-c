/**
 * @file list.h
 * @author rick
 * @date 22.07.20
 * @brief File for the List implementation
 */

#ifndef WAITUI_LIST_H
#define WAITUI_LIST_H

#include <stdbool.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Type representing a List node.
 */
typedef struct waitui_list_node waitui_list_node;

/**
 * @brief Type for element destroy function.
 */
typedef void (*waitui_list_element_destroy)(void **element);

/**
 * @brief Type representing a List.
 */
typedef struct waitui_list waitui_list;

/**
 * @brief Type representing a List iterator.
 */
typedef struct waitui_list_iter waitui_list_iter;


// -----------------------------------------------------------------------------
//  Public defines
// -----------------------------------------------------------------------------

#define INTERFACE_LIST_TYPEDEF(type)                                           \
    typedef waitui_list type##_list;                                           \
    typedef waitui_list_iter type##_list_iter

#define IMPLEMENTATION_LIST_TYPEDEF(type)

#define INTERFACE_LIST_NEW(type) extern type##_list *type##_list_new()
#define IMPLEMENTATION_LIST_NEW(type)                                          \
    type##_list *type##_list_new() {                                           \
        return (type##_list *) waitui_list_new(                                \
                (waitui_list_element_destroy) type##_destroy);                 \
    }

#define INTERFACE_LIST_DESTROY(type)                                           \
    extern void type##_list_destroy(type##_list **this)
#define IMPLEMENTATION_LIST_DESTROY(type)                                      \
    void type##_list_destroy(type##_list **this) {                             \
        waitui_list_destroy((waitui_list **) this);                            \
    }

#define INTERFACE_LIST_PUSH(type, elem)                                        \
    extern int type##_list_push(type##_list *this, type *elem)
#define IMPLEMENTATION_LIST_PUSH(type, elem)                                   \
    int type##_list_push(type##_list *this, type *elem) {                      \
        return waitui_list_push((waitui_list *) this, (void *) elem);          \
    }

#define INTERFACE_LIST_POP(type) extern type *type##_list_pop(type##_list *this)
#define IMPLEMENTATION_LIST_POP(type)                                          \
    type *type##_list_pop(type##_list *this) {                                 \
        return (type *) waitui_list_pop((waitui_list *) this);                 \
    }

#define INTERFACE_LIST_UNSHIFT(type, elem)                                     \
    extern int type##_list_unshift(type##_list *this, type *elem)
#define IMPLEMENTATION_LIST_UNSHIFT(type, elem)                                \
    int type##_list_unshift(type##_list *this, type *elem) {                   \
        return waitui_list_unshift((waitui_list *) this, (void *) elem);       \
    }

#define INTERFACE_LIST_SHIFT(type)                                             \
    extern type *type##_list_shift(type##_list *this)
#define IMPLEMENTATION_LIST_SHIFT(type)                                        \
    type *type##_list_shift(type##_list *this) {                               \
        return (type *) waitui_list_shift((waitui_list *) this);               \
    }

#define INTERFACE_LIST_PEEK(type)                                              \
    extern type *type##_list_peek(type##_list *this)
#define IMPLEMENTATION_LIST_PEEK(type)                                         \
    type *type##_list_peek(type##_list *this) {                                \
        return (type *) waitui_list_peek((waitui_list *) this);                \
    }

#define INTERFACE_LIST_GET_ITERATOR(type)                                      \
    extern type##_list_iter *type##_list_getIterator(type##_list *this)
#define IMPLEMENTATION_LIST_GET_ITERATOR(type)                                 \
    type##_list_iter *type##_list_getIterator(type##_list *this) {             \
        return (type##_list_iter *) waitui_list_getIterator(                   \
                (waitui_list *) this);                                         \
    }

#define INTERFACE_LIST_ITER_HAS_NEXT(type)                                     \
    extern bool type##_list_iter_hasNext(type##_list_iter *this)
#define IMPLEMENTATION_LIST_ITER_HAS_NEXT(type)                                \
    bool type##_list_iter_hasNext(type##_list_iter *this) {                    \
        return waitui_list_iter_hasNext((waitui_list_iter *) this);            \
    }

#define INTERFACE_LIST_ITER_NEXT(type)                                         \
    extern type *type##_list_iter_next(type##_list_iter *this)
#define IMPLEMENTATION_LIST_ITER_NEXT(type)                                    \
    type *type##_list_iter_next(type##_list_iter *this) {                      \
        return (type *) waitui_list_iter_next((waitui_list_iter *) this);      \
    }

#define INTERFACE_LIST_ITER_DESTROY(type)                                      \
    extern void type##_list_iter_destroy(type##_list_iter **this)
#define IMPLEMENTATION_LIST_ITER_DESTROY(type)                                 \
    void type##_list_iter_destroy(type##_list_iter **this) {                   \
        waitui_list_iter_destroy((waitui_list_iter **) this);                  \
    }

#define CREATE_LIST_TYPE(kind, type, elem)                                     \
    kind##_LIST_TYPEDEF(type);                                                 \
    kind##_LIST_NEW(type);                                                     \
    kind##_LIST_DESTROY(type);                                                 \
    kind##_LIST_PUSH(type, elem);                                              \
    kind##_LIST_POP(type);                                                     \
    kind##_LIST_UNSHIFT(type, elem);                                           \
    kind##_LIST_SHIFT(type);                                                   \
    kind##_LIST_PEEK(type);                                                    \
    kind##_LIST_GET_ITERATOR(type);                                            \
    kind##_LIST_ITER_HAS_NEXT(type);                                           \
    kind##_LIST_ITER_NEXT(type);                                               \
    kind##_LIST_ITER_DESTROY(type);


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create a List.
 * @param[in] elementDestroyCallback Function to call for element destruction
 * @return A pointer to waitui_list or NULL if memory allocation failed
 */
extern waitui_list *
waitui_list_new(waitui_list_element_destroy elementDestroyCallback);

/**
 * @brief Destroy a List.
 * @param[in,out] this The List to destroy
 * @note This will free call for every element the elementDestroyCallback
 */
extern void waitui_list_destroy(waitui_list **this);

/**
 * @brief Add the element to the end of the List.
 * @param[in,out] this The List to add the element at the end
 * @param[in] element The element to add
 * @note This function does steel the pointer to the element.
 * @retval 1 Ok
 * @retval 0 Memory allocation failed
 */
extern int waitui_list_push(waitui_list *this, void *element);

/**
 * @brief Remove the element from the end of the List and return it.
 * @param[in,out] this The List to remove the element from
 * @note The caller has to destroy element on its own.
 * @return The element or NULL if waitui_list is empty
 */
extern void *waitui_list_pop(waitui_list *this);

/**
 * @brief Add the element to the beginning of the List.
 * @param[in,out] this The List to add the element at the beginning
 * @param[in] element The element to add
 * @note This function does steel the pointer to the element.
 * @retval 1 Ok
 * @retval 0 Memory allocation failed
 */
extern int waitui_list_unshift(waitui_list *this, void *element);

/**
 * @brief Remove the element from the beginning of the List and return it.
 * @param[in,out] this The List to remove the element from
 * @note The caller has to destroy element on its own.
 * @return The element or NULL if waitui_list is empty
 */
extern void *waitui_list_shift(waitui_list *this);

/**
 * @brief Return the element from the beginning of the List, without removing it.
 * @param[in] this The List to get the first element from
 * @warning The caller has not to destroy element on its own.
 * @return The element or NULL if waitui_list is empty
 */
extern void *waitui_list_peek(waitui_list *this);

/**
 * @brief Return the iterator to iterate over the List.
 * @param[in] this The List to get the iterator for
 * @return A pointer to waitui_list_iter or NULL if memory allocation failed
 */
extern waitui_list_iter *waitui_list_getIterator(waitui_list *this);

/**
 * TODO
 * @param this
 * @return
 */
extern bool waitui_list_iter_hasNext(waitui_list_iter *this);

/**
 * TODO
 * @param this
 * @return
 */
extern void *waitui_list_iter_next(waitui_list_iter *this);

/**
 * @brief Destroy a List iterator.
 * @param[in,out] this The List iterator to destroy
 */
extern void waitui_list_iter_destroy(waitui_list_iter **this);

#endif//WAITUI_LIST_H
