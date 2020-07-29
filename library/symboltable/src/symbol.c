/**
 * @file symbol.c
 * @author rick
 * @date 29.07.20
 * @brief File for the Symbol implementation
 */

#include "blink/symbol.h"


// -----------------------------------------------------------------------------
// Implementation of public functions
// -----------------------------------------------------------------------------

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

symbol *symbol_new(unsigned long int line, unsigned long int column,
                   str identifier, symbol_type type) {
    symbol *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->line       = line;
    this->column     = column;
    this->identifier = identifier;//TODO: str dup
    this->type       = type;
    this->references = symbol_reference_list_new();
    if (!this->references) {
        free(this);
        return NULL;
    }

    return this;
}

void symbol_destroy(symbol **this) {
    if (!this || !(*this)) { return; }

    symbol_reference_list_destroy(&(*this)->references);

    free(*this);
    *this = NULL;
}
