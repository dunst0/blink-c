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
#define KEY_STR_VALUE                                                          \
    "<TR><TD ALIGN=\"LEFT\">%s:</TD><TD ALIGN=\"LEFT\">%.*s</TD></TR>"
#define KEY_STRING_VALUE                                                       \
    "<TR><TD ALIGN=\"LEFT\">%s:</TD><TD ALIGN=\"LEFT\">%s</TD></TR>"
#define KEY_PORT                                                               \
    "<TR><TD ALIGN=\"LEFT\" COLSPAN=\"2\" PORT=\"%s%llu\">%s</TD></TR>"


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
 * @brief Convert the boolean value to a const string.
 * @param[in] value The boolean value
 * @return The const string
 */
static inline const char *ast_printer_bool_string(bool value) {
    return value ? "true" : "false";
}

/**
 * @brief Convert the assignment operator value to a const string.
 * @param[in] operator The assignment operator value
 * @return The const string
 */
static inline const char *
ast_printer_assignment_operator(ast_assignment_operator operator) {
    switch (operator) {
        case AST_ASSIGNMENT_OPERATOR_EQUAL:
            return "=";
        case AST_ASSIGNMENT_OPERATOR_PLUS_EQUAL:
            return "+=";
        case AST_ASSIGNMENT_OPERATOR_MINUS_EQUAL:
            return "-=";
        case AST_ASSIGNMENT_OPERATOR_TIMES_EQUAL:
            return "*=";
        case AST_ASSIGNMENT_OPERATOR_DIV_EQUAL:
            return "/=";
        case AST_ASSIGNMENT_OPERATOR_MODULO_EQUAL:
            return "%=";
        case AST_ASSIGNMENT_OPERATOR_AND_EQUAL:
            return "&=";
        case AST_ASSIGNMENT_OPERATOR_CARET_EQUAL:
            return "^=";
        case AST_ASSIGNMENT_OPERATOR_TILDE_EQUAL:
            return "~=";
        case AST_ASSIGNMENT_OPERATOR_PIPE_EQUAL:
            return "|=";
        default:
            return "";
    }
}

/**
 * @brief Convert the unary operator value to a const string.
 * @param[in] operator The unary operator value
 * @return The const string
 */
static inline const char *
ast_printer_unary_operator_string(ast_unary_operator operator) {
    switch (operator) {
        case AST_UNARY_OPERATOR_MINUS:
            return "-";
        case AST_UNARY_OPERATOR_NOT:
            return "!";
        case AST_UNARY_OPERATOR_DOUBLE_PLUS:
            return "++";
        case AST_UNARY_OPERATOR_DOUBLE_MINUS:
            return "--";
        default:
            return "";
    }
}

/**
 * @brief Convert the binary operator value to a const string.
 * @param[in] operator The binary operator value
 * @return The const string
 */
static inline const char *
ast_printer_binary_operator_string(ast_binary_operator operator) {
    switch (operator) {
        case AST_BINARY_OPERATOR_PLUS:
            return "+";
        case AST_BINARY_OPERATOR_MINUS:
            return "-";
        case AST_BINARY_OPERATOR_TIMES:
            return "*";
        case AST_BINARY_OPERATOR_DIV:
            return "/";
        case AST_BINARY_OPERATOR_MODULO:
            return "%";
        case AST_BINARY_OPERATOR_AND:
            return "&";
        case AST_BINARY_OPERATOR_CARET:
            return "^";
        case AST_BINARY_OPERATOR_TILDE:
            return "~";
        case AST_BINARY_OPERATOR_PIPE:
            return "|";
        case AST_BINARY_OPERATOR_LESS:
            return "<";
        case AST_BINARY_OPERATOR_LESS_EQUAL:
            return "<=";
        case AST_BINARY_OPERATOR_GREATER:
            return ">";
        case AST_BINARY_OPERATOR_GREATER_EQUAL:
            return ">=";
        case AST_BINARY_OPERATOR_EQUAL:
            return "==";
        case AST_BINARY_OPERATOR_NOT_EQUAL:
            return "!=";
        case AST_BINARY_OPERATOR_DOUBLE_AND:
            return "&&";
        case AST_BINARY_OPERATOR_DOUBLE_PIPE:
            return "||";
        default:
            return "";
    }
}

/**
 * @brief Convert the function visibility value to a const string.
 * @param[in] visibility The function visibility value
 * @return The const string
 */
static inline const char *
ast_printer_function_visibility_string(ast_function_visibility visibility) {
    switch (visibility) {
        case AST_FUNCTION_VISIBILITY_PUBLIC:
            return "public";
        case AST_FUNCTION_VISIBILITY_PROTECTED:
            return "protected";
        case AST_FUNCTION_VISIBILITY_PRIVATE:
            return "private";
        default:
            return "";
    }
}

/**
 * @brief Print a graph node.
 * @param[in] printer The printer to print to
 * @param[in] node The node str to use
 * @param[in] nodeCount The node count to use
 * @param[in] format The format to use
 * @param[in] ... The arguments to fill in the format
 */
static inline void ast_printer_print_node(ast_printer *printer, str *node,
                                          unsigned long long nodeCount,
                                          char *format, ...) {
    va_list args;

    fprintf(printer->outFile, "\t%.*s%llu [label=<", STR_FMT(node), nodeCount);

    va_start(args, format);
    vfprintf(printer->outFile, format, args);
    va_end(args);

    fprintf(printer->outFile, ">];\n");
}

/**
 * @brief Print a graph node start edge.
 * @param[in] printer The printer to print to
 * @param[in] node The node str to use
 * @param[in] port The port to use
 * @param[in] nodeCount The node count to use
 */
static inline void ast_printer_print_link_left(ast_printer *printer, str *node,
                                               char *port,
                                               unsigned long long nodeCount) {
    fprintf(printer->outFile, "\t%.*s%llu:%s%llu -> ", STR_FMT(node), nodeCount,
            port, nodeCount);
}

/**
 * @brief Print a graph node end edge.
 * @param[in] printer The printer to print to
 * @param[in] node The node str to use
 * @param[in] nodeCount The node count to use
 */
static inline void ast_printer_print_link_right(ast_printer *printer, str *node,
                                                unsigned long long nodeCount) {
    fprintf(printer->outFile, "%.*s%llu;\n", STR_FMT(node), nodeCount);
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

    str title = STR_STATIC_INIT("ast_program");

    ast_printer_print_node(printer, &title, nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_PORT TABLE_END,
                           STR_FMT(&title), "program_namespaces", nodeCount,
                           "namespaces");
    for (node = programNode->namespaces->head; node; node = node->next) {
        ast_printer_print_link_left(printer, &title, "program_namespaces",
                                    nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks,
                              (ast_namespace *) node->element, args);
    }
}

/**
 * @brief Print the Namespace AST node.
 * @param[in] namespaceNode The Namespace AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_namespace_printer(ast_namespace *namespaceNode, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount;
    list_node *node              = NULL;

    str title = STR_STATIC_INIT("ast_namespace");

    str name = STR_NULL_INIT;

    if (namespaceNode->name) { name = namespaceNode->name->identifier; }

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(
            printer, &title, nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_PORT TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&name), "namespaces_classes",
            nodeCount, "classes");
    for (node = namespaceNode->classes->head; node; node = node->next) {
        ast_printer_print_link_left(printer, &title, "namespaces_classes",
                                    nodeCount);
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

    str title = STR_STATIC_INIT("ast_class");

    str className      = STR_NULL_INIT;
    str superClassName = STR_NULL_INIT;

    if (classNode->name) { className = classNode->name->identifier; }
    if (classNode->superClass) {
        superClassName = classNode->superClass->identifier;
    }

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(
            printer, &title, nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_PORT KEY_STR_VALUE KEY_PORT
                    KEY_PORT KEY_PORT TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&className), "class_parameters",
            nodeCount, "parameters", "super", STR_FMT(&superClassName),
            "class_super_class_args", nodeCount, "superClassArgs",
            "class_properties", nodeCount, "properties", "class_functions",
            nodeCount, "functions");

    if (classNode->parameters) {
        for (node = classNode->parameters->head; node; node = node->next) {
            ast_printer_print_link_left(printer, &title, "class_parameters",
                                        nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_formal *) node->element, args);
        }
    }

    if (classNode->superClassArgs) {
        for (node = classNode->superClassArgs->head; node; node = node->next) {
            ast_printer_print_link_left(printer, &title,
                                        "class_super_class_args", nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_expression *) node->element, args);
        }
    }

    if (classNode->properties) {
        for (node = classNode->properties->head; node; node = node->next) {
            ast_printer_print_link_left(printer, &title, "class_properties",
                                        nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_property *) node->element, args);
        }
    }

    if (classNode->functions) {
        for (node = classNode->functions->head; node; node = node->next) {
            ast_printer_print_link_left(printer, &title, "class_functions",
                                        nodeCount);
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

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_STR_VALUE
                                   KEY_STRING_VALUE TABLE_END,
                           STR_FMT(&title), "identifier", STR_FMT(&identifier),
                           "type", STR_FMT(&type), "isLazy",
                           ast_printer_bool_string(formalNode->isLazy));
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

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_STR_VALUE
                                   KEY_PORT TABLE_END,
                           STR_FMT(&title), "name", STR_FMT(&name), "type",
                           STR_FMT(&type), "property_value", nodeCount,
                           "value");
    if (propertyNode->value) {
        ast_printer_print_link_left(printer, &title, "property_value",
                                    nodeCount);
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

    str title = STR_STATIC_INIT("ast_function");

    str name       = STR_NULL_INIT;
    str returnType = STR_NULL_INIT;

    if (functionNode->functionName) {
        name = functionNode->functionName->identifier;
    }
    if (functionNode->returnType) {
        returnType = functionNode->returnType->identifier;
    }

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(
            printer, &title, nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_PORT KEY_STR_VALUE
                    KEY_STRING_VALUE KEY_STRING_VALUE KEY_STRING_VALUE
                            KEY_STRING_VALUE KEY_PORT TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&name), "function_parameters",
            nodeCount, "parameters", "returnType", STR_FMT(&returnType),
            "visibility",
            ast_printer_function_visibility_string(functionNode->visibility),
            "isAbstract", ast_printer_bool_string(functionNode->isAbstract),
            "isFinal", ast_printer_bool_string(functionNode->isFinal),
            "isOverwrite", ast_printer_bool_string(functionNode->isOverwrite),
            "function_body", nodeCount, "body");

    if (functionNode->parameters) {
        for (node = functionNode->parameters->head; node; node = node->next) {
            ast_printer_print_link_left(printer, &title, "function_parameters",
                                        nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_formal *) node->element, args);
        }
    }

    if (functionNode->body) {
        ast_printer_print_link_left(printer, &title, "function_body",
                                    nodeCount);
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

    if (assignmentNode->identifier) {
        identifier = assignmentNode->identifier->identifier;
    }

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(
            printer, &title, nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_STRING_VALUE KEY_PORT
                    TABLE_END,
            STR_FMT(&title), "identifier", STR_FMT(&identifier), "operator",
            ast_printer_assignment_operator(assignmentNode->operator),
            "assignment_value", nodeCount, "value");
    if (assignmentNode->value) {
        ast_printer_print_link_left(printer, &title, "assignment_value",
                                    nodeCount);
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

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
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

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
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

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
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

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STRING_VALUE TABLE_END,
                           STR_FMT(&title), "value",
                           ast_printer_bool_string(booleanLiteralNode->value));
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

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
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

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
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

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(
            printer, &title, nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_PORT KEY_STR_VALUE TABLE_END,
            STR_FMT(&title), "cast_object", nodeCount, "object", "type",
            STR_FMT(&type));
    if (castNode->object) {
        ast_printer_print_link_left(printer, &title, "cast_object", nodeCount);
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

    str title = STR_STATIC_INIT("ast_block");

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_PORT TABLE_END,
                           STR_FMT(&title), "block_expressions", nodeCount,
                           "expressions");
    if (blockNode->expressions) {
        for (node = blockNode->expressions->head; node; node = node->next) {
            ast_printer_print_link_left(printer, &title, "block_expressions",
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

    str title = STR_STATIC_INIT("ast_constructor_call");

    str name = STR_NULL_INIT;

    if (constructorCallNode->name) {
        name = constructorCallNode->name->identifier;
    }

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(
            printer, &title, nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_PORT TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&name), "constructor_call_args",
            nodeCount, "args");

    if (constructorCallNode->args) {
        for (node = constructorCallNode->args->head; node; node = node->next) {
            ast_printer_print_link_left(printer, &title,
                                        "constructor_call_args", nodeCount);
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

    str title = STR_STATIC_INIT("ast_let");

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_PORT KEY_PORT TABLE_END,
                           STR_FMT(&title), "let_initializations", nodeCount,
                           "initializations", "let_body", nodeCount, "body");

    if (letNode->initializations) {
        for (node = letNode->initializations->head; node; node = node->next) {
            ast_printer_print_link_left(printer, &title, "let_initializations",
                                        nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_expression *) node->element, args);
        }
    }

    if (letNode->body) {
        ast_printer_print_link_left(printer, &title, "let_body", nodeCount);
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

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_STR_VALUE KEY_STR_VALUE
                                   KEY_PORT TABLE_END,
                           STR_FMT(&title), "identifier", STR_FMT(&identifier),
                           "type", STR_FMT(&type), "initialization_value",
                           nodeCount, "value");
    if (initializationNode->value) {
        ast_printer_print_link_left(printer, &title, "initialization_value",
                                    nodeCount);
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

    str title = STR_STATIC_INIT("ast_binary_expression");

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(
            printer, &title, nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_PORT KEY_STRING_VALUE KEY_PORT TABLE_END,
            STR_FMT(&title), "binary_expression_left", nodeCount, "left",
            "operator",
            ast_printer_binary_operator_string(binaryExpressionNode->operator),
            "binary_expression_right", nodeCount, "right");

    if (binaryExpressionNode->left) {
        ast_printer_print_link_left(printer, &title, "binary_expression_left",
                                    nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, binaryExpressionNode->left,
                              args);
    }

    if (binaryExpressionNode->right) {
        ast_printer_print_link_left(printer, &title, "binary_expression_right",
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

    str title = STR_STATIC_INIT("ast_unary_expression");

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(
            printer, &title, nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_STRING_VALUE KEY_PORT TABLE_END,
            STR_FMT(&title), "operator",
            ast_printer_unary_operator_string(unaryExpressionNode->operator),
            "unary_expression_expression", nodeCount, "expression");

    if (unaryExpressionNode->expression) {
        ast_printer_print_link_left(printer, &title,
                                    "unary_expression_expression", nodeCount);
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

    str title = STR_STATIC_INIT("ast_if_else");

    ast_printer_print_link_right(printer, &title, nodeCount);
    if (!ifElseNode->elseBranch) {
        ast_printer_print_node(
                printer, &title, nodeCount,
                TABLE_BEGIN TITLE_ROW KEY_PORT KEY_PORT TABLE_END,
                STR_FMT(&title), "if_else_condition", nodeCount, "condition",
                "if_else_then_branch", nodeCount, "thenBranch");
    } else {
        ast_printer_print_node(
                printer, &title, nodeCount,
                TABLE_BEGIN TITLE_ROW KEY_PORT KEY_PORT KEY_PORT TABLE_END,
                STR_FMT(&title), "if_else_condition", nodeCount, "condition",
                "if_else_then_branch", nodeCount, "thenBranch",
                "if_else_else_branch", nodeCount, "elseBranch");
    }

    if (ifElseNode->condition) {
        ast_printer_print_link_left(printer, &title, "if_else_condition",
                                    nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, ifElseNode->condition, args);
    }

    if (ifElseNode->thenBranch) {
        ast_printer_print_link_left(printer, &title, "if_else_then_branch",
                                    nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, ifElseNode->thenBranch, args);
    }

    if (ifElseNode->elseBranch) {
        ast_printer_print_link_left(printer, &title, "if_else_else_branch",
                                    nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, ifElseNode->elseBranch, args);
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

    str title = STR_STATIC_INIT("ast_while");

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(printer, &title, nodeCount,
                           TABLE_BEGIN TITLE_ROW KEY_PORT KEY_PORT TABLE_END,
                           STR_FMT(&title), "while_condition", nodeCount,
                           "condition", "while_body", nodeCount, "body");

    if (whileNode->condition) {
        ast_printer_print_link_left(printer, &title, "while_condition",
                                    nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, whileNode->condition, args);
    }

    if (whileNode->body) {
        ast_printer_print_link_left(printer, &title, "while_body", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, whileNode->body, args);
    }
}

/**
 * @brief Print the Function Call AST node.
 * @param[in] functionCallNode The Function Call AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_function_call_printer(ast_function_call *functionCallNode,
                                      void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    str title = STR_STATIC_INIT("ast_function_call");

    str functionName = STR_NULL_INIT;

    if (functionCallNode->functionName) {
        functionName = functionCallNode->functionName->identifier;
    }

    ast_printer_print_link_right(printer, &title, nodeCount);
    ast_printer_print_node(
            printer, &title, nodeCount,
            TABLE_BEGIN TITLE_ROW KEY_PORT KEY_STR_VALUE KEY_PORT TABLE_END,
            STR_FMT(&title), "function_call_object", nodeCount, "object",
            "functionName", STR_FMT(&functionName), "function_call_args",
            nodeCount, "args");

    if (functionCallNode->object) {
        ast_printer_print_link_left(printer, &title, "function_call_object",
                                    nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, functionCallNode->object,
                              args);
    }

    if (functionCallNode->args) {
        for (node = functionCallNode->args->head; node; node = node->next) {
            ast_printer_print_link_left(printer, &title, "function_call_args",
                                        nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks,
                                  (ast_expression *) node->element, args);
        }
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
                    ast_function_call_printer((ast_function_call *) node, args);
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
                case AST_DEFINITION_TYPE_IMPORT:
                    break;
                case AST_DEFINITION_TYPE_NAMESPACE:
                    ast_namespace_printer((ast_namespace *) node, args);
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
