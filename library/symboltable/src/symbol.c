/**
 * @file symbol.c
 * @author rick
 * @date 29.07.20
 * @brief File for the Symbol implementation
 */

#include "waitui/symbol.h"

#include <waitui/log.h>

#include <stdio.h>


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

symbol *symbol_new(str identifier, symbol_type type, unsigned long int line,
                   unsigned long int column) {
    symbol *this = NULL;

    waitui_log_trace("creating new symbol: '%.*s'", STR_FMT(&identifier));

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->type = type;

    STR_COPY(&this->identifier, &identifier);
    if (!this->identifier.s) {
        waitui_log_fatal("could not allocate memory for identifier");
        symbol_destroy(&this);
        return NULL;
    }
    this->references = symbol_reference_list_new();
    if (!this->references) {
        waitui_log_fatal("could not allocate memory for symbol_reference_list");
        symbol_destroy(&this);
        return NULL;
    }
    {
        symbol_reference *reference = symbol_reference_new(line, column);
        if (!reference) {
            waitui_log_fatal("could not allocate memory for symbol_reference");
            symbol_destroy(&this);
            return NULL;
        }
        if (!symbol_reference_list_push(this->references, reference)) {
            waitui_log_fatal(
                    "could not allocate memory for pushing symbol_reference");
            symbol_reference_destroy(&reference);
            symbol_destroy(&this);
            return NULL;
        }
    }

    waitui_log_trace("new symbol successful created %p", this);

    return this;
}

void symbol_increment_refcount(symbol *this) {
    if (!this) { return; }

    this->refcount++;

    waitui_log_trace(
            "increment refcount for symbol with identifier '%.*s' to %ld %p",
            STR_FMT(&this->identifier), this->refcount, this);
}

void symbol_decrement_refcount(symbol **this) {
    if (!this || !(*this)) { return; }

    (*this)->refcount--;

    waitui_log_trace(
            "decrement refcount for symbol with identifier '%.*s' to %ld %p",
            STR_FMT(&(*this)->identifier), (*this)->refcount, *this);

    if ((*this)->refcount < 1) {
        waitui_log_trace(
                "freeing symbol with identifier '%.*s' and refcount %ld %p",
                STR_FMT(&(*this)->identifier), (*this)->refcount, *this);
        symbol_destroy(this);
    }
}

symbol_reference *symbol_get_reference_head(symbol *this) {
    if (!this || !this->references) { return NULL; }
    return symbol_reference_list_peek(this->references);
}

void symbol_destroy(symbol **this) {
    waitui_log_trace("destroying symbol");

    if (!this || !(*this)) { return; }

    waitui_log_trace("destroying symbol with identifier '%.*s' %p",
                     STR_FMT(&(*this)->identifier), *this);

    STR_FREE(&(*this)->identifier);
    symbol_reference_list_destroy(&(*this)->references);

    free(*this);
    *this = NULL;

    waitui_log_trace("symbol successful destroyed");
}
