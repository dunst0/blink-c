/**
 * @file symbol.c
 * @author rick
 * @date 29.07.20
 * @brief File for the Symbol implementation
 */

#include "blink/symbol.h"


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

symbol *symbol_new(str identifier, symbol_type type) {
    symbol *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->type   = type;

    STR_COPY(&this->identifier, &identifier);
    if (!this->identifier.s) {
        symbol_destroy(&this);
        return NULL;
    }
    this->references = symbol_reference_list_new();
    if (!this->references) {
        symbol_destroy(&this);
        return NULL;
    }

    return this;
}

void symbol_destroy(symbol **this) {
    if (!this || !(*this)) { return; }

    STR_FREE(&(*this)->identifier);
    symbol_reference_list_destroy(&(*this)->references);

    free(*this);
    *this = NULL;
}
