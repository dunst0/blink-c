/**
 * @file list.c
 * @author rick
 * @date 22.07.20
 * @brief File for the List implementation
 */

#include "blink/list.h"

#include <stdlib.h>


// -----------------------------------------------------------------------------
// Public functions
// -----------------------------------------------------------------------------

list *list_new(list_element_destroy elementDestroyCallback) {
    list *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->elementDestroyCallback = elementDestroyCallback;

    return this;
}

void list_destroy(list **this) {
    if (!this || !(*this)) { return; }

    while ((*this)->head) {
        list_node *temp = (*this)->head;
        (*this)->head   = (*this)->head->next;

        (*this)->elementDestroyCallback(&temp->element);
        free(temp);
    }

    free(*this);
    *this = NULL;
}

int list_push(list *this, void *element) {
    list_node *node = NULL;

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

void *list_pop(list *this) {
    list_node *node = NULL;
    void *element   = NULL;

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

int list_unshift(list *this, void *element) {
    list_node *node = NULL;

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

void *list_shift(list *this) {
    list_node *node = NULL;
    void *element   = NULL;

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
