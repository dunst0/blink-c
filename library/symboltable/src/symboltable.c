/**
 * @file symboltable.c
 * @author rick
 * @date 21.07.20
 * @brief File for the SymbolTable implementation
 */

#include "blink/symboltable.h"

#include <stdio.h>
#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Local defines
// -----------------------------------------------------------------------------

#define SYMBOLTABLE_SIZE 997


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

CREATE_HASHTABLE_TYPE(IMPLEMENTATION, symbol, symbol);

symboltable *symboltable_new() {
    symboltable *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

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

    return 1;
}

int symboltable_exit_scope(symboltable *this) {
    if (!this) { return 0; }

    for (unsigned long int i = 0; i < this->symbols->size; ++i) {
        while (this->symbols->list[i]) {
            hashtable_node *temp = this->symbols->list[i];
            symbol *tempSymbol   = (symbol *) temp->value;

            if (tempSymbol->scope == this->currentScope) {
                this->symbols->list[i] = this->symbols->list[i]->next;
                if (!temp->isStolen) {
                    this->symbols->valueDestroyCallback(&temp->value);
                }

                STR_FREE(&temp->key);

                free(temp);
            }
        }
    }

    this->currentScope--;

    return this->currentScope >= 0;
}

void symboltable_enter_declaration_mode(symboltable *this) {
    if (!this) { return; }

    this->declarationMode = 1;
}

void symboltable_leave_declaration_mode(symboltable *this) {
    if (!this) { return; }

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

        if (!symbol_hashtable_insert(this->symbols, identifier, foundSymbol)) {
            goto error_destroy_symbol;
        }
    } else {
        if (!symbol_reference_list_push(foundSymbol->references, reference)) {
            goto error_destroy_reference;
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
