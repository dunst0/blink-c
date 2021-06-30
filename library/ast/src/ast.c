/**
 * @file waitui_ast.c
 * @author rick
 * @date 26.03.20
 * @brief File for the AST implementation
 */

#include "waitui/ast.h"

#include <waitui/log.h>

#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Local types
// -----------------------------------------------------------------------------

/**
 * @brief Struct representing the AST.
 */
struct waitui_ast {
    waitui_ast_program *program;
};


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

waitui_ast *waitui_ast_new(waitui_ast_program *program) {
    waitui_ast *this = NULL;

    waitui_log_trace("creating new waitui_ast");

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->program = program;

    waitui_log_trace("new waitui_ast successful created");

    return this;
}

void ast_destroy(waitui_ast **this) {
    waitui_log_trace("destroying waitui_ast");

    if (!this || !(*this)) { return; }

    waitui_ast_program_destroy(&(*this)->program);

    free(*this);
    *this = NULL;

    waitui_log_trace("waitui_ast successful destroyed");
}

waitui_ast_program *waitui_ast_getProgram(waitui_ast *this) {
    waitui_log_trace("get the waitui_ast_program from waitui_ast");

    if (!this) { return NULL; }

    return this->program;
}

void waitui_ast_walk(waitui_ast *this, waitui_ast_callbacks *callbacks,
                     void *args) {
    waitui_log_trace("start walking the waitui_ast");

    if (!this || !callbacks) { return; }

    WAITUI_AST_EXECUTE_CALLBACKS(callbacks, this->program, args);

    waitui_log_trace("end walking the waitui_ast");
}
