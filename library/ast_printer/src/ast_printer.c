/**
 * @file waitui_ast_printer.c
 * @author rick
 * @date 04.08.20
 * @brief File for the AST printer implementation
 */

#include "waitui/ast_printer.h"

#include <stdarg.h>
#include <waitui/log.h>


// -----------------------------------------------------------------------------
//  Local defines
// -----------------------------------------------------------------------------

#define WAITUI_AST_PRINTER_TABLE_BEGIN                                         \
    "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
#define WAITUI_AST_PRINTER_TABLE_END "</TABLE>"
#define WAITUI_AST_PRINTER_TITLE_ROW                                           \
    "<TR><TD COLSPAN=\"2\"><B>%.*s</B></TD></TR>"
#define WAITUI_AST_PRINTER_KEY_STR_VALUE                                       \
    "<TR><TD ALIGN=\"LEFT\">%s:</TD><TD ALIGN=\"LEFT\">%.*s</TD></TR>"
#define WAITUI_AST_PRINTER_KEY_STRING_VALUE                                    \
    "<TR><TD ALIGN=\"LEFT\">%s:</TD><TD ALIGN=\"LEFT\">%s</TD></TR>"
#define WAITUI_AST_PRINTER_KEY_PORT                                            \
    "<TR><TD ALIGN=\"LEFT\" COLSPAN=\"2\" PORT=\"%s%llu\">%s</TD></TR>"


// -----------------------------------------------------------------------------
//  Local types
// -----------------------------------------------------------------------------

/**
 * @brief Type for the printing the AST.
 */
typedef struct waitui_ast_printer {
    FILE *outputFile;
    waitui_ast_callbacks *callbacks;
    unsigned long long nodeCount;
} waitui_ast_printer;


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

/**
 * @brief Convert the boolean value to a const string.
 * @param[in] value The boolean value
 * @return The const string
 */
static inline const char *waitui_ast_printer_boolToString(bool value) {
    return value ? "true" : "false";
}

/**
 * @brief Convert the assignment operator value to a const string.
 * @param[in] operator The assignment operator value
 * @return The const string
 */
static inline const char *waitui_ast_printer_assignmentOperatorToString(
        waitui_ast_assignment_operator operator) {
    switch (operator) {
        case WAITUI_AST_ASSIGNMENT_OPERATOR_EQUAL:
            return "=";
        case WAITUI_AST_ASSIGNMENT_OPERATOR_PLUS_EQUAL:
            return "+=";
        case WAITUI_AST_ASSIGNMENT_OPERATOR_MINUS_EQUAL:
            return "-=";
        case WAITUI_AST_ASSIGNMENT_OPERATOR_TIMES_EQUAL:
            return "*=";
        case WAITUI_AST_ASSIGNMENT_OPERATOR_DIV_EQUAL:
            return "/=";
        case WAITUI_AST_ASSIGNMENT_OPERATOR_MODULO_EQUAL:
            return "%=";
        case WAITUI_AST_ASSIGNMENT_OPERATOR_AND_EQUAL:
            return "&=";
        case WAITUI_AST_ASSIGNMENT_OPERATOR_CARET_EQUAL:
            return "^=";
        case WAITUI_AST_ASSIGNMENT_OPERATOR_TILDE_EQUAL:
            return "~=";
        case WAITUI_AST_ASSIGNMENT_OPERATOR_PIPE_EQUAL:
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
waitui_ast_printer_unaryOperatorToString(waitui_ast_unary_operator operator) {
    switch (operator) {
        case WAITUI_AST_UNARY_OPERATOR_MINUS:
            return "-";
        case WAITUI_AST_UNARY_OPERATOR_NOT:
            return "!";
        case WAITUI_AST_UNARY_OPERATOR_DOUBLE_PLUS:
            return "++";
        case WAITUI_AST_UNARY_OPERATOR_DOUBLE_MINUS:
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
waitui_ast_printer_binaryOperatorToString(waitui_ast_binary_operator operator) {
    switch (operator) {
        case WAITUI_AST_BINARY_OPERATOR_PLUS:
            return "+";
        case WAITUI_AST_BINARY_OPERATOR_MINUS:
            return "-";
        case WAITUI_AST_BINARY_OPERATOR_TIMES:
            return "*";
        case WAITUI_AST_BINARY_OPERATOR_DIV:
            return "/";
        case WAITUI_AST_BINARY_OPERATOR_MODULO:
            return "%";
        case WAITUI_AST_BINARY_OPERATOR_AND:
            return "&";
        case WAITUI_AST_BINARY_OPERATOR_CARET:
            return "^";
        case WAITUI_AST_BINARY_OPERATOR_TILDE:
            return "~";
        case WAITUI_AST_BINARY_OPERATOR_PIPE:
            return "|";
        case WAITUI_AST_BINARY_OPERATOR_LESS:
            return "<";
        case WAITUI_AST_BINARY_OPERATOR_LESS_EQUAL:
            return "<=";
        case WAITUI_AST_BINARY_OPERATOR_GREATER:
            return ">";
        case WAITUI_AST_BINARY_OPERATOR_GREATER_EQUAL:
            return ">=";
        case WAITUI_AST_BINARY_OPERATOR_EQUAL:
            return "==";
        case WAITUI_AST_BINARY_OPERATOR_NOT_EQUAL:
            return "!=";
        case WAITUI_AST_BINARY_OPERATOR_DOUBLE_AND:
            return "&&";
        case WAITUI_AST_BINARY_OPERATOR_DOUBLE_PIPE:
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
static inline const char *waitui_ast_printer_functionVisibilityToString(
        waitui_ast_function_visibility visibility) {
    switch (visibility) {
        case WAITUI_AST_FUNCTION_VISIBILITY_PUBLIC:
            return "public";
        case WAITUI_AST_FUNCTION_VISIBILITY_PROTECTED:
            return "protected";
        case WAITUI_AST_FUNCTION_VISIBILITY_PRIVATE:
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
static inline void
waitui_ast_printer_printGraphNode(waitui_ast_printer *printer, str *node,
                                  unsigned long long nodeCount, char *format,
                                  ...) {
    va_list args;

    fprintf(printer->outputFile, "\t%.*s%llu [label=<", STR_FMT(node),
            nodeCount);

    va_start(args, format);
    vfprintf(printer->outputFile, format, args);
    va_end(args);

    fprintf(printer->outputFile, ">];\n");
}

/**
 * @brief Print a graph node start edge.
 * @param[in] printer The printer to print to
 * @param[in] node The node str to use
 * @param[in] port The port to use
 * @param[in] nodeCount The node count to use
 */
static inline void
waitui_ast_printer_printGraphLinkLeft(waitui_ast_printer *printer, str *node,
                                      char *port,
                                      unsigned long long nodeCount) {
    fprintf(printer->outputFile, "\t%.*s%llu:%s%llu -> ", STR_FMT(node),
            nodeCount, port, nodeCount);
}

/**
 * @brief Print a graph node end edge.
 * @param[in] printer The printer to print to
 * @param[in] node The node str to use
 * @param[in] nodeCount The node count to use
 */
static inline void
waitui_ast_printer_printGraphLinkRight(waitui_ast_printer *printer, str *node,
                                       unsigned long long nodeCount) {
    fprintf(printer->outputFile, "%.*s%llu;\n", STR_FMT(node), nodeCount);
}

/**
 * @brief Print the Program AST node.
 * @param[in] programNode The Program AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printProgram(waitui_ast_program *programNode,
                                            void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount;

    str title = STR_STATIC_INIT("waitui_ast_program");

    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_PORT WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "program_namespaces", nodeCount, "namespaces");

    {
        waitui_ast_namespace_list *namespaces =
                waitui_ast_program_getNamespaces(programNode);
        if (namespaces) {
            for (list_node *node = namespaces->head; node; node = node->next) {
                waitui_ast_printer_printGraphLinkLeft(
                        printer, &title, "program_namespaces", nodeCount);
                WAITUI_AST_EXECUTE_CALLBACKS(
                        printer->callbacks,
                        (waitui_ast_namespace *) node->element, args);
            }
        }
    }
}

/**
 * @brief Print the Namespace AST node.
 * @param[in] namespaceNode The Namespace AST node to print
 * @param[in] args The extra args for the callback call
 */
static void
waitui_ast_printer_printNamespace(waitui_ast_namespace *namespaceNode,
                                  void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount;

    str title = STR_STATIC_INIT("waitui_ast_namespace");

    str name = STR_NULL_INIT;

    {
        symbol *nameSymbol = waitui_ast_namespace_getName(namespaceNode);
        if (nameSymbol) { name = nameSymbol->identifier; }
    }

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STR_VALUE WAITUI_AST_PRINTER_KEY_PORT
                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&name), "namespaces_classes",
            nodeCount, "classes");

    {
        waitui_ast_class_list *classes =
                waitui_ast_namespace_getClasses(namespaceNode);
        if (classes) {
            list_node *node = NULL;
            for (node = classes->head; node; node = node->next) {
                waitui_ast_printer_printGraphLinkLeft(
                        printer, &title, "namespaces_classes", nodeCount);
                WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks,
                                             (waitui_ast_class *) node->element,
                                             args);
            }
        }
    }
}

/**
 * @brief Print the Class AST node.
 * @param[in] classNode The Class AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printClass(waitui_ast_class *classNode,
                                          void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_class");

    str className      = STR_NULL_INIT;
    str superClassName = STR_NULL_INIT;

    {
        symbol *classNameSymbol = waitui_ast_class_getName(classNode);
        if (classNameSymbol) { className = classNameSymbol->identifier; }
    }
    {
        symbol *superClassSymbol = waitui_ast_class_getSuperClass(classNode);
        if (superClassSymbol) { superClassName = superClassSymbol->identifier; }
    }

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STR_VALUE WAITUI_AST_PRINTER_KEY_PORT
                            WAITUI_AST_PRINTER_KEY_STR_VALUE WAITUI_AST_PRINTER_KEY_PORT
                                    WAITUI_AST_PRINTER_KEY_PORT
                                            WAITUI_AST_PRINTER_KEY_PORT
                                                    WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&className), "class_parameters",
            nodeCount, "parameters", "super", STR_FMT(&superClassName),
            "class_super_class_args", nodeCount, "superClassArgs",
            "class_properties", nodeCount, "properties", "class_functions",
            nodeCount, "functions");

    {
        waitui_ast_formal_list *parameters =
                waitui_ast_class_getParameters(classNode);
        if (parameters) {
            for (list_node *node = parameters->head; node; node = node->next) {
                waitui_ast_printer_printGraphLinkLeft(
                        printer, &title, "class_parameters", nodeCount);
                WAITUI_AST_EXECUTE_CALLBACKS(
                        printer->callbacks, (waitui_ast_formal *) node->element,
                        args);
            }
        }
    }

    {
        waitui_ast_expression_list *superClassArgs =
                waitui_ast_class_getSuperClassArgs(classNode);
        if (superClassArgs) {
            for (list_node *node = superClassArgs->head; node;
                 node            = node->next) {
                waitui_ast_printer_printGraphLinkLeft(
                        printer, &title, "class_super_class_args", nodeCount);
                WAITUI_AST_EXECUTE_CALLBACKS(
                        printer->callbacks,
                        (waitui_ast_expression *) node->element, args);
            }
        }
    }

    {
        waitui_ast_property_list *properties =
                waitui_ast_class_getProperties(classNode);
        if (properties) {
            for (list_node *node = properties->head; node; node = node->next) {
                waitui_ast_printer_printGraphLinkLeft(
                        printer, &title, "class_properties", nodeCount);
                WAITUI_AST_EXECUTE_CALLBACKS(
                        printer->callbacks,
                        (waitui_ast_property *) node->element, args);
            }
        }
    }

    {
        waitui_ast_function_list *functions =
                waitui_ast_class_getFunctions(classNode);
        if (functions) {
            for (list_node *node = functions->head; node; node = node->next) {
                waitui_ast_printer_printGraphLinkLeft(
                        printer, &title, "class_functions", nodeCount);
                WAITUI_AST_EXECUTE_CALLBACKS(
                        printer->callbacks,
                        (waitui_ast_function *) node->element, args);
            }
        }
    }
}

/**
 * @brief Print the Formal AST node.
 * @param[in] formalNode The Formal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printFormal(waitui_ast_formal *formalNode,
                                           void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_formal");

    str identifier = STR_NULL_INIT;
    str type       = STR_NULL_INIT;
    bool isLazy    = false;

    {
        symbol *identifierSymbol = waitui_ast_formal_getIdentifier(formalNode);
        if (identifierSymbol) { identifier = identifierSymbol->identifier; }
    }

    {
        symbol *typeSymbol = waitui_ast_formal_getType(formalNode);
        if (typeSymbol) { type = typeSymbol->identifier; }
    }

    isLazy = waitui_ast_formal_isLazy(formalNode);

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STR_VALUE
                            WAITUI_AST_PRINTER_KEY_STR_VALUE
                                    WAITUI_AST_PRINTER_KEY_STRING_VALUE
                                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "identifier", STR_FMT(&identifier), "type",
            STR_FMT(&type), "isLazy", waitui_ast_printer_boolToString(isLazy));
}

/**
 * @brief Print the Property AST node.
 * @param[in] propertyNode The Property AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printProperty(waitui_ast_property *propertyNode,
                                             void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_property");

    str name = STR_NULL_INIT;
    str type = STR_NULL_INIT;

    {
        symbol *propertySymbol = waitui_ast_property_getName(propertyNode);
        if (propertySymbol) { name = propertySymbol->identifier; }
    }

    {
        symbol *typeSymbol = waitui_ast_property_getType(propertyNode);
        if (typeSymbol) { type = typeSymbol->identifier; }
    }

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STR_VALUE
                            WAITUI_AST_PRINTER_KEY_STR_VALUE
                                    WAITUI_AST_PRINTER_KEY_PORT
                                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&name), "type", STR_FMT(&type),
            "property_value", nodeCount, "value");

    {
        waitui_ast_expression *value =
                waitui_ast_property_getValue(propertyNode);
        if (value) {
            waitui_ast_printer_printGraphLinkLeft(printer, &title,
                                                  "property_value", nodeCount);
            WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks, value, args);
        }
    }
}

/**
 * @brief Print the Function AST node.
 * @param[in] functionNode The Function AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printFunction(waitui_ast_function *functionNode,
                                             void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_function");

    str name         = STR_NULL_INIT;
    str returnType   = STR_NULL_INIT;
    bool isAbstract  = false;
    bool isFinal     = false;
    bool isOverwrite = false;
    waitui_ast_function_visibility visibility =
            WAITUI_AST_FUNCTION_VISIBILITY_PUBLIC;

    {
        symbol *functionNameSymbol =
                waitui_ast_function_getFunctionName(functionNode);
        if (functionNameSymbol) { name = functionNameSymbol->identifier; }
    }

    {
        symbol *returnTypeSymbol =
                waitui_ast_function_getReturnType(functionNode);
        if (returnTypeSymbol) { returnType = returnTypeSymbol->identifier; }
    }

    visibility  = waitui_ast_function_getVisibility(functionNode);
    isAbstract  = waitui_ast_function_isAbstract(functionNode);
    isFinal     = waitui_ast_function_isFinal(functionNode);
    isOverwrite = waitui_ast_function_isOverwrite(functionNode);

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STR_VALUE WAITUI_AST_PRINTER_KEY_PORT
                            WAITUI_AST_PRINTER_KEY_STR_VALUE WAITUI_AST_PRINTER_KEY_STRING_VALUE
                                    WAITUI_AST_PRINTER_KEY_STRING_VALUE WAITUI_AST_PRINTER_KEY_STRING_VALUE
                                            WAITUI_AST_PRINTER_KEY_STRING_VALUE
                                                    WAITUI_AST_PRINTER_KEY_PORT
                                                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&name), "function_parameters",
            nodeCount, "parameters", "returnType", STR_FMT(&returnType),
            "visibility",
            waitui_ast_printer_functionVisibilityToString(visibility),
            "isAbstract", waitui_ast_printer_boolToString(isAbstract),
            "isFinal", waitui_ast_printer_boolToString(isFinal), "isOverwrite",
            waitui_ast_printer_boolToString(isOverwrite), "function_body",
            nodeCount, "body");

    {
        waitui_ast_formal_list *parameters =
                waitui_ast_function_getParameters(functionNode);
        if (parameters) {
            for (list_node *node = parameters->head; node; node = node->next) {
                waitui_ast_printer_printGraphLinkLeft(
                        printer, &title, "function_parameters", nodeCount);
                WAITUI_AST_EXECUTE_CALLBACKS(
                        printer->callbacks, (waitui_ast_formal *) node->element,
                        args);
            }
        }
    }

    {
        waitui_ast_expression *body = waitui_ast_function_getBody(functionNode);
        if (body) {
            waitui_ast_printer_printGraphLinkLeft(printer, &title,
                                                  "function_body", nodeCount);
            WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks, body, args);
        }
    }
}

/**
 * @brief Print the Assignment AST node.
 * @param[in] assignmentNode The Assignment AST node to print
 * @param[in] args The extra args for the callback call
 */
static void
waitui_ast_printer_printAssignment(waitui_ast_assignment *assignmentNode,
                                   void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_assignment");

    str identifier = STR_NULL_INIT;

    if (assignmentNode->identifier) {
        identifier = assignmentNode->identifier->identifier;
    }

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STR_VALUE
                            WAITUI_AST_PRINTER_KEY_STRING_VALUE
                                    WAITUI_AST_PRINTER_KEY_PORT
                                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "identifier", STR_FMT(&identifier), "operator",
            waitui_ast_printer_assignmentOperatorToString(
                    assignmentNode->operator),
            "assignment_value", nodeCount, "value");
    if (assignmentNode->value) {
        waitui_ast_printer_printGraphLinkLeft(printer, &title,
                                              "assignment_value", nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks, assignmentNode->value,
                                     args);
    }
}

/**
 * @brief Print the String Literal AST node.
 * @param[in] stringLiteralNode The String Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printStringLiteral(
        waitui_ast_string_literal *stringLiteralNode, void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_string_literal");

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STR_VALUE
                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "value", STR_FMT(&stringLiteralNode->value));
}

/**
 * @brief Print the Integer Literal AST node.
 * @param[in] integerLiteralNode The Integer Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printIntegerLiteral(
        waitui_ast_integer_literal *integerLiteralNode, void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_integer_literal");

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STR_VALUE
                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "value", STR_FMT(&integerLiteralNode->value));
}

/**
 * @brief Print the Null Literal AST node.
 * @param[in] nullLiteralNode The Null Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void
waitui_ast_printer_printNullLiteral(waitui_ast_null_literal *nullLiteralNode,
                                    void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    (void) nullLiteralNode;

    str title = STR_STATIC_INIT("waitui_ast_null_literal");

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title));
}

/**
 * @brief Print the Boolean Literal AST node.
 * @param[in] booleanLiteralNode The Boolean Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printBooleanLiteral(
        waitui_ast_boolean_literal *booleanLiteralNode, void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_boolean_literal");

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STRING_VALUE
                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "value",
            waitui_ast_printer_boolToString(booleanLiteralNode->value));
}

/**
 * @brief Print the This Literal AST node.
 * @param[in] thisLiteralNode The This Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void
waitui_ast_printer_printThisLiteral(waitui_ast_this_literal *thisLiteralNode,
                                    void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    (void) thisLiteralNode;

    str title = STR_STATIC_INIT("waitui_ast_this_literal");

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title));
}

/**
 * @brief Print the Reference Literal AST node.
 * @param[in] referenceNode The Reference Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void
waitui_ast_printer_printReference(waitui_ast_reference *referenceNode,
                                  void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_reference");

    str identifier = STR_NULL_INIT;

    if (referenceNode->value) { identifier = referenceNode->value->identifier; }

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STR_VALUE
                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "value", STR_FMT(&identifier));
}

/**
 * @brief Print the Cast AST node.
 * @param[in] castNode The Cast AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printCast(waitui_ast_cast *castNode,
                                         void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_cast");

    str type = STR_NULL_INIT;

    if (castNode->type) { type = castNode->type->identifier; }

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_PORT WAITUI_AST_PRINTER_KEY_STR_VALUE
                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "cast_object", nodeCount, "object", "type",
            STR_FMT(&type));
    if (castNode->object) {
        waitui_ast_printer_printGraphLinkLeft(printer, &title, "cast_object",
                                              nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks, castNode->object,
                                     args);
    }
}

/**
 * @brief Print the Block AST node.
 * @param[in] blockNode The Block AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printBlock(waitui_ast_block *blockNode,
                                          void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_block");

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_PORT WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "block_expressions", nodeCount, "expressions");

    {
        waitui_ast_expression_list *expressions =
                waitui_ast_block_getExpressions(blockNode);
        if (expressions) {
            for (list_node *node = expressions->head; node; node = node->next) {
                waitui_ast_printer_printGraphLinkLeft(
                        printer, &title, "block_expressions", nodeCount);
                WAITUI_AST_EXECUTE_CALLBACKS(
                        printer->callbacks, (waitui_ast_formal *) node->element,
                        args);
            }
        }
    }
}

/**
 * @brief Print the Constructor Call AST node.
 * @param[in] constructorCallNode The Constructor Call AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printConstructorCall(
        waitui_ast_constructor_call *constructorCallNode, void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    str title = STR_STATIC_INIT("waitui_ast_constructor_call");

    str name = STR_NULL_INIT;

    if (constructorCallNode->name) {
        name = constructorCallNode->name->identifier;
    }

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STR_VALUE WAITUI_AST_PRINTER_KEY_PORT
                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "name", STR_FMT(&name), "constructor_call_args",
            nodeCount, "args");

    if (constructorCallNode->args) {
        for (node = constructorCallNode->args->head; node; node = node->next) {
            waitui_ast_printer_printGraphLinkLeft(
                    printer, &title, "constructor_call_args", nodeCount);
            WAITUI_AST_EXECUTE_CALLBACKS(
                    printer->callbacks, (waitui_ast_expression *) node->element,
                    args);
        }
    }
}

/**
 * @brief Print the Let AST node.
 * @param[in] letNode The Let AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printLet(waitui_ast_let *letNode, void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_let");

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_PORT WAITUI_AST_PRINTER_KEY_PORT
                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "let_initializations", nodeCount,
            "initializations", "let_body", nodeCount, "body");

    {
        waitui_ast_initialization_list *initializations =
                waitui_ast_let_getInitializations(letNode);
        if (initializations) {
            for (list_node *node = initializations->head; node;
                 node            = node->next) {
                waitui_ast_printer_printGraphLinkLeft(
                        printer, &title, "let_initializations", nodeCount);
                WAITUI_AST_EXECUTE_CALLBACKS(
                        printer->callbacks,
                        (waitui_ast_expression *) node->element, args);
            }
        }
    }

    {
        waitui_ast_expression *body = waitui_ast_let_getBody(letNode);
        if (body) {
            waitui_ast_printer_printGraphLinkLeft(printer, &title, "let_body",
                                                  nodeCount);
            WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks, body, args);
        }
    }
}

/**
 * @brief Print the Initialization AST node.
 * @param[in] initializationNode The Initialization AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printInitialization(
        waitui_ast_initialization *initializationNode, void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_initialization");

    str identifier = STR_NULL_INIT;
    str type       = STR_NULL_INIT;

    if (initializationNode->identifier) {
        identifier = initializationNode->identifier->identifier;
    }
    if (initializationNode->type) {
        type = initializationNode->type->identifier;
    }

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STR_VALUE
                            WAITUI_AST_PRINTER_KEY_STR_VALUE
                                    WAITUI_AST_PRINTER_KEY_PORT
                                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "identifier", STR_FMT(&identifier), "type",
            STR_FMT(&type), "initialization_value", nodeCount, "value");
    if (initializationNode->value) {
        waitui_ast_printer_printGraphLinkLeft(
                printer, &title, "initialization_value", nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks,
                                     initializationNode->value, args);
    }
}

/**
 * @brief Print the Binary Expression AST node.
 * @param[in] binaryExpressionNode The Binary Expression AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printBinaryExpression(
        waitui_ast_binary_expression *binaryExpressionNode, void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_binary_expression");

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_PORT
                            WAITUI_AST_PRINTER_KEY_STRING_VALUE
                                    WAITUI_AST_PRINTER_KEY_PORT
                                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "binary_expression_left", nodeCount, "left",
            "operator",
            waitui_ast_printer_binaryOperatorToString(
                    binaryExpressionNode->operator),
            "binary_expression_right", nodeCount, "right");

    if (binaryExpressionNode->left) {
        waitui_ast_printer_printGraphLinkLeft(
                printer, &title, "binary_expression_left", nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks,
                                     binaryExpressionNode->left, args);
    }

    if (binaryExpressionNode->right) {
        waitui_ast_printer_printGraphLinkLeft(
                printer, &title, "binary_expression_right", nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks,
                                     binaryExpressionNode->right, args);
    }
}

/**
 * @brief Print the Unary Expression AST node.
 * @param[in] unaryExpressionNode The Unary Expression AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printUnaryExpression(
        waitui_ast_unary_expression *unaryExpressionNode, void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_unary_expression");

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_STRING_VALUE
                            WAITUI_AST_PRINTER_KEY_PORT
                                    WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "operator",
            waitui_ast_printer_unaryOperatorToString(
                    unaryExpressionNode->operator),
            "unary_expression_expression", nodeCount, "expression");

    if (unaryExpressionNode->expression) {
        waitui_ast_printer_printGraphLinkLeft(
                printer, &title, "unary_expression_expression", nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks,
                                     unaryExpressionNode->expression, args);
    }
}

/**
 * @brief Print the If Else AST node.
 * @param[in] ifElseNode The If Else AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printIfElse(waitui_ast_if_else *ifElseNode,
                                           void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_if_else");

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    if (!ifElseNode->elseBranch) {
        waitui_ast_printer_printGraphNode(
                printer, &title, nodeCount,
                WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                        WAITUI_AST_PRINTER_KEY_PORT WAITUI_AST_PRINTER_KEY_PORT
                                WAITUI_AST_PRINTER_TABLE_END,
                STR_FMT(&title), "if_else_condition", nodeCount, "condition",
                "if_else_then_branch", nodeCount, "thenBranch");
    } else {
        waitui_ast_printer_printGraphNode(
                printer, &title, nodeCount,
                WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                        WAITUI_AST_PRINTER_KEY_PORT WAITUI_AST_PRINTER_KEY_PORT
                                WAITUI_AST_PRINTER_KEY_PORT
                                        WAITUI_AST_PRINTER_TABLE_END,
                STR_FMT(&title), "if_else_condition", nodeCount, "condition",
                "if_else_then_branch", nodeCount, "thenBranch",
                "if_else_else_branch", nodeCount, "elseBranch");
    }

    if (ifElseNode->condition) {
        waitui_ast_printer_printGraphLinkLeft(printer, &title,
                                              "if_else_condition", nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks, ifElseNode->condition,
                                     args);
    }

    if (ifElseNode->thenBranch) {
        waitui_ast_printer_printGraphLinkLeft(printer, &title,
                                              "if_else_then_branch", nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks, ifElseNode->thenBranch,
                                     args);
    }

    if (ifElseNode->elseBranch) {
        waitui_ast_printer_printGraphLinkLeft(printer, &title,
                                              "if_else_else_branch", nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks, ifElseNode->elseBranch,
                                     args);
    }
}

/**
 * @brief Print the While AST node.
 * @param[in] whileNode The While AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printWhile(waitui_ast_while *whileNode,
                                          void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str title = STR_STATIC_INIT("waitui_ast_while");

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_PORT WAITUI_AST_PRINTER_KEY_PORT
                            WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "while_condition", nodeCount, "condition",
            "while_body", nodeCount, "body");

    if (whileNode->condition) {
        waitui_ast_printer_printGraphLinkLeft(printer, &title,
                                              "while_condition", nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks, whileNode->condition,
                                     args);
    }

    if (whileNode->body) {
        waitui_ast_printer_printGraphLinkLeft(printer, &title, "while_body",
                                              nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks, whileNode->body, args);
    }
}

/**
 * @brief Print the Function Call AST node.
 * @param[in] functionCallNode The Function Call AST node to print
 * @param[in] args The extra args for the callback call
 */
static void
waitui_ast_printer_printFunctionCall(waitui_ast_function_call *functionCallNode,
                                     void *args) {
    waitui_ast_printer *printer  = (waitui_ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    str title = STR_STATIC_INIT("waitui_ast_function_call");

    str functionName = STR_NULL_INIT;

    if (functionCallNode->functionName) {
        functionName = functionCallNode->functionName->identifier;
    }

    waitui_ast_printer_printGraphLinkRight(printer, &title, nodeCount);
    waitui_ast_printer_printGraphNode(
            printer, &title, nodeCount,
            WAITUI_AST_PRINTER_TABLE_BEGIN WAITUI_AST_PRINTER_TITLE_ROW
                    WAITUI_AST_PRINTER_KEY_PORT WAITUI_AST_PRINTER_KEY_STR_VALUE
                            WAITUI_AST_PRINTER_KEY_PORT
                                    WAITUI_AST_PRINTER_TABLE_END,
            STR_FMT(&title), "function_call_object", nodeCount, "object",
            "functionName", STR_FMT(&functionName), "function_call_args",
            nodeCount, "args");

    if (functionCallNode->object) {
        waitui_ast_printer_printGraphLinkLeft(
                printer, &title, "function_call_object", nodeCount);
        WAITUI_AST_EXECUTE_CALLBACKS(printer->callbacks,
                                     functionCallNode->object, args);
    }

    if (functionCallNode->args) {
        for (node = functionCallNode->args->head; node; node = node->next) {
            waitui_ast_printer_printGraphLinkLeft(
                    printer, &title, "function_call_args", nodeCount);
            WAITUI_AST_EXECUTE_CALLBACKS(
                    printer->callbacks, (waitui_ast_expression *) node->element,
                    args);
        }
    }
}

/**
 * @brief Print the AST definition.
 * @param[in] definition The AST definition to print
 * @param[in] args The extra args for the callback call
 */
static void
waitui_ast_printer_printDefinition(waitui_ast_definition *definition,
                                   void *args) {
    switch (waitui_ast_definition_getDefinitionType(definition)) {
        case WAITUI_AST_DEFINITION_TYPE_FORMAL:
            waitui_ast_printer_printFormal((waitui_ast_formal *) definition,
                                           args);
            break;
        case WAITUI_AST_DEFINITION_TYPE_PROPERTY:
            waitui_ast_printer_printProperty((waitui_ast_property *) definition,
                                             args);
            break;
        case WAITUI_AST_DEFINITION_TYPE_FUNCTION:
            waitui_ast_printer_printFunction((waitui_ast_function *) definition,
                                             args);
            break;
        case WAITUI_AST_DEFINITION_TYPE_CLASS:
            waitui_ast_printer_printClass((waitui_ast_class *) definition,
                                          args);
            break;
        case WAITUI_AST_DEFINITION_TYPE_PROGRAM:
            waitui_ast_printer_printProgram((waitui_ast_program *) definition,
                                            args);
            break;
        case WAITUI_AST_DEFINITION_TYPE_IMPORT:
            break;
        case WAITUI_AST_DEFINITION_TYPE_NAMESPACE:
            waitui_ast_printer_printNamespace(
                    (waitui_ast_namespace *) definition, args);
            break;
        default:
            waitui_log_trace("trying to print a definition with an undefined "
                             "definition type");
            break;
    }
}

/**
 * @brief Print the AST expression.
 * @param[in] expression The AST expression to print
 * @param[in] args The extra args for the callback call
 */
static void
waitui_ast_printer_printExpression(waitui_ast_expression *expression,
                                   void *args) {
    switch (waitui_ast_expression_getExpressionType(expression)) {
        case WAITUI_AST_EXPRESSION_TYPE_INTEGER_LITERAL:
            waitui_ast_printer_printIntegerLiteral(
                    (waitui_ast_integer_literal *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_BOOLEAN_LITERAL:
            waitui_ast_printer_printBooleanLiteral(
                    (waitui_ast_boolean_literal *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_DECIMAL_LITERAL:
            break;
        case WAITUI_AST_EXPRESSION_TYPE_NULL_LITERAL:
            waitui_ast_printer_printNullLiteral(
                    (waitui_ast_null_literal *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_STRING_LITERAL:
            waitui_ast_printer_printStringLiteral(
                    (waitui_ast_string_literal *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_THIS_LITERAL:
            waitui_ast_printer_printThisLiteral(
                    (waitui_ast_this_literal *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_ASSIGNMENT:
            waitui_ast_printer_printAssignment(
                    (waitui_ast_assignment *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_REFERENCE:
            waitui_ast_printer_printReference(
                    (waitui_ast_reference *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_CAST:
            waitui_ast_printer_printCast((waitui_ast_cast *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_INITIALIZATION:
            waitui_ast_printer_printInitialization(
                    (waitui_ast_initialization *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_LET:
            waitui_ast_printer_printLet((waitui_ast_let *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_BLOCK:
            waitui_ast_printer_printBlock((waitui_ast_block *) expression,
                                          args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_CONSTRUCTOR_CALL:
            waitui_ast_printer_printConstructorCall(
                    (waitui_ast_constructor_call *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_FUNCTION_CALL:
            waitui_ast_printer_printFunctionCall(
                    (waitui_ast_function_call *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_SUPER_FUNCTION_CALL:
            break;
        case WAITUI_AST_EXPRESSION_TYPE_BINARY_EXPRESSION:
            waitui_ast_printer_printBinaryExpression(
                    (waitui_ast_binary_expression *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_UNARY_EXPRESSION:
            waitui_ast_printer_printUnaryExpression(
                    (waitui_ast_unary_expression *) expression, args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_IF_ELSE:
            waitui_ast_printer_printIfElse((waitui_ast_if_else *) expression,
                                           args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_WHILE:
            waitui_ast_printer_printWhile((waitui_ast_while *) expression,
                                          args);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_LAZY_EXPRESSION:
            break;
        case WAITUI_AST_EXPRESSION_TYPE_NATIVE_EXPRESSION:
            break;
        default:
            waitui_log_trace("trying to print a expression with an undefined "
                             "expression type");
            break;
    }
}

/**
 * @brief Print the AST node.
 * @param[in] node The AST node to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printNode(waitui_ast_node *node, void *args) {
    switch (waitui_ast_node_getNodeType(node)) {
        case WAITUI_AST_NODE_TYPE_EXPRESSION:
            waitui_ast_printer_printExpression((waitui_ast_expression *) node,
                                               args);
            break;
        case WAITUI_AST_NODE_TYPE_DEFINITION:
            waitui_ast_printer_printDefinition((waitui_ast_definition *) node,
                                               args);
            break;
        default:
            waitui_log_trace(
                    "trying to print a node with an undefined node type");
            break;
    }
}

/**
 * @brief Print the AST.
 * @param[in] ast The AST to print
 * @param[in] args The extra args for the callback call
 */
static void waitui_ast_printer_printAst(waitui_ast *ast, void *args) {
    waitui_ast_printer *printer = (waitui_ast_printer *) args;

    if (!ast || !printer || !printer->outputFile || !printer->callbacks) {
        return;
    }

    fprintf(printer->outputFile, "digraph AST {\n");
    fprintf(printer->outputFile, "\tnode [shape=plain]\n");

    waitui_ast_walk(ast, printer->callbacks, printer);

    fprintf(printer->outputFile, "}\n");
}


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

void waitui_ast_printer_generateGraph(const waitui_ast *ast, FILE *file) {
    waitui_log_trace("start generating the waitui_ast graph");

    if (!ast || !file) { return; }

    waitui_ast_callbacks callbacks = {
            .preNodeCallback  = NULL,
            .nodeCallback     = waitui_ast_printer_printNode,
            .postNodeCallback = NULL,
    };
    waitui_ast_printer printer = {
            .callbacks  = &callbacks,
            .outputFile = file,
    };

    waitui_ast_printer_printAst((waitui_ast *) ast, &printer);

    waitui_log_trace("end generating the waitui_ast graph");
}