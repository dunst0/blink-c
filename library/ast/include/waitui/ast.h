/**
 * @file ast.h
 * @author rick
 * @date 26.03.20
 * @brief File for the AST implementation
 */

#ifndef WAITUI_AST_H
#define WAITUI_AST_H

#include "waitui/ast_node.h"

#include <stdio.h>


// -----------------------------------------------------------------------------
//  Public defines
// -----------------------------------------------------------------------------

/**
 * @brief Execute the callbacks if set for the given node.
 */
#define AST_EXECUTE_CALLBACKS(callbacks, node, args)                           \
    if ((callbacks)->preNodeCallback) {                                        \
        (callbacks)->preNodeCallback((ast_node *) (node), (args));             \
    }                                                                          \
    if ((callbacks)->nodeCallback) {                                           \
        (callbacks)->nodeCallback((ast_node *) (node), (args));                \
    }                                                                          \
    if ((callbacks)->postNodeCallback) {                                       \
        (callbacks)->postNodeCallback((ast_node *) (node), (args));            \
    }


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
    ast_node_callback preNodeCallback;
    ast_node_callback nodeCallback;
    ast_node_callback postNodeCallback;
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
 * @param[in] callbacks The struct with the callbacks for the AST nodes.
 * @param[in] args The extra argument for the callbacks for the AST nodes
 */
extern void ast_walk(ast *this, ast_callbacks *callbacks, void *args);

/**
 * @brief Generate a dot graph description of the AST into the file.
 * @param[in] this The AST to visualize as graph
 * @param[int, out] file The file to write the dot graph description
 */
extern void ast_generate_graph(const ast *this, FILE *file);

#endif// WAITUI_AST_H
