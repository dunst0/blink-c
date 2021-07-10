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
typedef struct waitui_symbol_reference waitui_symbol_reference;

CREATE_LIST_TYPE(INTERFACE, waitui_symbol_reference)


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create the SymbolReference.
 * @param[in] line Line of the SymbolReference
 * @param[in] column Column of the SymbolReference
 * @return On success a pointer to SymbolReference, else NULL
 */
extern waitui_symbol_reference *
waitui_symbol_reference_new(unsigned long int line, unsigned long int column);

/**
 * @brief Destroy the SymbolReference and its content.
 * @param[in,out] this The SymbolReference to destroy
 */
extern void waitui_symbol_reference_destroy(waitui_symbol_reference **this);

/**
 * @brief Get the line where the reference exists.
 * @param[in] this The SymbolReference to get the line from.
 * @return
 */
extern unsigned long long
waitui_symbol_reference_getLine(waitui_symbol_reference *this);

/**
 * @brief Get the column where the reference exists.
 * @param[in] this The SymbolReference to get the column from.
 * @return
 */
extern unsigned long long
waitui_symbol_reference_getColumn(waitui_symbol_reference *this);

#endif//WAITUI_SYMBOL_REFERENCE_H
