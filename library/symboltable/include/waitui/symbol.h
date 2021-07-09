/**
 * @file symbol.h
 * @author rick
 * @date 29.07.20
 * @brief File for the Symbol implementation
 */

#ifndef WAITUI_SYMBOL_H
#define WAITUI_SYMBOL_H

#include "symbol_reference.h"

#include <waitui/list.h>
#include <waitui/str.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Symbol types possible for a Symbol.
 */
typedef enum symbol_type {
    SYMBOL_TYPE_UNDEFINED,
    SYMBOL_TYPE_IDENTIFIER,
    SYMBOL_TYPE_CLASS_NAME,
    SYMBOL_TYPE_FUNCTION_NAME,
} symbol_type;

/**
 * @brief Type for the Symbol.
 */
typedef struct symbol {
    long int scope;
    str identifier;
    symbol_type type;
    waitui_symbol_reference_list *references;
    long int refcount;
} symbol;


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create the Symbol.
 * @param[in] identifier Identifier of the Symbol
 * @param[in] type Type of the Symbol
 * @param[in] line Line of the Symbol
 * @param[in] column Column of the Symbol
 * @return On success a pointer to Symbol, else NULL
 */
extern symbol *symbol_new(str identifier, symbol_type type,
                          unsigned long int line, unsigned long int column);

/**
 * @brief Increment the refcount of the symbol.
 * @param this The Symbol to increment the refcount.
 */
extern void symbol_increment_refcount(symbol *this);

/**
 * @brief Decrement the refcount of the symbol.
 * @param[in,out] this The Symbol to decrement the refcount.
 * @note When refcount is less or equal zero symbol_destroy will be called.
 */
extern void symbol_decrement_refcount(symbol **this);

/**
 * @brief Get the head of the Symbol reference list.
 * @param[in] this The Symbol to get the head of the reference list.
 * @return On success a pointer to SymbolReference, else NULL
 */
extern waitui_symbol_reference *symbol_get_reference_head(symbol *this);

/**
 * @brief Destroy the Symbol and its content.
 * @param[in,out] this The Symbol to destroy
 */
extern void symbol_destroy(symbol **this);

#endif//WAITUI_SYMBOL_H
