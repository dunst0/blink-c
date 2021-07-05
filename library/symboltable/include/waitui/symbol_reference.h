/**
 * @file symbol_reference.h
 * @author rick
 * @date 28.08.20
 * @brief File for the SymbolReference implementation
 */

#ifndef WAITUI_SYMBOL_REFERENCE_H
#define WAITUI_SYMBOL_REFERENCE_H

#include <waitui/list.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Type for the SymbolReference.
 */
typedef struct symbol_reference {
    unsigned long long line;
    unsigned long long column;
} symbol_reference;

CREATE_LIST_TYPE(INTERFACE, symbol_reference)


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create the SymbolReference.
 * @param[in] line Line of the SymbolReference
 * @param[in] column Column of the SymbolReference
 * @return On success a pointer to SymbolReference, else NULL
 */
extern symbol_reference *symbol_reference_new(unsigned long int line,
                                              unsigned long int column);

/**
 * @brief Destroy the SymbolReference and its content.
 * @param[in,out] this The SymbolReference to destroy
 */
extern void symbol_reference_destroy(symbol_reference **this);

#endif//WAITUI_SYMBOL_REFERENCE_H
