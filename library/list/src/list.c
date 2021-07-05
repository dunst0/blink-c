/**
 * @file list.c
 * @author rick
 * @date 22.07.20
 * @brief File for the List implementation
 */

#include "waitui/list.h"

#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Local types
// -----------------------------------------------------------------------------

/**
 * @brief Struct representing a List node.
 */
struct waitui_list_node {
    void *element;
    waitui_list_node *next;
    waitui_list_node *prev;
};

/**
 * @brief Struct representing a List.
 */
struct waitui_list {
    waitui_list_node *head;
    waitui_list_node *tail;
    waitui_list_element_destroy elementDestroyCallback;
    unsigned long long length;
};

/**
 * @brief Struct representing a List iterator.
 */
struct waitui_list_iter {
    waitui_list_node *node;
};


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

/**
 * @brief Create a List iterator.
 * @param[in] node The node to use as a start for the iterator
 * @return A pointer to waitui_list_iter or NULL if memory allocation failed
 */
static waitui_list_iter *waitui_list_iter_new(waitui_list_node *node) {
    waitui_list_iter *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->node = node;

    return this;
}


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

waitui_list *
waitui_list_new(waitui_list_element_destroy elementDestroyCallback) {
    waitui_list *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->elementDestroyCallback = elementDestroyCallback;

    return this;
}

void waitui_list_destroy(waitui_list **this) {
    if (!this || !(*this)) { return; }

    while ((*this)->head) {
        waitui_list_node *temp = (*this)->head;
        (*this)->head          = (*this)->head->next;

        if ((*this)->elementDestroyCallback) {
            (*this)->elementDestroyCallback(&temp->element);
        }
        free(temp);
    }

    free(*this);
    *this = NULL;
}

int waitui_list_push(waitui_list *this, void *element) {
    waitui_list_node *node = NULL;

    if (!this || !element) { return 0; }

    node = calloc(1, sizeof(*node));
    if (!node) { return 0; }

    node->element = element;

    if (this->tail == NULL) {
        this->head = this->tail = node;
    } else {
        node->prev       = this->tail;
        this->tail->next = node;
        this->tail       = node;
    }

    this->length++;

    return 1;
}

void *waitui_list_pop(waitui_list *this) {
    waitui_list_node *node = NULL;
    void *element          = NULL;

    if (!this || this->length == 0) { return NULL; }

    node    = this->tail;
    element = node->element;

    this->tail = this->tail->prev;
    this->length--;
    if (this->tail != NULL) {
        this->tail->next = NULL;
    } else {
        this->head = NULL;
    }

    free(node);

    return element;
}

int waitui_list_unshift(waitui_list *this, void *element) {
    waitui_list_node *node = NULL;

    if (!this || !element) { return 0; }

    node = calloc(1, sizeof(*node));
    if (!node) { return 0; }

    node->element = element;

    if (this->head == NULL) {
        this->head = this->tail = node;
    } else {
        node->next       = this->head;
        this->head->prev = node;
        this->head       = node;
    }

    this->length++;

    return 1;
}

void *waitui_list_shift(waitui_list *this) {
    waitui_list_node *node = NULL;
    void *element          = NULL;

    if (!this || this->length == 0) { return NULL; }

    node    = this->head;
    element = node->element;

    this->head = this->head->next;
    this->length--;
    if (this->head != NULL) {
        this->head->prev = NULL;
    } else {
        this->tail = NULL;
    }

    free(node);

    return element;
}

void *waitui_list_peek(waitui_list *this) {
    if (!this || this->length == 0) { return NULL; }
    return this->head->element;
}

waitui_list_iter *waitui_list_getIterator(waitui_list *this) {
    if (!this) { return NULL; }
    return waitui_list_iter_new(this->head);
}

bool waitui_list_iter_hasNext(waitui_list_iter *this) {
    if (!this) { return false; }
    return this->node != NULL;
}

void *waitui_list_iter_next(waitui_list_iter *this) {
    waitui_list_node *node = NULL;

    if (!this) { return NULL; }

    node       = this->node;
    this->node = this->node->next;

    return node->element;
}

void waitui_list_iter_destroy(waitui_list_iter **this) {
    if (!this || !(*this)) { return; }

    free(*this);
    *this = NULL;
}
