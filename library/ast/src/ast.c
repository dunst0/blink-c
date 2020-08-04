/**
 * @file ast.c
 * @author rick
 * @date 26.03.20
 * @brief File for the AST implementation
 */

#include "blink/ast.h"

#include "blink/ast_printer.h"

#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

ast *ast_new(ast_program *program) {
    ast *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->program = program;

    return this;
}

void ast_destroy(ast **this) {
    if (!this || !(*this)) { return; }

    ast_program_destroy(&(*this)->program);

    free(*this);
    *this = NULL;
}

void ast_walk(ast *this, ast_callbacks *callbacks, void *args) {
    if (!this || !callbacks) { return; }

    AST_EXECUTE_CALLBACKS(callbacks, this->program, args);
}

void ast_generate_graph(const ast *this, FILE *file) {
    if (!this || !file) { return; }

    ast_printer_print_ast_graph(this, file);
}
