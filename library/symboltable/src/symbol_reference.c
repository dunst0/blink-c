/**
 * @file symbol_reference.c
 * @author rick
 * @date 28.08.20
 * @brief File for the SymbolReference implementation
 */

#include "waitui/symbol_reference.h"

#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

CREATE_LIST_TYPE(IMPLEMENTATION, symbol_reference, reference)

symbol_reference *symbol_reference_new(unsigned long int line,
                                       unsigned long int column) {
    symbol_reference *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->line   = line;
    this->column = column;

    return this;
}

void symbol_reference_destroy(symbol_reference **this) {
    if (!this || !(*this)) { return; }

    free(*this);
    *this = NULL;
}
