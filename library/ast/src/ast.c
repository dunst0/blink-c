//
// Created by rick on 26.03.20.
//

#include "blink/ast.h"

#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Local types
// -----------------------------------------------------------------------------

typedef struct ast_printer {
    FILE *outFile;
    ast_callbacks callbacks;
    unsigned long int classCount;
    unsigned long int formalCount;
    unsigned long int functionCount;
} ast_printer;


// -----------------------------------------------------------------------------
// Local functions
// -----------------------------------------------------------------------------

static void ast_program_printer(ast_program *program, void *args) {
    ast_printer *printer      = (ast_printer *) args;
    ast_class_list_node *node = NULL;

    fprintf(printer->outFile, "digraph AST {\n");
    fprintf(printer->outFile, "\tnode [shape=record]\n");
    fprintf(printer->outFile, "\tprogram [label=\"{ %s }\"];\n", "ast_program");
    for (node = program->classes->head; node; node = node->next) {
        fprintf(printer->outFile, "\tprogram -> ");
        printer->callbacks.classCallback(node->class, args);
    }
    fprintf(printer->outFile, "}\n");
}

static void ast_class_printer(ast_class *class, void *args) {
    ast_printer *printer = (ast_printer *) args;

    fprintf(printer->outFile, "class%lu;\n", printer->classCount);
    fprintf(printer->outFile,
            "\tclass%lu"
            " [label=\"{ %s"
            " | { name: %.*s  }"
            " | { super: %.*s }"
            " }\"];\n",
            printer->classCount, "ast_class", STR_FMT(&class->name),
            STR_FMT(&class->superClass));
    if (class->parameters) {
        ast_formal_list_node *node = NULL;
        for (node = class->parameters->head; node; node = node->next) {
            fprintf(printer->outFile, "\tclass%lu -> ", printer->classCount);
            printer->callbacks.formalCallback(node->formal, args);
        }
    }

    if (class->functions) {
        ast_function_list_node *node = NULL;
        for (node = class->functions->head; node; node = node->next) {
            fprintf(printer->outFile, "\tclass%lu -> ", printer->classCount);
            printer->callbacks.functionCallback(node->function, args);
        }
    }

    printer->classCount++;
}

static void ast_formal_printer(ast_formal *formal, void *args) {
    ast_printer *printer = (ast_printer *) args;

    fprintf(printer->outFile, "formal%lu;\n", printer->formalCount);
    fprintf(printer->outFile,
            "\tformal%lu"
            " [label=\"{ %s"
            " | { identifier: %.*s  }"
            " | { type: %.*s }"
            " | { isLazy: %s }"
            " }\"];\n",
            printer->formalCount, "ast_formal", STR_FMT(&formal->identifier),
            STR_FMT(&formal->type), (formal->isLazy ? "true" : "false"));

    printer->formalCount++;
}

static void ast_function_printer(ast_function *function, void *args) {
    ast_printer *printer = (ast_printer *) args;

    str visibility = STR_NULL;
    switch (function->visibility) {
        case AST_FUNCTION_VISIBILITY_PUBLIC:
            STR_STATIC_SET(&visibility, "public");
            break;
        case AST_FUNCTION_VISIBILITY_PROTECTED:
            STR_STATIC_SET(&visibility, "protected");
            break;
        case AST_FUNCTION_VISIBILITY_PRIVATE:
            STR_STATIC_SET(&visibility, "private");
            break;
    }

    fprintf(printer->outFile, "function%lu;\n", printer->functionCount);
    fprintf(printer->outFile,
            "\tfunction%lu"
            " [label=\"{ %s "
            " | { name: %.*s  }"
            " | { returnType: %.*s }"
            " | { visibility: %.*s }"
            " | { isAbstract: %s }"
            " | { isFinal: %s }"
            " | { isOverwrite: %s }"
            " }\"];\n",
            printer->functionCount, "ast_function", STR_FMT(&function->name),
            STR_FMT(&function->returnType), STR_FMT(&visibility),
            (function->isAbstract ? "true" : "false"),
            (function->isFinal ? "true" : "false"),
            (function->isOverwrite ? "true" : "false"));

    printer->functionCount++;
}


// -----------------------------------------------------------------------------
// Implementation of public functions
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

    if (callbacks->programCallback) {
        callbacks->programCallback(this->program, args);
    }
}

void ast_generate_graph(const ast *this, FILE *file) {
    ast_callbacks callbacks = {
            .programCallback  = ast_program_printer,
            .classCallback    = ast_class_printer,
            .formalCallback   = ast_formal_printer,
            .functionCallback = ast_function_printer,
    };
    ast_printer printer = {
            .callbacks = callbacks,
            .outFile   = file,
    };

    ast_walk((ast *) this, &callbacks, &printer);
}