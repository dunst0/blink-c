/**
 * @file waitui_ast.h
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
#define WAITUI_AST_EXECUTE_CALLBACKS(callbacks, node, args)                    \
    do {                                                                       \
        if ((callbacks)->preNodeCallback) {                                    \
            (callbacks)->preNodeCallback((waitui_ast_node *) (node), (args));  \
        }                                                                      \
        if ((callbacks)->nodeCallback) {                                       \
            (callbacks)->nodeCallback((waitui_ast_node *) (node), (args));     \
        }                                                                      \
        if ((callbacks)->postNodeCallback) {                                   \
            (callbacks)->postNodeCallback((waitui_ast_node *) (node), (args)); \
        }                                                                      \
    } while (0)


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Type for the AST.
 */
typedef struct waitui_ast {
    waitui_ast_program *program;
} waitui_ast;

/**
 * @brief Type for AST callbacks for the waitui_ast_walk function.
 */
typedef struct waitui_ast_callbacks {
    waitui_ast_node_callback preNodeCallback;
    waitui_ast_node_callback nodeCallback;
    waitui_ast_node_callback postNodeCallback;
} waitui_ast_callbacks;


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create the AST.
 * @param[in] program The program node for the AST
 * @return On success a pointer to AST, else NULL
 */
extern waitui_ast *waitui_ast_new(waitui_ast_program *program);

/**
 * @brief Destroy the AST and its content.
 * @param[in,out] this The AST to destroy
 */
extern void ast_destroy(waitui_ast **this);

/**
 * @brief Get the program ast node from the the AST.
 * @param[in,out] this The AST to get the program from
 * @return On success a pointer to waitui_ast_program, else NULL
 */
extern waitui_ast_program *waitui_ast_getProgram(waitui_ast *this);

/**
 * @brief Walk the AST with waitui_ast_callbacks and args.
 * @param[in,out] this The AST to walk
 * @param[in] callbacks The struct with the callbacks for the AST nodes.
 * @param[in] args The extra argument for the callbacks for the AST nodes
 */
extern void waitui_ast_walk(waitui_ast *this, waitui_ast_callbacks *callbacks,
                            void *args);

#endif// WAITUI_AST_H
