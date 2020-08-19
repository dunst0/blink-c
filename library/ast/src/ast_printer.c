/**
 * @file ast_printer.c
 * @author rick
 * @date 04.08.20
 * @brief File for the AST printer implementation
 */

#include "waitui/ast_printer.h"


// -----------------------------------------------------------------------------
//  Local types
// -----------------------------------------------------------------------------

/**
 * @brief Type for the printing the AST.
 */
typedef struct ast_printer {
    FILE *outFile;
    ast_callbacks *callbacks;
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
    fprintf(printer->outFile, "\tnode [shape=plain]\n");

    fprintf(printer->outFile,
            "\tprogram "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR><TD><B>%s</B></TD></TR>"
            "</TABLE>"
            ">];\n",
            "ast_program");

    for (node = program->classes->head; node; node = node->next) {
        ast_class *class = (ast_class *) node->element;
        fprintf(printer->outFile, "\tprogram -> ");
        AST_EXECUTE_CALLBACKS(printer->callbacks, class, args);
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

    str className      = STR_NULL_INIT;
    str superClassName = STR_NULL_INIT;

    if (class->name) { className = class->name->identifier; }
    if (class->superClass) { superClassName = class->superClass->identifier; }

    fprintf(printer->outFile, "class%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tclass%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">name:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">super:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s "
            "</TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_class", STR_FMT(&className),
            STR_FMT(&superClassName));

    if (class->parameters) {
        for (node = class->parameters->head; node; node = node->next) {
            ast_formal *formal = (ast_formal *) node->element;
            fprintf(printer->outFile, "\tclass%llu -> ", nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks, formal, args);
        }
    }

    if (class->functions) {
        for (node = class->functions->head; node; node = node->next) {
            ast_function *function = (ast_function *) node->element;
            fprintf(printer->outFile, "\tclass%llu -> ", nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks, function, args);
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

    str identifier = STR_NULL_INIT;
    str type       = STR_NULL_INIT;

    if (formal->identifier) { identifier = formal->identifier->identifier; }
    if (formal->type) { type = formal->type->identifier; }

    fprintf(printer->outFile, "formal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tformal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">identifier:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">type:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">isLazy:</TD>"
            "<TD ALIGN=\"LEFT\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_formal", STR_FMT(&identifier), STR_FMT(&type),
            (formal->isLazy ? "true" : "false"));
}

/**
 * @brief TODO
 * @param program TODO
 * @param args TODO
 */
static void ast_function_printer(ast_function *function, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str name       = STR_NULL_INIT;
    str returnType = STR_NULL_INIT;
    str visibility = STR_NULL_INIT;

    if (function->functionName) { name = function->functionName->identifier; }
    if (function->returnType) { returnType = function->returnType->identifier; }

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
            "\tfunction%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">name:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">returnType:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">visibility:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">isAbstract:</TD>"
            "<TD ALIGN=\"LEFT\"> %s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">isFinal:</TD>"
            "<TD ALIGN=\"LEFT\"> %s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">isOverwrite:</TD>"
            "<TD ALIGN=\"LEFT\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_function", STR_FMT(&name), STR_FMT(&returnType),
            STR_FMT(&visibility), (function->isAbstract ? "true" : "false"),
            (function->isFinal ? "true" : "false"),
            (function->isOverwrite ? "true" : "false"));

    if (function->body) {
        fprintf(printer->outFile, "\tfunction%llu -> ", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, function->body, args);
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

    str identifier = STR_NULL_INIT;
    str operator   = STR_NULL_INIT;

    if (assignment->identifier) {
        identifier = assignment->identifier->identifier;
    }

    switch (assignment->operator) {
        case AST_ASSIGNMENT_OPERATOR_EQUAL:
            STR_STATIC_SET(&operator, "=");
            break;
        case AST_ASSIGNMENT_OPERATOR_PLUS_EQUAL:
            STR_STATIC_SET(&operator, "+=");
            break;
        case AST_ASSIGNMENT_OPERATOR_MINUS_EQUAL:
            STR_STATIC_SET(&operator, "-=");
            break;
        case AST_ASSIGNMENT_OPERATOR_TIMES_EQUAL:
            STR_STATIC_SET(&operator, "*=");
            break;
        case AST_ASSIGNMENT_OPERATOR_DIV_EQUAL:
            STR_STATIC_SET(&operator, "/=");
            break;
        case AST_ASSIGNMENT_OPERATOR_MODULO_EQUAL:
            STR_STATIC_SET(&operator, "%=");
            break;
        case AST_ASSIGNMENT_OPERATOR_AND_EQUAL:
            STR_STATIC_SET(&operator, "&=");
            break;
        case AST_ASSIGNMENT_OPERATOR_CARET_EQUAL:
            STR_STATIC_SET(&operator, "^=");
            break;
        case AST_ASSIGNMENT_OPERATOR_TILDE_EQUAL:
            STR_STATIC_SET(&operator, "~=");
            break;
        case AST_ASSIGNMENT_OPERATOR_PIPE_EQUAL:
            STR_STATIC_SET(&operator, "|=");
            break;
    }

    fprintf(printer->outFile, "assignment%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tassignment%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">identifier:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">operator:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_assignment", STR_FMT(&identifier),
            STR_FMT(&operator));

    if (assignment->value) {
        fprintf(printer->outFile, "\tassignment%llu -> ", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, assignment->value, args);
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
            "\tstring_literal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">value:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_string_literal", STR_FMT(&stringLiteral->value));
}

/**
 * @brief TODO
 * @param program TODO
 * @param args TODO
 */
static void ast_integer_literal_printer(ast_integer_literal *integerLiteral,
                                        void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    fprintf(printer->outFile, "integer_literal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tinteger_literal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">value:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_integer_literal", STR_FMT(&integerLiteral->value));
}

/**
 * @brief TODO
 * @param program TODO
 * @param args TODO
 */
static void ast_null_literal_printer(ast_null_literal *nullLiteral,
                                     void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    fprintf(printer->outFile, "null_literal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tnull_literal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">value:</TD>"
            "<TD ALIGN=\"LEFT\"> null </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_null_literal");
}

/**
 * @brief TODO
 * @param program TODO
 * @param args TODO
 */
static void ast_boolean_literal_printer(ast_boolean_literal *booleanLiteral,
                                        void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    fprintf(printer->outFile, "boolean_literal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tboolean_literal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">value:</TD>"
            "<TD ALIGN=\"LEFT\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_boolean_literal",
            booleanLiteral->value ? "true" : "false");
}

static void ast_node_printer(ast_node *node, void *args) {
    switch (node->astNodeType) {
        case AST_NODE_TYPE_EXPRESSION:
            switch (((ast_expression *) node)->astExpressionType) {
                case AST_EXPRESSION_TYPE_INTEGER_LITERAL:
                    ast_integer_literal_printer((ast_integer_literal *) node,
                                                args);
                    break;
                case AST_EXPRESSION_TYPE_BOOLEAN_LITERAL:
                    ast_boolean_literal_printer((ast_boolean_literal *) node,
                                                args);
                    break;
                case AST_EXPRESSION_TYPE_DECIMAL_LITERAL:
                    break;
                case AST_EXPRESSION_TYPE_NULL_LITERAL:
                    ast_null_literal_printer((ast_null_literal *) node, args);
                    break;
                case AST_EXPRESSION_TYPE_STRING_LITERAL:
                    ast_string_literal_printer((ast_string_literal *) node,
                                               args);
                    break;
                case AST_EXPRESSION_TYPE_THIS_LITERAL:
                    break;
                case AST_EXPRESSION_TYPE_ASSIGNMENT:
                    ast_assignment_printer((ast_assignment *) node, args);
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
            break;
        case AST_NODE_TYPE_DEFINITION:
            switch (((ast_definition *) node)->astDefinitionType) {
                case AST_DEFINITION_TYPE_FORMAL:
                    ast_formal_printer((ast_formal *) node, args);
                    break;
                case AST_DEFINITION_TYPE_PROPERTY:
                    break;
                case AST_DEFINITION_TYPE_FUNCTION:
                    ast_function_printer((ast_function *) node, args);
                    break;
                case AST_DEFINITION_TYPE_CLASS:
                    ast_class_printer((ast_class *) node, args);
                    break;
                case AST_DEFINITION_TYPE_PROGRAM:
                    ast_program_printer((ast_program *) node, args);
                    break;
            }
            break;
    }
}


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

void ast_printer_print_ast_graph(const ast *this, FILE *file) {
    if (!this || !file) { return; }

    ast_callbacks callbacks = {
            .preNodeCallback  = NULL,
            .nodeCallback     = ast_node_printer,
            .postNodeCallback = NULL,
    };
    ast_printer printer = {
            .callbacks = &callbacks,
            .outFile   = file,
    };

    ast_walk((ast *) this, &callbacks, &printer);
}
