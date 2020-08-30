/**
 * @file ast_printer.c
 * @author rick
 * @date 04.08.20
 * @brief File for the AST printer implementation
 */

#include "waitui/ast_printer.h"

#include <stdarg.h>


// -----------------------------------------------------------------------------
//  Local defines
// -----------------------------------------------------------------------------

#define TABLE_BEGIN "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
#define TABLE_END "</TABLE>"
#define TITLE_ROW "<TR><TD COLSPAN=\"2\"><B>%.*s</B></TD></TR>"
#define SUBTITLE_ROW "<TR><TD COLSPAN=\"2\">%.*s</TD></TR>"
#define KEY_STR_VALUE                                                          \
    "<TR><TD ALIGN=\"LEFT\">%s:</TD><TD ALIGN=\"LEFT\">%.*s</TD></TR>"
#define KEY_STRING_VALUE                                                       \
    "<TR><TD ALIGN=\"LEFT\">%s:</TD><TD ALIGN=\"LEFT\">%s</TD></TR>"


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
 * @param printer
 * @param node
 * @param nodeCount
 * @param format
 * @param ...
 */
static void ast_printer_print_node(ast_printer *printer, char *node,
                                   unsigned long long nodeCount, char *format,
                                   ...) {
    va_list args;

    fprintf(printer->outFile, "\t%s%llu [label=<", node, nodeCount);

    va_start(args, format);
    vfprintf(printer->outFile, format, args);
    va_end(args);

    fprintf(printer->outFile, ">];\n");
}

/**
 * @brief TODO
 * @param printer
 * @param node
 * @param nodeCount
 */
static void ast_printer_print_link_left(ast_printer *printer, char *node,
                                        unsigned long long nodeCount) {
    fprintf(printer->outFile, "\t%s%llu -> ", node, nodeCount);
}

/**
 * @brief TODO
 * @param printer
 * @param node
 * @param nodeCount
 */
static void ast_printer_print_link_right(ast_printer *printer, char *node,
                                         unsigned long long nodeCount) {
    fprintf(printer->outFile, "%s%llu;\n", node, nodeCount);
}

/**
 * @brief Print the Program AST node.
 * @param[in] programNode The Program AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_program_printer(ast_program *programNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount;
    list_node *node              = NULL;

    str title    = STR_STATIC_INIT("ast_program");
    str subtitle = STR_STATIC_INIT("classes");

    ast_printer_print_node(printer, "program", nodeCount,
                           TABLE_BEGIN TITLE_ROW TABLE_END, STR_FMT(&title));
    ast_printer_print_link_left(printer, "program", nodeCount);
    ast_printer_print_link_right(printer, "program_classes", nodeCount);
    ast_printer_print_node(printer, "program_classes", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitle));
    for (node = programNode->classes->head; node; node = node->next) {
        ast_printer_print_link_left(printer, "program_classes", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, (ast_class *) node->element,
                              args);
    }
}

/**
 * @brief Print the Class AST node.
 * @param[in] classNode The Class AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_class_printer(ast_class *classNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    str title                  = STR_STATIC_INIT("ast_class");
    str subtitleParameters     = STR_STATIC_INIT("parameters");
    str subtitleSuperClassArgs = STR_STATIC_INIT("superClassArgs");
    str subtitleProperties     = STR_STATIC_INIT("properties");
    str subtitleFunctions      = STR_STATIC_INIT("functions");

    str className      = STR_NULL_INIT;
    str superClassName = STR_NULL_INIT;

    if (classNode->name) { className = classNode->name->identifier; }
    if (classNode->superClass) {
        superClassName = classNode->superClass->identifier;
    }

    ast_printer_print_link_right(printer, "class", nodeCount);
    ast_printer_print_node(
            printer, "class", nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_STR_VALUE TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&className), "super",
            STR_FMT(&superClassName));

    ast_printer_print_link_left(printer, "class", nodeCount);
    ast_printer_print_link_right(printer, "class_parameters", nodeCount);
    ast_printer_print_node(printer, "class_parameters", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleParameters));
    if (classNode->parameters) {
        for (node = classNode->parameters->head; node; node = node->next) {
            ast_printer_print_link_left(printer, "class_parameters", nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_formal *) node->element, args);
        }
    }

    ast_printer_print_link_left(printer, "class", nodeCount);
    ast_printer_print_link_right(printer, "class_super_class_args", nodeCount);
    ast_printer_print_node(printer, "class_super_class_args", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleSuperClassArgs));
    if (classNode->superClassArgs) {
        for (node = classNode->superClassArgs->head; node; node = node->next) {
            ast_printer_print_link_left(printer, "class_super_class_args",
                                        nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_expression *) node->element, args);
        }
    }

    ast_printer_print_link_left(printer, "class", nodeCount);
    ast_printer_print_link_right(printer, "class_properties", nodeCount);
    ast_printer_print_node(printer, "class_properties", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleProperties));
    if (classNode->properties) {
        for (node = classNode->properties->head; node; node = node->next) {
            ast_printer_print_link_left(printer, "class_properties", nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_property *) node->element, args);
        }
    }

    ast_printer_print_link_left(printer, "class", nodeCount);
    ast_printer_print_link_right(printer, "class_functions", nodeCount);
    ast_printer_print_node(printer, "class_functions", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleFunctions));
    if (classNode->functions) {
        for (node = classNode->functions->head; node; node = node->next) {
            ast_printer_print_link_left(printer, "class_functions", nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_function *) node->element, args);
        }
    }
}

/**
 * @brief Print the Formal AST node.
 * @param[in] formalNode The Formal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_formal_printer(ast_formal *formalNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("ast_formal");

    str identifier = STR_NULL_INIT;
    str type       = STR_NULL_INIT;

    if (formalNode->identifier) {
        identifier = formalNode->identifier->identifier;
    }
    if (formalNode->type) { type = formalNode->type->identifier; }

    ast_printer_print_link_right(printer, "formal", nodeCount);
    ast_printer_print_node(printer, "formal", nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_STR_VALUE
                                   KEY_STRING_VALUE TABLE_END,
                           STR_FMT(&title), "identifier", STR_FMT(&identifier),
                           "type", STR_FMT(&type), "isLazy",
                           (formalNode->isLazy ? "true" : "false"));
}

/**
 * @brief Print the Property AST node.
 * @param[in] propertyNode The Property AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_property_printer(ast_property *propertyNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("ast_property");

    str name = STR_NULL_INIT;
    str type = STR_NULL_INIT;

    if (propertyNode->name) { name = propertyNode->name->identifier; }
    if (propertyNode->type) { type = propertyNode->type->identifier; }

    ast_printer_print_link_right(printer, "property", nodeCount);
    ast_printer_print_node(
            printer, "property", nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_STR_VALUE TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&name), "type", STR_FMT(&type));
    if (propertyNode->value) {
        ast_printer_print_link_left(printer, "property", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, propertyNode->value, args);
    }
}

/**
 * @brief Print the Function AST node.
 * @param[in] functionNode The Function AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_function_printer(ast_function *functionNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    str title              = STR_STATIC_INIT("ast_function");
    str subtitleParameters = STR_STATIC_INIT("parameters");
    str subtitleBody       = STR_STATIC_INIT("body");

    str name       = STR_NULL_INIT;
    str returnType = STR_NULL_INIT;
    str visibility = STR_NULL_INIT;

    if (functionNode->functionName) {
        name = functionNode->functionName->identifier;
    }
    if (functionNode->returnType) {
        returnType = functionNode->returnType->identifier;
    }

    switch (functionNode->visibility) {
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

    ast_printer_print_link_right(printer, "function", nodeCount);
    ast_printer_print_node(
            printer, "function", nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_STR_VALUE KEY_STR_VALUE
                    KEY_STRING_VALUE KEY_STRING_VALUE KEY_STRING_VALUE
                            TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&name), "returnType",
            STR_FMT(&returnType), "visibility", STR_FMT(&visibility),
            "isAbstract", (functionNode->isAbstract ? "true" : "false"),
            "isFinal", (functionNode->isFinal ? "true" : "false"),
            "isOverwrite", (functionNode->isOverwrite ? "true" : "false"));

    ast_printer_print_link_left(printer, "function", nodeCount);
    ast_printer_print_link_right(printer, "function_parameters", nodeCount);
    ast_printer_print_node(printer, "function_parameters", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleParameters));
    if (functionNode->parameters) {
        for (node = functionNode->parameters->head; node; node = node->next) {
            ast_printer_print_link_right(printer, "function_parameters",
                                         nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_formal *) node->element, args);
        }
    }

    ast_printer_print_link_left(printer, "function", nodeCount);
    ast_printer_print_link_right(printer, "function_body", nodeCount);
    ast_printer_print_node(printer, "function_body", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleBody));
    if (functionNode->body) {
        ast_printer_print_link_left(printer, "function_body", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, functionNode->body, args);
    }
}

/**
 * @brief Print the Assignment AST node.
 * @param[in] assignmentNode The Assignment AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_assignment_printer(ast_assignment *assignmentNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("ast_assignment");

    str identifier = STR_NULL_INIT;
    str operator   = STR_NULL_INIT;

    if (assignmentNode->identifier) {
        identifier = assignmentNode->identifier->identifier;
    }

    switch (assignmentNode->operator) {
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

    ast_printer_print_link_right(printer, "assignment", nodeCount);
    ast_printer_print_node(
            printer, "assignment", nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_STR_VALUE TABLE_END,
            STR_FMT(&title), "identifier", STR_FMT(&identifier), "operator",
            STR_FMT(&operator));
    if (assignmentNode->value) {
        ast_printer_print_link_left(printer, "assignment", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, assignmentNode->value, args);
    }
}

/**
 * @brief Print the String Literal AST node.
 * @param[in] stringLiteralNode The String Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_string_literal_printer(ast_string_literal *stringLiteralNode,
                                       void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("ast_string_literal");

    ast_printer_print_link_right(printer, "string_literal", nodeCount);
    ast_printer_print_node(printer, "string_literal", nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STR_VALUE TABLE_END,
                           STR_FMT(&title), "value",
                           STR_FMT(&stringLiteralNode->value));
}

/**
 * @brief Print the Integer Literal AST node.
 * @param[in] integerLiteralNode The Integer Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_integer_literal_printer(ast_integer_literal *integerLiteralNode,
                                        void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("ast_integer_literal");

    ast_printer_print_link_right(printer, "integer_literal", nodeCount);
    ast_printer_print_node(printer, "integer_literal", nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STR_VALUE TABLE_END,
                           STR_FMT(&title), "value",
                           STR_FMT(&integerLiteralNode->value));
}

/**
 * @brief Print the Null Literal AST node.
 * @param[in] nullLiteralNode The Null Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_null_literal_printer(ast_null_literal *nullLiteralNode,
                                     void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    (void) nullLiteralNode;

    str title = STR_STATIC_INIT("ast_null_literal");

    ast_printer_print_link_right(printer, "null_literal", nodeCount);
    ast_printer_print_node(printer, "null_literal", nodeCount,
                           TABLE_BEGIN TITLE_ROW TABLE_END, STR_FMT(&title));
}

/**
 * @brief Print the Boolean Literal AST node.
 * @param[in] booleanLiteralNode The Boolean Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_boolean_literal_printer(ast_boolean_literal *booleanLiteralNode,
                                        void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("ast_boolean_literal");

    ast_printer_print_link_right(printer, "boolean_literal", nodeCount);
    ast_printer_print_node(printer, "boolean_literal", nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STRING_VALUE TABLE_END,
                           STR_FMT(&title), "value",
                           (booleanLiteralNode->value ? "true" : "false"));
}

/**
 * @brief Print the This Literal AST node.
 * @param[in] thisLiteralNode The This Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_this_literal_printer(ast_this_literal *thisLiteralNode,
                                     void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    (void) thisLiteralNode;

    str title = STR_STATIC_INIT("ast_this_literal");

    ast_printer_print_link_right(printer, "this_literal", nodeCount);
    ast_printer_print_node(printer, "this_literal", nodeCount,
                           TABLE_BEGIN TITLE_ROW TABLE_END, STR_FMT(&title));
}

/**
 * @brief Print the Reference Literal AST node.
 * @param[in] referenceNode The Reference Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_reference_printer(ast_reference *referenceNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("ast_reference");

    str identifier = STR_NULL_INIT;

    if (referenceNode->value) { identifier = referenceNode->value->identifier; }

    ast_printer_print_link_right(printer, "reference", nodeCount);
    ast_printer_print_node(printer, "reference", nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STR_VALUE TABLE_END,
                           STR_FMT(&title), "value", STR_FMT(&identifier));
}

/**
 * @brief Print the Cast AST node.
 * @param[in] castNode The Cast AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_cast_printer(ast_cast *castNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("ast_cast");

    str type = STR_NULL_INIT;

    if (castNode->type) { type = castNode->type->identifier; }

    ast_printer_print_link_right(printer, "cast", nodeCount);
    ast_printer_print_node(printer, "cast", nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STR_VALUE TABLE_END,
                           STR_FMT(&title), "type", STR_FMT(&type));
    if (castNode->object) {
        ast_printer_print_link_left(printer, "cast", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, castNode->object, args);
    }
}

/**
 * @brief Print the Block AST node.
 * @param[in] blockNode The Block AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_block_printer(ast_block *blockNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    str title              = STR_STATIC_INIT("ast_block");
    str subtitleParameters = STR_STATIC_INIT("expressions");

    ast_printer_print_link_right(printer, "block", nodeCount);
    ast_printer_print_node(printer, "block", nodeCount,
                           TABLE_BEGIN TITLE_ROW TABLE_END, STR_FMT(&title));

    ast_printer_print_link_left(printer, "block", nodeCount);
    ast_printer_print_link_right(printer, "block_expressions", nodeCount);
    ast_printer_print_node(printer, "block_expressions", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleParameters));
    if (blockNode->expressions) {
        for (node = blockNode->expressions->head; node; node = node->next) {
            ast_printer_print_link_left(printer, "block_expressions",
                                        nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_formal *) node->element, args);
        }
    }
}

/**
 * @brief Print the Constructor Call AST node.
 * @param[in] constructorCallNode The Constructor Call AST node to print
 * @param[in] args The extra args for the callback call
 */
static void
ast_constructor_call_printer(ast_constructor_call *constructorCallNode,
                             void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    str title        = STR_STATIC_INIT("ast_constructor_call");
    str subtitleArgs = STR_STATIC_INIT("args");

    str name = STR_NULL_INIT;

    if (constructorCallNode->name) {
        name = constructorCallNode->name->identifier;
    }

    ast_printer_print_link_right(printer, "constructor_call", nodeCount);
    ast_printer_print_node(printer, "constructor_call", nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STR_VALUE TABLE_END,
                           STR_FMT(&title), "name", STR_FMT(&name));

    ast_printer_print_link_left(printer, "constructor_call", nodeCount);
    ast_printer_print_link_right(printer, "constructor_call_args", nodeCount);
    ast_printer_print_node(printer, "constructor_call_args", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleArgs));
    if (constructorCallNode->args) {
        for (node = constructorCallNode->args->head; node; node = node->next) {
            ast_printer_print_link_left(printer, "constructor_call_args",
                                        nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_expression *) node->element, args);
        }
    }
}

/**
 * @brief Print the Let AST node.
 * @param[in] letNode The Let AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_let_printer(ast_let *letNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    str title                   = STR_STATIC_INIT("ast_let");
    str subtitleInitializations = STR_STATIC_INIT("initializations");
    str subtitleBody            = STR_STATIC_INIT("body");

    ast_printer_print_link_right(printer, "let", nodeCount);
    ast_printer_print_node(printer, "let", nodeCount,
                           TABLE_BEGIN TITLE_ROW TABLE_END, STR_FMT(&title));

    ast_printer_print_link_left(printer, "let", nodeCount);
    ast_printer_print_link_right(printer, "let_initializations", nodeCount);
    ast_printer_print_node(printer, "let_initializations", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleInitializations));
    if (letNode->initializations) {
        for (node = letNode->initializations->head; node; node = node->next) {
            ast_printer_print_link_left(printer, "let_initializations",
                                        nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_expression *) node->element, args);
        }
    }

    ast_printer_print_link_left(printer, "let", nodeCount);
    ast_printer_print_link_right(printer, "let_body", nodeCount);
    ast_printer_print_node(printer, "let_body", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleBody));
    if (letNode->body) {
        ast_printer_print_link_left(printer, "let_body", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, letNode->body, args);
    }
}

/**
 * @brief Print the Initialization AST node.
 * @param[in] initializationNode The Initialization AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_initialization_printer(ast_initialization *initializationNode,
                                       void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("ast_initialization");

    str identifier = STR_NULL_INIT;
    str type       = STR_NULL_INIT;

    if (initializationNode->identifier) {
        identifier = initializationNode->identifier->identifier;
    }
    if (initializationNode->type) {
        type = initializationNode->type->identifier;
    }

    ast_printer_print_link_right(printer, "initialization", nodeCount);
    ast_printer_print_node(
            printer, "initialization", nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_STR_VALUE TABLE_END,
            STR_FMT(&title), "identifier", STR_FMT(&identifier), "type",
            STR_FMT(&type));

    if (initializationNode->value) {
        ast_printer_print_link_left(printer, "initialization", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, initializationNode->value,
                              args);
    }
}

/**
 * @brief Print the Binary Expression AST node.
 * @param[in] binaryExpressionNode The Binary Expression AST node to print
 * @param[in] args The extra args for the callback call
 */
static void
ast_binary_expression_printer(ast_binary_expression *binaryExpressionNode,
                              void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title         = STR_STATIC_INIT("ast_binary_expression");
    str subtitleLeft  = STR_STATIC_INIT("left");
    str subtitleRight = STR_STATIC_INIT("right");

    str operator= STR_NULL_INIT;

    switch (binaryExpressionNode->operator) {
        case AST_BINARY_OPERATOR_PLUS:
            STR_STATIC_SET(&operator, "+");
            break;
        case AST_BINARY_OPERATOR_MINUS:
            STR_STATIC_SET(&operator, "-");
            break;
        case AST_BINARY_OPERATOR_TIMES:
            STR_STATIC_SET(&operator, "*");
            break;
        case AST_BINARY_OPERATOR_DIV:
            STR_STATIC_SET(&operator, "/");
            break;
        case AST_BINARY_OPERATOR_MODULO:
            STR_STATIC_SET(&operator, "%");
            break;
        case AST_BINARY_OPERATOR_AND:
            STR_STATIC_SET(&operator, "&");
            break;
        case AST_BINARY_OPERATOR_CARET:
            STR_STATIC_SET(&operator, "^");
            break;
        case AST_BINARY_OPERATOR_TILDE:
            STR_STATIC_SET(&operator, "~");
            break;
        case AST_BINARY_OPERATOR_PIPE:
            STR_STATIC_SET(&operator, "|");
            break;
        case AST_BINARY_OPERATOR_LESS:
            STR_STATIC_SET(&operator, "<");
            break;
        case AST_BINARY_OPERATOR_LESS_EQUAL:
            STR_STATIC_SET(&operator, "<=");
            break;
        case AST_BINARY_OPERATOR_GREATER:
            STR_STATIC_SET(&operator, ">");
            break;
        case AST_BINARY_OPERATOR_GREATER_EQUAL:
            STR_STATIC_SET(&operator, ">=");
            break;
        case AST_BINARY_OPERATOR_EQUAL:
            STR_STATIC_SET(&operator, "==");
            break;
        case AST_BINARY_OPERATOR_NOT_EQUAL:
            STR_STATIC_SET(&operator, "!=");
            break;
        case AST_BINARY_OPERATOR_DOUBLE_AND:
            STR_STATIC_SET(&operator, "&&");
            break;
        case AST_BINARY_OPERATOR_DOUBLE_PIPE:
            STR_STATIC_SET(&operator, "||");
            break;
    }

    ast_printer_print_link_right(printer, "binary_expression", nodeCount);
    ast_printer_print_node(printer, "binary_expression", nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STR_VALUE TABLE_END,
                           STR_FMT(&title), "operator", STR_FMT(&operator));

    ast_printer_print_link_left(printer, "binary_expression", nodeCount);
    ast_printer_print_link_right(printer, "binary_expression_left", nodeCount);
    ast_printer_print_node(printer, "binary_expression_left", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleLeft));
    if (binaryExpressionNode->left) {
        ast_printer_print_link_left(printer, "binary_expression_left",
                                    nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, binaryExpressionNode->left,
                              args);
    }

    ast_printer_print_link_left(printer, "binary_expression", nodeCount);
    ast_printer_print_link_right(printer, "binary_expression_right", nodeCount);
    ast_printer_print_node(printer, "binary_expression_right", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleRight));
    if (binaryExpressionNode->right) {
        ast_printer_print_link_left(printer, "binary_expression_right",
                                    nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, binaryExpressionNode->right,
                              args);
    }
}

/**
 * @brief Print the Unary Expression AST node.
 * @param[in] unaryExpressionNode The Unary Expression AST node to print
 * @param[in] args The extra args for the callback call
 */
static void
ast_unary_expression_printer(ast_unary_expression *unaryExpressionNode,
                             void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title              = STR_STATIC_INIT("ast_unary_expression");
    str subtitleExpression = STR_STATIC_INIT("unary_expression_expression");

    str operator= STR_NULL_INIT;

    switch (unaryExpressionNode->operator) {
        case AST_UNARY_OPERATOR_MINUS:
            STR_STATIC_SET(&operator, "-");
            break;
        case AST_UNARY_OPERATOR_NOT:
            STR_STATIC_SET(&operator, "!");
            break;
        case AST_UNARY_OPERATOR_DOUBLE_PLUS:
            STR_STATIC_SET(&operator, "++");
            break;
        case AST_UNARY_OPERATOR_DOUBLE_MINUS:
            STR_STATIC_SET(&operator, "--");
            break;
    }

    ast_printer_print_link_right(printer, "unary_expression", nodeCount);
    ast_printer_print_node(printer, "unary_expression", nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STR_VALUE TABLE_END,
                           STR_FMT(&title), "operator", STR_FMT(&operator));

    ast_printer_print_link_left(printer, "unary_expression", nodeCount);
    ast_printer_print_link_right(printer, "unary_expression_expression",
                                 nodeCount);
    ast_printer_print_node(printer, "unary_expression_expression", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleExpression));
    if (unaryExpressionNode->expression) {
        ast_printer_print_link_left(printer, "unary_expression_expression",
                                    nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks,
                              unaryExpressionNode->expression, args);
    }
}

/**
 * @brief Print the If Else AST node.
 * @param[in] ifElseNode The If Else AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_if_else_printer(ast_if_else *ifElseNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title              = STR_STATIC_INIT("ast_if_else");
    str subtitleCondition  = STR_STATIC_INIT("condition");
    str subtitleThenBranch = STR_STATIC_INIT("thenBranch");
    str subtitleElseBranch = STR_STATIC_INIT("elseBranch");

    ast_printer_print_link_right(printer, "if_else", nodeCount);
    ast_printer_print_node(printer, "if_else", nodeCount,
                           TABLE_BEGIN TITLE_ROW TABLE_END, STR_FMT(&title));

    ast_printer_print_link_left(printer, "if_else", nodeCount);
    ast_printer_print_link_right(printer, "if_else_condition", nodeCount);
    ast_printer_print_node(printer, "if_else_condition", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleCondition));
    if (ifElseNode->condition) {
        ast_printer_print_link_left(printer, "if_else_condition", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, ifElseNode->condition, args);
    }

    ast_printer_print_link_left(printer, "if_else", nodeCount);
    ast_printer_print_link_right(printer, "if_else_then", nodeCount);
    ast_printer_print_node(printer, "if_else_then", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleThenBranch));
    if (ifElseNode->thenBranch) {
        ast_printer_print_link_left(printer, "if_else_then", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, ifElseNode->thenBranch, args);
    }

    if (ifElseNode->elseBranch) {
        ast_printer_print_link_left(printer, "if_else", nodeCount);
        ast_printer_print_link_right(printer, "if_else_else", nodeCount);
        ast_printer_print_node(printer, "if_else_else", nodeCount,
                               TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                               STR_FMT(&subtitleElseBranch));
        if (ifElseNode->thenBranch) {
            ast_printer_print_link_left(printer, "if_else_else", nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks, ifElseNode->elseBranch,
                                  args);
        }
    }
}

/**
 * @brief Print the While AST node.
 * @param[in] whileNode The While AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_while_printer(ast_while *whileNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title             = STR_STATIC_INIT("ast_while");
    str subtitleCondition = STR_STATIC_INIT("condition");
    str subtitleBody      = STR_STATIC_INIT("body");

    ast_printer_print_link_right(printer, "while", nodeCount);
    ast_printer_print_node(printer, "while", nodeCount,
                           TABLE_BEGIN TITLE_ROW TABLE_END, STR_FMT(&title));

    ast_printer_print_link_left(printer, "while", nodeCount);
    ast_printer_print_link_right(printer, "while_condition", nodeCount);
    ast_printer_print_node(printer, "while_condition", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleCondition));
    if (whileNode->condition) {
        ast_printer_print_link_left(printer, "while_condition", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, whileNode->condition, args);
    }

    ast_printer_print_link_left(printer, "while", nodeCount);
    ast_printer_print_link_right(printer, "while_body", nodeCount);
    ast_printer_print_node(printer, "while_body", nodeCount,
                           TABLE_BEGIN SUBTITLE_ROW TABLE_END,
                           STR_FMT(&subtitleBody));
    if (whileNode->body) {
        ast_printer_print_link_left(printer, "while_body", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, whileNode->body, args);
    }
}

/**
 * @brief Print the AST node.
 * @param[in] node The AST node to print
 * @param[in] args The extra args for the callback call
 */
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
                    ast_this_literal_printer((ast_this_literal *) node, args);
                    break;
                case AST_EXPRESSION_TYPE_ASSIGNMENT:
                    ast_assignment_printer((ast_assignment *) node, args);
                    break;
                case AST_EXPRESSION_TYPE_REFERENCE:
                    ast_reference_printer((ast_reference *) node, args);
                    break;
                case AST_EXPRESSION_TYPE_CAST:
                    ast_cast_printer((ast_cast *) node, args);
                    break;
                case AST_EXPRESSION_TYPE_INITIALIZATION:
                    ast_initialization_printer((ast_initialization *) node,
                                               args);
                    break;
                case AST_EXPRESSION_TYPE_LET:
                    ast_let_printer((ast_let *) node, args);
                    break;
                case AST_EXPRESSION_TYPE_BLOCK:
                    ast_block_printer((ast_block *) node, args);
                    break;
                case AST_EXPRESSION_TYPE_CONSTRUCTOR_CALL:
                    ast_constructor_call_printer((ast_constructor_call *) node,
                                                 args);
                    break;
                case AST_EXPRESSION_TYPE_FUNCTION_CALL:
                    break;
                case AST_EXPRESSION_TYPE_SUPER_FUNCTION_CALL:
                    break;
                case AST_EXPRESSION_TYPE_BINARY_EXPRESSION:
                    ast_binary_expression_printer(
                            (ast_binary_expression *) node, args);
                    break;
                case AST_EXPRESSION_TYPE_UNARY_EXPRESSION:
                    ast_unary_expression_printer((ast_unary_expression *) node,
                                                 args);
                    break;
                case AST_EXPRESSION_TYPE_IF_ELSE:
                    ast_if_else_printer((ast_if_else *) node, args);
                    break;
                case AST_EXPRESSION_TYPE_WHILE:
                    ast_while_printer((ast_while *) node, args);
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
                    ast_property_printer((ast_property *) node, args);
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

    fprintf(printer.outFile, "digraph AST {\n");
    fprintf(printer.outFile, "\tnode [shape=plain]\n");

    ast_walk((ast *) this, &callbacks, &printer);

    fprintf(printer.outFile, "}\n");
}
