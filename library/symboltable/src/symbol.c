/**
 * @file symbol.c
 * @author rick
 * @date 29.07.20
 * @brief File for the Symbol implementation
 */

#include "waitui/symbol.h"

#include <stdio.h>


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

symbol *symbol_new(str identifier, symbol_type type, unsigned long int line,
                   unsigned long int column) {
    symbol *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->type = type;

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
    {
        symbol_reference *reference = symbol_reference_new(line, column);
        if (!reference) {
            symbol_destroy(&this);
            return NULL;
        }
        if (!symbol_reference_list_push(this->references, reference)) {
            symbol_reference_destroy(&reference);
            symbol_destroy(&this);
            return NULL;
        }
    }

    return this;
}

void symbol_increment_refcount(symbol *this) {
    if (!this) { return; }

    this->refcount++;

    fprintf(stderr,
            "symbol: Increment refcount for symbol with identifier '%.*s' to "
            "%ld\n",
            STR_FMT(&this->identifier), this->refcount);
}

void symbol_decrement_refcount(symbol **this) {
    if (!this || !(*this)) { return; }

    (*this)->refcount--;

    fprintf(stderr,
            "symbol: Decrement refcount for symbol with identifier '%.*s' to "
            "%ld\n",
            STR_FMT(&(*this)->identifier), (*this)->refcount);

    if ((*this)->refcount < 1) {
        fprintf(stderr,
                "symbol: Freeing symbol with identifier '%.*s' and refcount "
                "%ld\n",
                STR_FMT(&(*this)->identifier), (*this)->refcount);
        symbol_destroy(this);
    }
}

symbol_reference *symbol_get_reference_head(symbol *this) {
    if (!this || !this->references || !this->references->head) { return NULL; }

    return this->references->head->element;
}

void symbol_destroy(symbol **this) {
    if (!this || !(*this)) { return; }

    fprintf(stderr, "symbol: Freeing symbol with identifier '%.*s'\n",
            STR_FMT(&(*this)->identifier));

    STR_FREE(&(*this)->identifier);
    symbol_reference_list_destroy(&(*this)->references);

    free(*this);
    *this = NULL;
}
