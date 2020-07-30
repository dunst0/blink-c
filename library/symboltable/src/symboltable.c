/**
 * @file symboltable.c
 * @author rick
 * @date 21.07.20
 * @brief File for the SymbolTable implementation
 */

#include "blink/symboltable.h"

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
        free(this);
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
                free(temp->key.s);
                free(temp);
            }
        }
    }

    this->currentScope--;

    return this->currentScope >= 0;
}

int symboltable_add_symbol(symboltable *this, symbol *newSymbol) {
    if (!this || !newSymbol) { return 0; }

    symbol *foundSymbol =
            symbol_hashtable_lookup(this->symbols, newSymbol->identifier);
    if (foundSymbol && foundSymbol->scope == this->currentScope) { return 0; }

    newSymbol->scope = this->currentScope;
    if (!symbol_hashtable_insert(this->symbols, newSymbol->identifier,
                                 newSymbol)) {
        return 0;
    }

    return 1;
}

int symboltable_add_reference(symboltable *this, str identifier,
                              symbol_reference *newReference) {
    if (!this || !newReference) { return 0; }

    symbol *foundSymbol = symbol_hashtable_lookup(this->symbols, identifier);
    if (!foundSymbol) { return 0; }

    if (!symbol_reference_list_push(foundSymbol->references, newReference)) {
        return 0;
    }

    return 1;
}

int symboltable_has(symboltable *this, str identifier) {
    if (!this) { return 0; }

    return symbol_hashtable_has(this->symbols, identifier);
}

symbol *symboltable_lookup(symboltable *this, str identifier) {
    if (!this) { return NULL; }

    return symbol_hashtable_lookup(this->symbols, identifier);
}
