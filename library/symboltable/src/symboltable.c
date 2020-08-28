/**
 * @file symboltable.c
 * @author rick
 * @date 21.07.20
 * @brief File for the SymbolTable implementation
 */

#include "waitui/symboltable.h"

#include <stdio.h>
#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Local defines
// -----------------------------------------------------------------------------

#define SYMBOLTABLE_SIZE 997


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

CREATE_HASHTABLE_TYPE_CUSTOM(IMPLEMENTATION, symbol, symbol,
                             symbol_decrement_refcount);

symboltable *symboltable_new(int debug) {
    symboltable *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->debug        = debug;
    this->currentScope = 0;
    this->symbols      = symbol_hashtable_new(SYMBOLTABLE_SIZE);
    if (!this->symbols) {
        symboltable_destroy(&this);
        return NULL;
    }

    return this;
}

void symboltable_destroy(symboltable **this) {
    if (!this || !(*this)) { return; }

    symbol_hashtable_destroy(&(*this)->symbols);

    free(*this);
    *this = NULL;
}

void symboltable_enter_scope(symboltable *this) {
    if (!this) { return; }

    this->currentScope++;
    if (this->debug) {
        fprintf(stderr, "Entering new scope: %ld\n", this->currentScope);
    }
}

void symboltable_exit_scope(symboltable *this) {
    if (!this) { return; }

    if (this->debug) {
        fprintf(stderr, "Leaving scope: %ld\n", this->currentScope);
    }

    for (unsigned long int i = 0; i < this->symbols->size; ++i) {
        if (this->symbols->list[i]) {
            hashtable_node *node = this->symbols->list[i];

            while (node &&
                   ((symbol *) node->value)->scope == this->currentScope) {
                hashtable_node *temp = node;

                node = temp->next;

                if (!temp->isStolen) {
                    this->symbols->valueDestroyCallback(&temp->value);
                }

                STR_FREE(&temp->key);

                free(temp);
            }

            this->symbols->list[i] = node;
        }
    }

    this->currentScope--;
}

void symboltable_enter_declaration_mode(symboltable *this) {
    if (!this) { return; }

    if (this->debug) { fprintf(stderr, "Entering declaration mode\n"); }

    this->declarationMode = 1;
}

void symboltable_leave_declaration_mode(symboltable *this) {
    if (!this) { return; }

    if (this->debug) { fprintf(stderr, "Leaving declaration mode\n"); }

    this->declarationMode = 0;
}

int symboltable_add_symbol(symboltable *this, str identifier,
                           symbol **newSymbol) {
    if (!this || !newSymbol || !(*newSymbol)) { goto error; }

    symbol *foundSymbol = symbol_hashtable_lookup(this->symbols, identifier);

    if (foundSymbol) {
        if (this->declarationMode) {
            symbol_reference *reference = symbol_get_reference_head(*newSymbol);
            if (!reference) { goto error; }

            if (foundSymbol->scope == this->currentScope) {
                fprintf(stderr,
                        "Error: Multiple declaration of variable %.*s at "
                        "%llu:%llu\n",
                        STR_FMT(&identifier), reference->line,
                        reference->column);
                symbol_reference_destroy(&reference);
                goto error;
            }

            (*newSymbol)->scope = this->currentScope;

            if (!symbol_hashtable_insert_check(
                        this->symbols, identifier, *newSymbol,
                        (hashtable_value_check) symboltable_symbol_check,
                        *newSymbol)) {
                goto error;
            }

            symbol_increment_refcount(*newSymbol);

            if (this->debug) {
                fprintf(stderr,
                        "Declaring new symbol with identifier '%.*s' in scope "
                        "%ld\n",
                        STR_FMT(&identifier), this->currentScope);
            }
        } else {
            symbol_reference *reference =
                    symbol_reference_list_pop((*newSymbol)->references);
            if (!reference) { goto error; }

            if (!symbol_reference_list_push(foundSymbol->references,
                                            reference)) {
                symbol_reference_destroy(&reference);
                goto error;
            }

            if (this->debug) {
                fprintf(stderr,
                        "Referencing known symbol with identifier '%.*s' in "
                        "scope %ld\n",
                        STR_FMT(&identifier), this->currentScope);
            }

            symbol_destroy(newSymbol);

            *newSymbol = foundSymbol;
        }
    } else {
        (*newSymbol)->scope = this->currentScope;

        if (!symbol_hashtable_insert_check(
                    this->symbols, identifier, *newSymbol,
                    (hashtable_value_check) symboltable_symbol_check,
                    *newSymbol)) {
            goto error;
        }

        symbol_increment_refcount(*newSymbol);

        if (this->debug) {
            if (this->declarationMode) {
                fprintf(stderr,
                        "Declaring new symbol with identifier '%.*s' in scope "
                        "%ld\n",
                        STR_FMT(&identifier), this->currentScope);
            } else {
                fprintf(stderr,
                        "Referencing new symbol with identifier '%.*s' in "
                        "scope %ld\n",
                        STR_FMT(&identifier), this->currentScope);
            }
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
