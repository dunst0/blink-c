/**
 * @file waitui_ast_printer.h
 * @author rick
 * @date 04.08.20
 * @brief File for the AST printer implementation
 */

#ifndef WAITUI_AST_PRINTER_H
#define WAITUI_AST_PRINTER_H

#include <waitui/ast.h>


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Generate a dot graph description of the AST into the file.
 * @param[in] this The AST to visualize as graph
 * @param[int, out] file The file to write the dot graph description
 */
extern void waitui_ast_printer_generateGraph(const waitui_ast *ast, FILE *file);

#endif//WAITUI_AST_PRINTER_H
