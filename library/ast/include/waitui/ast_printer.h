/**
 * @file ast_printer.h
 * @author rick
 * @date 04.08.20
 * @brief File for the AST printer implementation
 */

#ifndef WAITUI_AST_PRINTER_H
#define WAITUI_AST_PRINTER_H

#include "waitui/ast.h"


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Print the dot graph for the given AST into the file pointer.
 * @param[in] this The AST to print the graph for
 * @param[in,out] file The file pointer to print into
 */
extern void ast_printer_print_ast_graph(const ast *this, FILE *file);

#endif //WAITUI_AST_PRINTER_H
