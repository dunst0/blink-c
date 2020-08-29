/**
 * @file ast.c
 * @author rick
 * @date 26.03.20
 * @brief File for the AST implementation
 */

#include "waitui/ast.h"

#include "waitui/ast_printer.h"

#include <waitui/log.h>

#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

ast *ast_new(ast_program *program) {
    ast *this = NULL;

    log_trace("creating new ast");

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->program = program;

    log_trace("new ast successful created");

    return this;
}

void ast_destroy(ast **this) {
    log_trace("destroying ast");

    if (!this || !(*this)) { return; }

    ast_program_destroy(&(*this)->program);

    free(*this);
    *this = NULL;

    log_trace("ast successful destroyed");
}

void ast_walk(ast *this, ast_callbacks *callbacks, void *args) {
    log_trace("start walking the ast");

    if (!this || !callbacks) { return; }

    AST_EXECUTE_CALLBACKS(callbacks, this->program, args);

    log_trace("end walking the ast");
}

void ast_generate_graph(const ast *this, FILE *file) {
    log_trace("start generating the ast graph");

    if (!this || !file) { return; }

    ast_printer_print_ast_graph(this, file);

    log_trace("end generating the ast graph");
}
