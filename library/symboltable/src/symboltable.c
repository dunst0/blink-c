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
 * @brief TODO
 * @return
 */
static int symboltable_symbol_check(symbol *this, symbol *other) {
    return this->scope == other->scope;
}


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

CREATE_HASHTABLE_TYPE(IMPLEMENTATION, symbol, symbol);

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

int symboltable_enter_scope(symboltable *this) {
    if (!this) { return 0; }

    this->currentScope++;
    if (this->debug) {
        fprintf(stderr, "Entering new scope: %ld\n", this->currentScope);
    }

    return 1;
}

int symboltable_exit_scope(symboltable *this) {
    if (!this) { return 0; }

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

    return this->currentScope >= 0;
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

int symboltable_add_symbol(symboltable *this, str identifier, symbol_type type,
                           unsigned long int line, unsigned long int column,
                           symbol **resultSymbol) {
    if (!this) { return 0; }

    symbol *foundSymbol = symbol_hashtable_lookup(this->symbols, identifier);

    symbol_reference *reference = symbol_reference_new(line, column);
    if (!reference) { goto error; }

    if (!foundSymbol || this->declarationMode) {
        if (foundSymbol && foundSymbol->scope == this->currentScope) {
            fprintf(stderr,
                    "Error: Multiple declaration of variable %.*s at %lu:%lu\n",
                    STR_FMT(&identifier), line, column);
            goto error_destroy_reference;
        }

        foundSymbol = symbol_new(identifier, type);
        if (!foundSymbol) { goto error_destroy_reference; }

        foundSymbol->scope = this->currentScope;

        if (!symbol_reference_list_push(foundSymbol->references, reference)) {
            goto error_destroy_symbol;
        }

        if (!symbol_hashtable_insert_check(
                    this->symbols, identifier, foundSymbol,
                    (hashtable_value_check) symboltable_symbol_check,
                    foundSymbol)) {
            goto error_destroy_symbol;
        }

        if (this->debug) {
            if (this->declarationMode) {
                fprintf(stderr,
                        "Declaring new identifier '%.*s' in scope %ld\n",
                        STR_FMT(&identifier), this->currentScope);
            } else {
                fprintf(stderr,
                        "Referencing new identifier '%.*s' in scope %ld\n",
                        STR_FMT(&identifier), this->currentScope);
            }
        }
    } else {
        if (!symbol_reference_list_push(foundSymbol->references, reference)) {
            goto error_destroy_reference;
        }
        if (this->debug) {
            fprintf(stderr,
                    "Referencing known identifier '%.*s' in scope %ld\n",
                    STR_FMT(&identifier), this->currentScope);
        }
    }

    if (resultSymbol) { *resultSymbol = foundSymbol; }

    return 1;

error_destroy_symbol:
    symbol_destroy(&foundSymbol);
error_destroy_reference:
    symbol_reference_destroy(&reference);
error:
    if (resultSymbol) { *resultSymbol = NULL; }
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

int symboltable_mark_symbol_stolen(symboltable *this, symbol *stolenSymbol) {
    if (!this || !stolenSymbol) { return 0; }

    if (this->debug) {
        fprintf(stderr,
                "Mark symbol with identifier '%.*s' in scope %ld as stolen\n",
                STR_FMT(&stolenSymbol->identifier), stolenSymbol->scope);
    }

    return symbol_hashtable_mark_stolen_check(
            this->symbols, stolenSymbol->identifier,
            (hashtable_value_check) symboltable_symbol_check, stolenSymbol);
}
