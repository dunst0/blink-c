/**
 * @file symboltable.h
 * @author rick
 * @date 21.07.20
 * @brief File for the SymbolTable implementation
 */

#ifndef BLINK_SYMBOLTABLE_H
#define BLINK_SYMBOLTABLE_H

#include "blink/symbol.h"

#include <blink/hashtable.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

CREATE_HASHTABLE_TYPE(INTERFACE, symbol, symbol);

/**
 * @brief Type for the SymbolTable.
 */
typedef struct symboltable {
    long int currentScope;
    symbol_hashtable *symbols;
} symboltable;


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create the SymbolTable.
 * @return On success a pointer to SymbolTable, else NULL
 */
extern symboltable *symboltable_new();

/**
 * @brief Destroy the SymbolTable and its content.
 * @param[in,out] this The SymbolTable to destroy
 */
extern void symboltable_destroy(symboltable **this);

/**
 * @brief TODO
 * @param this TODO
 * @return TODO
 */
extern int symboltable_enter_scope(symboltable *this);

/**
 * @brief TODO
 * @param this TODO
 * @return TODO
 */
extern int symboltable_exit_scope(symboltable *this);

/**
 * @brief TODO
 * @param this TODO
 * @param newSymbol TODO
 * @return TODO
 */
extern int symboltable_add_symbol(symboltable *this, symbol *newSymbol);

/**
 * @brief TODO
 * @param this TODO
 * @param identifier TODO
 * @param newReference TODO
 * @return TODO
 */
extern int symboltable_add_reference(symboltable *this, str identifier,
                                     symbol_reference *newReference);

/**
 * @brief TODO
 * @param this TODO
 * @param identifier TODO
 * @return TODO
 */
extern int symboltable_has(symboltable *this, str identifier);

/**
 * @brief TODO
 * @param this TODO
 * @param identifier TODO
 * @return TODO
 */
extern symbol *symboltable_lookup(symboltable *this, str identifier);

#endif//BLINK_SYMBOLTABLE_H
