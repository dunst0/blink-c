//
// Created by rick on 26.03.20.
//

#ifndef BLINK_AST_H
#define BLINK_AST_H

#include "blink/ast_node.h"

typedef struct ast {
  ast_program *program;
} ast;

typedef struct ast_callbacks {
  ast_program_callback programCallback;
  ast_class_callback classCallback;
} ast_callbacks;

extern ast *ast_new();
extern void ast_destroy(ast **this);

extern void ast_setProgram(ast *this, ast_program *program);

extern void ast_walk(ast *this, ast_callbacks *callbacks);

#endif // BLINK_AST_H
