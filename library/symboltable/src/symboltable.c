/**
 * @file symboltable.c
 * @author rick
 * @date 21.07.20
 * @brief File for the SymbolTable implementation
 */

#include "waitui/symboltable.h"

#include <waitui/log.h>

#include <stdio.h>
#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Local defines
// -----------------------------------------------------------------------------

#define SYMBOLTABLE_SIZE 997


// -----------------------------------------------------------------------------
//  Local types
// -----------------------------------------------------------------------------

/**
 * @brief Struct representing a SymbolTable.
 */
struct symboltable {
    long int currentScope;
    int declarationMode;
    symbol_hashtable *symbols;
};


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

/**
 * @brief Check the both Symbols to have same scope.
 * @return 1 if both Symbols have the same scope, else 0
 */
static int symboltable_symbol_check(symbol *this, symbol *other) {
    return this->scope == other->scope;
}


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

CREATE_HASHTABLE_TYPE_CUSTOM(IMPLEMENTATION, symbol, symbol_decrement_refcount);

symboltable *symboltable_new(void) {
    symboltable *this = NULL;

    waitui_log_trace("creating new symboltable");

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->currentScope = 0;
    this->symbols      = symbol_hashtable_new(SYMBOLTABLE_SIZE);
    if (!this->symbols) {
        waitui_log_fatal("could not allocate memory for symbol_hashtable");
        symboltable_destroy(&this);
        return NULL;
    }

    waitui_log_trace("new symboltable successful created");

    return this;
}

void symboltable_destroy(symboltable **this) {
    waitui_log_trace("destroying symboltable");

    if (!this || !(*this)) { return; }

    symbol_hashtable_destroy(&(*this)->symbols);

    free(*this);
    *this = NULL;

    waitui_log_trace("symboltable successful destroyed");
}

void symboltable_enter_scope(symboltable *this) {
    if (!this) { return; }

    this->currentScope++;

    waitui_log_debug("entering new scope: %ld", this->currentScope);
}

void symboltable_exit_scope(symboltable *this) {
    if (!this) { return; }

    waitui_log_debug("leaving scope: %ld", this->currentScope);

    for (unsigned long int i = 0; i < this->symbols->size; ++i) {
        if (this->symbols->list[i]) {
            waitui_hashtable_node *node = this->symbols->list[i];

            while (node &&
                   ((symbol *) node->value)->scope == this->currentScope) {
                waitui_hashtable_node *temp = node;

                node = temp->next;

                if (!temp->isStolen) {
                    this->symbols->valueDestroyFn(&temp->value);
                }

                STR_FREE(&temp->key);

                free(temp);
            }

            this->symbols->list[i] = node;
        }
    }

    waitui_log_trace("left current scope: %ld", this->currentScope);

    this->currentScope--;
}

void symboltable_enter_declaration_mode(symboltable *this) {
    if (!this) { return; }

    waitui_log_debug("entering declaration mode");

    this->declarationMode = 1;
}

void symboltable_leave_declaration_mode(symboltable *this) {
    if (!this) { return; }

    waitui_log_debug("leaving declaration mode");

    this->declarationMode = 0;
}

int symboltable_add_symbol(symboltable *this, str identifier,
                           symbol **newSymbol) {
    if (!this || !newSymbol || !(*newSymbol)) { goto error; }

    symbol *foundSymbol = symbol_hashtable_lookup(this->symbols, identifier);

    if (foundSymbol) {
        if (this->declarationMode) {
            waitui_symbol_reference *reference =
                    symbol_get_reference_head(*newSymbol);
            if (!reference) { goto error; }

            if (foundSymbol->scope == this->currentScope) {
                waitui_log_error("multiple declaration of identifier '%.*s' at "
                                 "%llu:%llu",
                                 STR_FMT(&identifier),
                                 waitui_symbol_reference_getLine(reference),
                                 waitui_symbol_reference_getColumn(reference));
                goto error;
            }

            (*newSymbol)->scope = this->currentScope;

            if (!symbol_hashtable_insert_check(this->symbols, identifier,
                                               *newSymbol,
                                               (waitui_hashtable_value_check_fn)
                                                       symboltable_symbol_check,
                                               *newSymbol)) {
                goto error;
            }

            symbol_increment_refcount(*newSymbol);

            waitui_log_debug(
                    "declaring new symbol with identifier '%.*s' in scope %ld",
                    STR_FMT(&identifier), this->currentScope);
        } else {
            waitui_symbol_reference *reference =
                    waitui_symbol_reference_list_pop((*newSymbol)->references);
            if (!reference) { goto error; }

            if (!waitui_symbol_reference_list_push(foundSymbol->references,
                                            reference)) {
                waitui_symbol_reference_destroy(&reference);
                goto error;
            }

            waitui_log_debug(
                    "referencing known symbol with identifier '%.*s' in "
                    "scope %ld",
                    STR_FMT(&identifier), this->currentScope);

            symbol_decrement_refcount(newSymbol);

            *newSymbol = foundSymbol;
        }
    } else {
        (*newSymbol)->scope = this->currentScope;

        if (!symbol_hashtable_insert_check(
                    this->symbols, identifier, *newSymbol,
                    (waitui_hashtable_value_check_fn) symboltable_symbol_check,
                    *newSymbol)) {
            goto error;
        }

        symbol_increment_refcount(*newSymbol);

        if (this->declarationMode) {
            waitui_log_debug(
                    "declaring new symbol with identifier '%.*s' in scope "
                    "%ld",
                    STR_FMT(&identifier), this->currentScope);
        } else {
            waitui_log_debug("referencing new symbol with identifier '%.*s' in "
                             "scope %ld",
                             STR_FMT(&identifier), this->currentScope);
        }
    }

    return 1;

error:
    return 0;
}

int symboltable_has(symboltable *this, str identifier) {
    if (!this) { return 0; }
    return symbol_hashtable_has(this->symbols, identifier);
}

symbol *symboltable_lookup(symboltable *this, str identifier) {
    if (!this) { return NULL; }
    return symbol_hashtable_lookup(this->symbols, identifier);
}
