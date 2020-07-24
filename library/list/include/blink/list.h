/**
 * @file list.h
 * @author rick
 * @date 22.07.20
 * @brief File for the List implementation
 */

#ifndef BLINK_LIST_H
#define BLINK_LIST_H


// -----------------------------------------------------------------------------
//  Types
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
// Public functions
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

#endif//BLINK_LIST_H
