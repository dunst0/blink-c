/**
 * @file ast_printer.h
 * @author rick
 * @date 04.08.20
 * @brief File for the AST printer implementation
 */

#ifndef BLINK_AST_PRINTER_H
#define BLINK_AST_PRINTER_H

#include "blink/ast.h"


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Print the dot graph for the given AST into the file pointer.
 * @param[in] this The AST to print the graph for
 * @param[in,out] file The file pointer to print into
 */
extern void ast_printer_print_ast_graph(const ast *this, FILE *file);

#endif//BLINK_AST_PRINTER_H
