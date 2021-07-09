/**
 * @file symboltable.h
 * @author rick
 * @date 21.07.20
 * @brief File for the SymbolTable implementation
 */

#ifndef WAITUI_SYMBOLTABLE_H
#define WAITUI_SYMBOLTABLE_H

#include "waitui/symbol.h"

#include <waitui/hashtable.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

CREATE_HASHTABLE_TYPE_CUSTOM(INTERFACE, symbol, symbol_decrement_refcount);

/**
 * @brief Type for the SymbolTable.
 */
typedef struct symboltable symboltable;


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create the SymbolTable.
 * @return On success a pointer to SymbolTable, else NULL
 */
extern symboltable *symboltable_new(void);

/**
 * @brief Destroy the SymbolTable and its content.
 * @param[in,out] this The SymbolTable to destroy
 */
extern void symboltable_destroy(symboltable **this);

/**
 * @brief Enter new scope for the SymbolTable.
 * @param[in] this The SymbolTable to enter a new scope
 */
extern void symboltable_enter_scope(symboltable *this);

/**
 * @brief Exit current scope of the SymbolTable.
 * @param[in] this The SymbolTable to exit the current scope
 */
extern void symboltable_exit_scope(symboltable *this);

/**
 * @brief Enter declaration mode for the SymbolTable.
 * @param[in] this The SymbolTable to enter declaration mode
 */
extern void symboltable_enter_declaration_mode(symboltable *this);

/**
 * @brief Leave declaration mode for the SymbolTable.
 * @param[in] this The SymbolTable to leave declaration mode
 */
extern void symboltable_leave_declaration_mode(symboltable *this);

/**
 * @brief Add a Symbol to the SymbolTable.
 * @param[in] this The SymbolTable to add the Symbol
 * @param[in] identifier The identifier for which to add the SymbolTable
 * @param[in,out] newSymbol The Symbol to add
 * @retval 1 Ok
 * @retval 0 Double declaration or Memory allocation failed
 */
extern int symboltable_add_symbol(symboltable *this, str identifier,
                                  symbol **newSymbol);
/**
 * @brief TODO
 * @param this
 * @param identifier
 * @return
 */
extern int symboltable_has(symboltable *this, str identifier);

/**
 * @brief TODO
 * @param this
 * @param identifier
 * @return
 */
extern symbol *symboltable_lookup(symboltable *this, str identifier);

#endif//WAITUI_SYMBOLTABLE_H
