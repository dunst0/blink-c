//
// Created by rick on 26.03.20.
//

#include "blink/ast.h"

#include <stdlib.h>

ast *ast_new() {
  ast *this = NULL;

  this = calloc(1, sizeof(*this));
  if (!this) {
    return NULL;
  }

  return this;
}

void ast_destroy(ast **this) {
  if (!this || !(*this)) {
    return;
  }

  ast_program_destroy(&(*this)->program);

  free(*this);
  *this = NULL;
}

void ast_setProgram(ast *this, ast_program *program) {
  if (!this) {
    return;
  }

  this->program = program;
}

void ast_walk(ast *this, ast_callbacks *callbacks) {
  if (!this || !callbacks) {
    return;
  }

  if (callbacks->programCallback) {
    callbacks->programCallback(this->program, callbacks);
  }
}
