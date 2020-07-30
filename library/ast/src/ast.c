/**
 * @file ast.c
 * @author rick
 * @date 26.03.20
 * @brief File for the AST implementation
 */

#include "blink/ast.h"

#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Local types
// -----------------------------------------------------------------------------

/**
 * @brief Type for the printing the AST.
 */
typedef struct ast_printer {
    FILE *outFile;
    ast_callbacks callbacks;
    unsigned long long nodeCount;
} ast_printer;


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

/**
 * @brief TODO
 * @param program TODO
 * @param args TODO
 */
static void ast_program_printer(ast_program *program, void *args) {
    ast_printer *printer = (ast_printer *) args;
    list_node *node      = NULL;

    fprintf(printer->outFile, "digraph AST {\n");
    fprintf(printer->outFile, "\tnode [shape=record]\n");
    fprintf(printer->outFile, "\tprogram [label=\"{ %s }\"];\n", "ast_program");
    for (node = program->classes->head; node; node = node->next) {
        ast_class *class = (ast_class *) node->element;
        fprintf(printer->outFile, "\tprogram -> ");
        printer->callbacks.classCallback(class, args);
    }
    fprintf(printer->outFile, "}\n");
}

/**
 * @brief TODO
 * @param program TODO
 * @param args TODO
 */
static void ast_class_printer(ast_class *class, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    fprintf(printer->outFile, "class%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tclass%llu"
            " [label=\"{ %s"
            " | { name: %.*s  }"
            " | { super: %.*s }"
            " }\"];\n",
            nodeCount, "ast_class", STR_FMT(&class->name),
            STR_FMT(&class->superClass));
    if (class->parameters) {
        for (node = class->parameters->head; node; node = node->next) {
            ast_formal *formal = (ast_formal *) node->element;
            fprintf(printer->outFile, "\tclass%llu -> ", nodeCount);
            printer->callbacks.formalCallback(formal, args);
        }
    }

    if (class->functions) {
        for (node = class->functions->head; node; node = node->next) {
            ast_function *function = (ast_function *) node->element;
            fprintf(printer->outFile, "\tclass%llu -> ", nodeCount);
            printer->callbacks.functionCallback(function, args);
        }
    }
}

/**
 * @brief TODO
 * @param program TODO
 * @param args TODO
 */
static void ast_formal_printer(ast_formal *formal, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    fprintf(printer->outFile, "formal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tformal%llu"
            " [label=\"{ %s"
            " | { identifier: %.*s  }"
            " | { type: %.*s }"
            " | { isLazy: %s }"
            " }\"];\n",
            nodeCount, "ast_formal", STR_FMT(&formal->identifier),
            STR_FMT(&formal->type), (formal->isLazy ? "true" : "false"));
}

/**
 * @brief TODO
 * @param program TODO
 * @param args TODO
 */
static void ast_function_printer(ast_function *function, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

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

    fprintf(printer->outFile, "function%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tfunction%llu"
            " [label=\"{ %s "
            " | { name: %.*s  }"
            " | { returnType: %.*s }"
            " | { visibility: %.*s }"
            " | { isAbstract: %s }"
            " | { isFinal: %s }"
            " | { isOverwrite: %s }"
            " }\"];\n",
            nodeCount, "ast_function", STR_FMT(&function->name),
            STR_FMT(&function->returnType), STR_FMT(&visibility),
            (function->isAbstract ? "true" : "false"),
            (function->isFinal ? "true" : "false"),
            (function->isOverwrite ? "true" : "false"));
    if (function->body) {
        fprintf(printer->outFile, "\tfunction%llu -> ", nodeCount);
        printer->callbacks.expressionCallback(function->body, args);
    }
}

/**
 * @brief TODO
 * @param program TODO
 * @param args TODO
 */
static void ast_expression_printer(ast_expression *expression, void *args) {
    ast_printer *printer = (ast_printer *) args;

    switch (expression->astExpressionType) {
        case AST_EXPRESSION_TYPE_INTEGER_LITERAL:
            break;
        case AST_EXPRESSION_TYPE_BOOLEAN_LITERAL:
            break;
        case AST_EXPRESSION_TYPE_DECIMAL_LITERAL:
            break;
        case AST_EXPRESSION_TYPE_NULL_LITERAL:
            break;
        case AST_EXPRESSION_TYPE_STRING_LITERAL:
            printer->callbacks.stringLiteralCallback(
                    (ast_string_literal *) expression, args);
            break;
        case AST_EXPRESSION_TYPE_THIS_LITERAL:
            break;
        case AST_EXPRESSION_TYPE_ASSIGNMENT:
            printer->callbacks.assignmentCallback((ast_assignment *) expression,
                                                  args);
            break;
        case AST_EXPRESSION_TYPE_REFERENCE:
            break;
        case AST_EXPRESSION_TYPE_CAST:
            break;
        case AST_EXPRESSION_TYPE_INITIALIZATION:
            break;
        case AST_EXPRESSION_TYPE_LET:
            break;
        case AST_EXPRESSION_TYPE_BLOCK:
            break;
        case AST_EXPRESSION_TYPE_CONSTRUCTOR_CALL:
            break;
        case AST_EXPRESSION_TYPE_FUNCTION_CALL:
            break;
        case AST_EXPRESSION_TYPE_SUPER_FUNCTION_CALL:
            break;
        case AST_EXPRESSION_TYPE_BINARY_EXPRESSION:
            break;
        case AST_EXPRESSION_TYPE_UNARY_EXPRESSION:
            break;
        case AST_EXPRESSION_TYPE_IF_ELSE:
            break;
        case AST_EXPRESSION_TYPE_WHILE:
            break;
        case AST_EXPRESSION_TYPE_LAZY_EXPRESSION:
            break;
        case AST_EXPRESSION_TYPE_NATIVE_EXPRESSION:
            break;
    }
}

/**
 * @brief TODO
 * @param program TODO
 * @param args TODO
 */
static void ast_assignment_printer(ast_assignment *assignment, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    fprintf(printer->outFile, "assignment%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tassignment%llu"
            " [label=\"{ %s "
            " | { identifier: %.*s  }"
            " | { operator: %.*s }"
            " }\"];\n",
            nodeCount, "ast_assignment", STR_FMT(&assignment->identifier),
            STR_FMT(&assignment->operator));

    if (assignment->value) {
        fprintf(printer->outFile, "\tassignment%llu -> ", nodeCount);
        printer->callbacks.expressionCallback(assignment->value, args);
    }
}

/**
 * @brief TODO
 * @param program TODO
 * @param args TODO
 */
static void ast_string_literal_printer(ast_string_literal *stringLiteral,
                                       void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    fprintf(printer->outFile, "string_literal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tstring_literal%llu"
            " [label=\"{ %s "
            " | { value: %.*s }"
            " }\"];\n",
            nodeCount, "ast_string_literal", STR_FMT(&stringLiteral->value));
}


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

    if (callbacks->programCallback) {
        callbacks->programCallback(this->program, args);
    }
}

void ast_generate_graph(const ast *this, FILE *file) {
    ast_callbacks callbacks = {
            .programCallback       = ast_program_printer,
            .classCallback         = ast_class_printer,
            .formalCallback        = ast_formal_printer,
            .functionCallback      = ast_function_printer,
            .expressionCallback    = ast_expression_printer,
            .assignmentCallback    = ast_assignment_printer,
            .stringLiteralCallback = ast_string_literal_printer,
    };
    ast_printer printer = {
            .callbacks = callbacks,
            .outFile   = file,
    };

    ast_walk((ast *) this, &callbacks, &printer);
}
