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
//  Local types
// -----------------------------------------------------------------------------

/**
 * @brief Struct representing a SymbolReference.
 */
struct waitui_symbol_reference {
    unsigned long long line;
    unsigned long long column;
};


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

CREATE_LIST_TYPE(IMPLEMENTATION, waitui_symbol_reference)

waitui_symbol_reference *waitui_symbol_reference_new(unsigned long int line,
                                                     unsigned long int column) {
    waitui_symbol_reference *this = NULL;

    waitui_log_trace("creating new waitui_symbol_reference");

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->line   = line;
    this->column = column;

    waitui_log_trace("new waitui_symbol_reference successful created");

    return this;
}

void waitui_symbol_reference_destroy(waitui_symbol_reference **this) {
    waitui_log_trace("destroying waitui_symbol_reference");

    if (!this || !(*this)) { return; }

    free(*this);
    *this = NULL;

    waitui_log_trace("waitui_symbol_reference successful destroyed");
}

unsigned long long
waitui_symbol_reference_getLine(waitui_symbol_reference *this) {
    if (!this) { return 0; }
    return this->line;
}

unsigned long long
waitui_symbol_reference_getColumn(waitui_symbol_reference *this) {
    if (!this) { return 0; }
    return this->column;
}
