/**
 * @file symbol.h
 * @author rick
 * @date 29.07.20
 * @brief File for the Symbol implementation
 */

#ifndef BLINK_SYMBOL_H
#define BLINK_SYMBOL_H

#include <blink/list.h>
#include <blink/str.h>


// -----------------------------------------------------------------------------
//  Types
// -----------------------------------------------------------------------------

/**
 * @brief Type for the SymbolReference.
 */
typedef struct symbol_reference {
    unsigned long long line;
    unsigned long long column;
} symbol_reference;

CREATE_LIST_TYPE(INTERFACE, symbol_reference, reference)

/**
 * @brief Symbol types possible for a Symbol.
 */
typedef enum symbol_type {
    SYMBOL_TYPE_IDENTIFIER,
    SYMBOL_TYPE_CLASS,
    //TODO: add more types
} symbol_type;

/**
 * @brief Type for the Symbol.
 */
typedef struct symbol {
    unsigned long long line;
    unsigned long long column;
    long int scope;
    str identifier;
    symbol_type type;
    symbol_reference_list *references;
} symbol;


// -----------------------------------------------------------------------------
// Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create the SymbolReference.
 * @param[in] line TODO
 * @param[in] column TODO
 * @return On success a pointer to SymbolReference, else NULL
 */
extern symbol_reference *symbol_reference_new(unsigned long int line,
                                              unsigned long int column);

/**
 * @brief Destroy the SymbolReference and its content.
 * @param[in,out] this The SymbolReference to destroy
 */
extern void symbol_reference_destroy(symbol_reference **this);

/**
 * @brief Create the Symbol.
 * @param[in] line TODO
 * @param[in] column TODO
 * @param[in] identifier TODO
 * @param[in] type TODO
 * @return On success a pointer to Symbol, else NULL
 */
extern symbol *symbol_new(unsigned long int line, unsigned long int column,
                          str identifier, symbol_type type);

/**
 * @brief Destroy the Symbol and its content.
 * @param[in,out] this The Symbol to destroy
 */
extern void symbol_destroy(symbol **this);

#endif//BLINK_SYMBOL_H
