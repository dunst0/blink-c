/**
 * @file symbol_reference.c
 * @author rick
 * @date 28.08.20
 * @brief File for the SymbolReference implementation
 */

#include "waitui/symbol_reference.h"

#include <waitui/log.h>

#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

CREATE_LIST_TYPE(IMPLEMENTATION, symbol_reference)

symbol_reference *symbol_reference_new(unsigned long int line,
                                       unsigned long int column) {
    symbol_reference *this = NULL;

    waitui_log_trace("creating new symbol_reference");

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->line   = line;
    this->column = column;

    waitui_log_trace("new symbol_reference successful created");

    return this;
}

void symbol_reference_destroy(symbol_reference **this) {
    waitui_log_trace("destroying symbol_reference");

    if (!this || !(*this)) { return; }

    free(*this);
    *this = NULL;

    waitui_log_trace("symbol_reference successful destroyed");
}
