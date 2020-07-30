/**
 * @file ast.h
 * @author rick
 * @date 26.03.20
 * @brief File for the AST implementation
 */

#ifndef BLINK_AST_H
#define BLINK_AST_H

#include "blink/ast_node.h"

#include <stdio.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Type for the AST.
 */
typedef struct ast {
    ast_program *program;
} ast;

/**
 * @brief Type for AST callbacks for the ast_walk function.
 */
typedef struct ast_callbacks {
    ast_program_callback programCallback;
    ast_class_callback classCallback;
    ast_formal_callback formalCallback;
    ast_function_callback functionCallback;
    ast_expression_callback expressionCallback;
    ast_assignment_callback assignmentCallback;
    ast_string_literal_callback stringLiteralCallback;
} ast_callbacks;


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create the AST.
 * @param[in] program The program node for the AST
 * @return On success a pointer to AST, else NULL
 */
extern ast *ast_new(ast_program *program);

/**
 * @brief Destroy the AST and its content.
 * @param[in,out] this The AST to destroy
 */
extern void ast_destroy(ast **this);

/**
 * @brief Walk the AST with ast_callbacks and args.
 * @param[in,out] this The AST to walk
 * @param[in] callbacks TODO
 * @param args TODO
 */
extern void ast_walk(ast *this, ast_callbacks *callbacks, void *args);

/**
 * @brief Generate a dot graph description of the AST into the file.
 * @param[in] this The AST to visualize as graph
 * @param[out] file The file to write the dot graph description
 */
extern void ast_generate_graph(const ast *this, FILE *file);

#endif// BLINK_AST_H
