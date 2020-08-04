/**
 * @file ast_node.h
 * @author rick
 * @date 08.03.20
 * @brief File for the AST node implementation
 */

#ifndef BLINK_AST_NODE_H
#define BLINK_AST_NODE_H

#include <blink/list.h>
#include <blink/str.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Node types possible for an AST node.
 */
typedef enum ast_node_type {
    AST_NODE_TYPE_EXPRESSION,
    AST_NODE_TYPE_DEFINITION,
} ast_node_type;

/**
 * @brief Definition node types possible for a definition AST node.
 */
typedef enum ast_definition_type {
    AST_DEFINITION_TYPE_FORMAL,
    AST_DEFINITION_TYPE_PROPERTY,
    AST_DEFINITION_TYPE_FUNCTION,
    AST_DEFINITION_TYPE_CLASS,
    AST_DEFINITION_TYPE_PROGRAM,
} ast_definition_type;

/**
 * @brief Expression node types possible for an expression AST node.
 */
typedef enum ast_expression_type {
    AST_EXPRESSION_TYPE_INTEGER_LITERAL,
    AST_EXPRESSION_TYPE_BOOLEAN_LITERAL,
    AST_EXPRESSION_TYPE_DECIMAL_LITERAL,
    AST_EXPRESSION_TYPE_NULL_LITERAL,
    AST_EXPRESSION_TYPE_STRING_LITERAL,
    AST_EXPRESSION_TYPE_THIS_LITERAL,
    AST_EXPRESSION_TYPE_ASSIGNMENT,
    AST_EXPRESSION_TYPE_REFERENCE,
    AST_EXPRESSION_TYPE_CAST,
    AST_EXPRESSION_TYPE_INITIALIZATION,
    AST_EXPRESSION_TYPE_LET,
    AST_EXPRESSION_TYPE_BLOCK,
    AST_EXPRESSION_TYPE_CONSTRUCTOR_CALL,
    AST_EXPRESSION_TYPE_FUNCTION_CALL,
    AST_EXPRESSION_TYPE_SUPER_FUNCTION_CALL,
    AST_EXPRESSION_TYPE_BINARY_EXPRESSION,
    AST_EXPRESSION_TYPE_UNARY_EXPRESSION,
    AST_EXPRESSION_TYPE_IF_ELSE,
    AST_EXPRESSION_TYPE_WHILE,
    AST_EXPRESSION_TYPE_LAZY_EXPRESSION,
    AST_EXPRESSION_TYPE_NATIVE_EXPRESSION,
} ast_expression_type;

/**
 * @brief Type representing an AST node.
 */
typedef struct ast_node ast_node;

/**
 * @brief TODO
 */
typedef void (*ast_node_callback)(ast_node *node, void *args);

/**
 * @brief Type representing a definition AST node.
 */
typedef struct ast_definition ast_definition;

/**
 * @brief Type representing an expression AST node.
 */
typedef struct ast_expression ast_expression;

/**
 * @brief Type representing a program AST node.
 */
typedef struct ast_program ast_program;

/**
 * @brief Type representing a class AST node.
 */
typedef struct ast_class ast_class;

/**
 * @brief Type representing a formal AST node.
 */
typedef struct ast_formal ast_formal;

/**
 * @brief Type representing a property AST node.
 */
typedef struct ast_property ast_property;

/**
 * @brief Function visibility types possible for a function AST node.
 */
typedef enum ast_function_visibility {
    AST_FUNCTION_VISIBILITY_PUBLIC,
    AST_FUNCTION_VISIBILITY_PROTECTED,
    AST_FUNCTION_VISIBILITY_PRIVATE,
} ast_function_visibility;

/**
 * @brief Type representing a function AST node.
 */
typedef struct ast_function ast_function;

/**
 * @brief Type representing a block AST node.
 */
typedef struct ast_block ast_block;

/**
 * @brief Type representing a let AST node.
 */
typedef struct ast_let ast_let;

/**
 * @brief Type representing an initialization AST node.
 */
typedef struct ast_initialization ast_initialization;

/**
 * @brief Assignment operator types possible for an assignment AST node.
 */
typedef enum ast_assignment_operator {
    AST_ASSIGNMENT_OPERATOR_EQUAL,
    AST_ASSIGNMENT_OPERATOR_PLUS_EQUAL,
    AST_ASSIGNMENT_OPERATOR_MINUS_EQUAL,
    AST_ASSIGNMENT_OPERATOR_TIMES_EQUAL,
    AST_ASSIGNMENT_OPERATOR_DIV_EQUAL,
    AST_ASSIGNMENT_OPERATOR_MODULO_EQUAL,
    AST_ASSIGNMENT_OPERATOR_AND_EQUAL,
    AST_ASSIGNMENT_OPERATOR_CARET_EQUAL,
    AST_ASSIGNMENT_OPERATOR_TILDE_EQUAL,
    AST_ASSIGNMENT_OPERATOR_PIPE_EQUAL,
} ast_assignment_operator;

/**
 * @brief Type representing an assignment AST node.
 */
typedef struct ast_assignment ast_assignment;

/**
 * @brief Type representing a cast AST node.
 */
typedef struct ast_cast ast_cast;

/**
 * @brief Type representing an if else AST node.
 */
typedef struct ast_if_else ast_if_else;

/**
 * @brief Type representing a while AST node.
 */
typedef struct ast_while ast_while;

/**
 * @brief Binary operator types possible for a binary expression AST node.
 */
typedef enum ast_binary_operator {
    AST_BINARY_OPERATOR_PLUS,
    AST_BINARY_OPERATOR_MINUS,
    AST_BINARY_OPERATOR_TIMES,
    AST_BINARY_OPERATOR_DIV,
    AST_BINARY_OPERATOR_MODULO,
    AST_BINARY_OPERATOR_AND,
    AST_BINARY_OPERATOR_CARET,
    AST_BINARY_OPERATOR_TILDE,
    AST_BINARY_OPERATOR_PIPE,
    AST_BINARY_OPERATOR_LESS,
    AST_BINARY_OPERATOR_LESS_EQUAL,
    AST_BINARY_OPERATOR_GREATER,
    AST_BINARY_OPERATOR_GREATER_EQUAL,
    AST_BINARY_OPERATOR_EQUAL,
    AST_BINARY_OPERATOR_NOT_EQUAL,
    AST_BINARY_OPERATOR_DOUBLE_AND,
    AST_BINARY_OPERATOR_DOUBLE_PIPE,
} ast_binary_operator;

/**
 * @brief Type representing a binary expression AST node.
 */
typedef struct ast_binary_expression ast_binary_expression;

/**
 * @brief Type representing an unary expression AST node.
 */
typedef struct ast_unary_expression ast_unary_expression;

typedef struct ast_lazy_expression ast_lazy_expression;
typedef struct ast_native_expression ast_native_expression;

/**
 * @brief Type representing a constructor call AST node.
 */
typedef struct ast_constructor_call ast_constructor_call;

/**
 * @brief Type representing a function call AST node.
 */
typedef struct ast_function_call ast_function_call;

/**
 * @brief Type representing a super function call AST node.
 */
typedef struct ast_super_function_call ast_super_function_call;

/**
 * @brief Type representing a reference AST node.
 */
typedef struct ast_reference ast_reference;

/**
 * @brief Type representing a this literal AST node.
 */
typedef struct ast_this_literal ast_this_literal;

/**
 * @brief Type representing an integer literal AST node.
 */
typedef struct ast_integer_literal ast_integer_literal;

/**
 * @brief Type representing a boolean literal AST node.
 */
typedef struct ast_boolean_literal ast_boolean_literal;

/**
 * @brief Type representing a decimal literal AST node.
 */
typedef struct ast_decimal_literal ast_decimal_literal;

/**
 * @brief Type representing a null literal AST node.
 */
typedef struct ast_null_literal ast_null_literal;

/**
 * @brief Type representing a string literal AST node.
 */
typedef struct ast_string_literal ast_string_literal;

CREATE_LIST_TYPE(INTERFACE, ast_class, class)
CREATE_LIST_TYPE(INTERFACE, ast_expression, expression)
CREATE_LIST_TYPE(INTERFACE, ast_formal, formal)
CREATE_LIST_TYPE(INTERFACE, ast_property, property)
CREATE_LIST_TYPE(INTERFACE, ast_function, function)
CREATE_LIST_TYPE(INTERFACE, ast_initialization, initialization)


// -----------------------------------------------------------------------------
//  Public defines
// -----------------------------------------------------------------------------

/**
 * @brief Properties for an AST node in general.
 */
#define AST_NODE_PROPERTIES                                                    \
    ast_node_type astNodeType;                                                 \
    unsigned long long line;                                                   \
    unsigned long long column;

/**
 * @brief Properties for a definition AST node in general.
 */
#define AST_DEFINITION_PROPERTIES                                              \
    AST_NODE_PROPERTIES                                                        \
    ast_definition_type astDefinitionType;

/**
 * @brief Properties for an expression AST node in general.
 */
#define AST_EXPRESSION_PROPERTIES                                              \
    AST_NODE_PROPERTIES                                                        \
    ast_expression_type astExpressionType;                                     \
    void *expressionType;// FIXME: not specified


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Destroy a definition node from the AST.
 * @param[in,out] this The definition node to destroy
 */
extern void ast_definition_destroy(ast_definition **this);

/**
 * @brief Destroy a expression node from the AST.
 * @param[in,out] this The expression node to destroy
 */
extern void ast_expression_destroy(ast_expression **this);

/**
 * @brief Create a program node for the AST.
 * @param[in] classes The list of classes for the program
 * @return On success a pointer to ast_program, else NULL
 */
extern ast_program *ast_program_new(ast_class_list *classes);

/**
 * @brief Destroy a program node and its content.
 * @param[in,out] this The program node to destroy
 */
extern void ast_program_destroy(ast_program **this);

/**
 * @brief Create a class node for the AST. TODO
 * @param[in] line
 * @param[in] column
 * @param[in] name
 * @param[in] parameters
 * @param[in] superClass
 * @param[in] superClassArgs
 * @param[in] properties
 * @param[in] functions
 * @return On success a pointer to ast_class, else NULL
 */
extern ast_class *ast_class_new(unsigned long int line,
                                unsigned long int column, str name,
                                ast_formal_list *parameters, str superClass,
                                ast_expression_list *superClassArgs,
                                ast_property_list *properties,
                                ast_function_list *functions);

/**
 * @brief Destroy a class node and its content.
 * @param[in,out] this The class node to destroy
 */
extern void ast_class_destroy(ast_class **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param identifier
 * @param type
 * @param isLazy
 * @return
 */
extern ast_formal *ast_formal_new(unsigned long int line,
                                  unsigned long int column, str identifier,
                                  str type, int isLazy);

/**
 * @brief TODO
 * @param this
 */
extern void ast_formal_destroy(ast_formal **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param name
 * @param parameters
 * @param returnType
 * @param body
 * @param visibility
 * @param isAbstract
 * @param isFinal
 * @param isOverwrite
 * @return
 */
extern ast_function *
ast_function_new(unsigned long int line, unsigned long int column, str name,
                 ast_formal_list *parameters, str returnType,
                 ast_expression *body, ast_function_visibility visibility,
                 int isAbstract, int isFinal, int isOverwrite);

/**
 * @brief TODO
 * @param this
 */
extern void ast_function_destroy(ast_function **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param name
 * @param type
 * @param value
 * @return
 */
extern ast_property *ast_property_new(unsigned long int line,
                                      unsigned long int column, str name,
                                      str type, ast_expression *value);

/**
 * @brief TODO
 * @param this
 */
extern void ast_property_destroy(ast_property **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param expressions
 * @return
 */
extern ast_block *ast_block_new(unsigned long int line,
                                unsigned long int column,
                                ast_expression_list *expressions);

/**
 * @brief TODO
 * @param this
 */
extern void ast_block_destroy(ast_block **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param initializations
 * @param body
 * @return
 */
extern ast_let *ast_let_new(unsigned long int line, unsigned long int column,
                            ast_initialization_list *initializations,
                            ast_expression *body);

/**
 * @brief TODO
 * @param this
 */
extern void ast_let_destroy(ast_let **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param identifier
 * @param type
 * @param value
 * @return
 */
extern ast_initialization *ast_initialization_new(unsigned long int line,
                                                  unsigned long int column,
                                                  str identifier, str type,
                                                  ast_expression *value);

/**
 * @brief TODO
 * @param this
 */
extern void ast_initialization_destroy(ast_initialization **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param identifier
 * @param operator
 * @param value
 * @return
 */
extern ast_assignment *ast_assignment_new(unsigned long int line,
                                          unsigned long int column,
                                          str identifier,
                                          ast_assignment_operator operator,
                                          ast_expression * value);

/**
 * @brief TODO
 * @param this
 */
extern void ast_assignment_destroy(ast_assignment **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param object
 * @param type
 * @return
 */
extern ast_cast *ast_cast_new(unsigned long int line, unsigned long int column,
                              ast_expression *object, str type);

/**
 * @brief TODO
 * @param this
 */
extern void ast_cast_destroy(ast_cast **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param condition
 * @param thenBranch
 * @param elseBranch
 * @return
 */
extern ast_if_else *ast_if_else_new(unsigned long int line,
                                    unsigned long int column,
                                    ast_expression *condition,
                                    ast_expression *thenBranch,
                                    ast_expression *elseBranch);

/**
 * @brief TODO
 * @param this
 */
extern void ast_if_else_destroy(ast_if_else **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param condition
 * @param body
 * @return
 */
extern ast_while *ast_while_new(unsigned long int line,
                                unsigned long int column,
                                ast_expression *condition,
                                ast_expression *body);

/**
 * @brief TODO
 * @param this
 */
extern void ast_while_destroy(ast_while **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param left
 * @param operator
 * @param right
 * @return
 */
extern ast_binary_expression *
ast_binary_expression_new(unsigned long int line, unsigned long int column,
                          ast_expression *left, ast_binary_operator operator,
                          ast_expression * right);

/**
 * @brief TODO
 * @param this
 */
extern void ast_binary_expression_destroy(ast_binary_expression **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param operator
 * @param expression
 * @return
 */
extern ast_unary_expression *
ast_unary_expression_new(unsigned long int line, unsigned long int column,
                         str operator, ast_expression * expression);

/**
 * @brief TODO
 * @param this
 */
extern void ast_unary_expression_destroy(ast_unary_expression **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param expression
 * @param context
 * @return
 */
extern ast_lazy_expression *ast_lazy_expression_new(unsigned long int line,
                                                    unsigned long int column,
                                                    ast_expression *expression,
                                                    void *context);

/**
 * @brief TODO
 * @param this
 */
extern void ast_lazy_expression_destroy(ast_lazy_expression **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param func
 * @return
 */
extern ast_native_expression *
ast_native_expression_new(unsigned long int line, unsigned long int column,
                          void *func);

/**
 * @brief TODO
 * @param this
 */
extern void ast_native_expression_destroy(ast_native_expression **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param type
 * @param args
 * @return
 */
extern ast_constructor_call *
ast_constructor_call_new(unsigned long int line, unsigned long int column,
                         str type, ast_expression_list *args);

/**
 * @brief TODO
 * @param this
 */
extern void ast_constructor_call_destroy(ast_constructor_call **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param object
 * @param functionName
 * @param args
 * @return
 */
extern ast_function_call *ast_function_call_new(unsigned long int line,
                                                unsigned long int column,
                                                ast_expression *object,
                                                str functionName,
                                                ast_expression_list *args);

/**
 * @brief TODO
 * @param this
 */
extern void ast_function_call_destroy(ast_function_call **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param functionName
 * @param args
 * @return
 */
extern ast_super_function_call *
ast_super_function_call_new(unsigned long int line, unsigned long int column,
                            str functionName, ast_expression_list *args);

/**
 * @brief TODO
 * @param this
 */
extern void ast_super_function_call_destroy(ast_super_function_call **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param identifier
 * @return
 */
extern ast_reference *ast_reference_new(unsigned long int line,
                                        unsigned long int column,
                                        str identifier);

/**
 * @brief TODO
 * @param this
 */
extern void ast_reference_destroy(ast_reference **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @return
 */
extern ast_this_literal *ast_this_literal_new(unsigned long int line,
                                              unsigned long int column);

/**
 * @brief TODO
 * @param this
 */
extern void ast_this_literal_destroy(ast_this_literal **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param value
 * @return
 */
extern ast_integer_literal *ast_integer_literal_new(unsigned long int line,
                                                    unsigned long int column,
                                                    str value);

/**
 * @brief TODO
 * @param this
 */
extern void ast_integer_literal_destroy(ast_integer_literal **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param value
 * @return
 */
extern ast_boolean_literal *ast_boolean_literal_new(unsigned long int line,
                                                    unsigned long int column,
                                                    int value);

/**
 * @brief TODO
 * @param this
 */
extern void ast_boolean_literal_destroy(ast_boolean_literal **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param value
 * @return
 */
extern ast_decimal_literal *ast_decimal_literal_new(unsigned long int line,
                                                    unsigned long int column,
                                                    str value);

/**
 * @brief TODO
 * @param this
 */
extern void ast_decimal_literal_destroy(ast_decimal_literal **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @return
 */
extern ast_null_literal *ast_null_literal_new(unsigned long int line,
                                              unsigned long int column);

/**
 * @brief TODO
 * @param this
 */
extern void ast_null_literal_destroy(ast_null_literal **this);

/**
 * @brief TODO
 * @param line
 * @param column
 * @param value
 * @return
 */
extern ast_string_literal *ast_string_literal_new(unsigned long int line,
                                                  unsigned long int column,
                                                  str value);

/**
 * @brief TODO
 * @param this
 */
extern void ast_string_literal_destroy(ast_string_literal **this);


// -----------------------------------------------------------------------------
//  Public types implementation
// -----------------------------------------------------------------------------

struct ast_node {
    AST_NODE_PROPERTIES
};

struct ast_definition {
    AST_DEFINITION_PROPERTIES
};

struct ast_expression {
    AST_EXPRESSION_PROPERTIES
};

struct ast_program {
    AST_DEFINITION_PROPERTIES
    ast_class_list *classes;
};

struct ast_class {
    AST_DEFINITION_PROPERTIES
    str name;
    ast_formal_list *parameters;
    str superClass;
    ast_expression_list *superClassArgs;
    ast_property_list *properties;
    ast_function_list *functions;
};

struct ast_formal {
    AST_DEFINITION_PROPERTIES
    str identifier;
    str type;
    int isLazy;
};

struct ast_property {
    AST_DEFINITION_PROPERTIES
    str name;
    str type;
    ast_expression *value;
};

struct ast_function {
    AST_DEFINITION_PROPERTIES
    str name;
    ast_formal_list *parameters;
    str returnType;
    ast_expression *body;
    ast_function_visibility visibility;
    int isAbstract;
    int isFinal;
    int isOverwrite;
};

struct ast_block {
    AST_EXPRESSION_PROPERTIES
    ast_expression_list *expressions;
};

struct ast_let {
    AST_EXPRESSION_PROPERTIES
    ast_initialization_list *initializations;
    ast_expression *body;
};

struct ast_initialization {
    AST_EXPRESSION_PROPERTIES
    str identifier;
    str type;
    ast_expression *value;
};

struct ast_assignment {
    AST_EXPRESSION_PROPERTIES
    str identifier;
    ast_assignment_operator operator;
    ast_expression *value;
};

struct ast_cast {
    AST_EXPRESSION_PROPERTIES
    str type;
    ast_expression *object;
};

struct ast_if_else {
    AST_EXPRESSION_PROPERTIES
    ast_expression *condition;
    ast_expression *thenBranch;
    ast_expression *elseBranch;
};

struct ast_while {
    AST_EXPRESSION_PROPERTIES
    ast_expression *condition;
    ast_expression *body;
};

struct ast_binary_expression {
    AST_EXPRESSION_PROPERTIES
    ast_expression *left;
    ast_binary_operator operator;
    ast_expression *right;
};

struct ast_unary_expression {
    AST_EXPRESSION_PROPERTIES
    str operator;
    ast_expression *expression;
};

struct ast_lazy_expression {
    AST_EXPRESSION_PROPERTIES
    ast_expression *expression;
    void *context;// FIXME: not specified
};

struct ast_native_expression {
    AST_EXPRESSION_PROPERTIES
    void *func;// FIXME: not specified
};

struct ast_constructor_call {
    AST_EXPRESSION_PROPERTIES
    str type;
    ast_expression_list *args;
};

struct ast_function_call {
    AST_EXPRESSION_PROPERTIES
    ast_expression *object;
    str functionName;
    ast_expression_list *args;
};

struct ast_super_function_call {
    AST_EXPRESSION_PROPERTIES
    str functionName;
    ast_expression_list *args;
};

struct ast_reference {
    AST_EXPRESSION_PROPERTIES
    str identifier;
};

struct ast_this_literal {
    AST_EXPRESSION_PROPERTIES
};

struct ast_integer_literal {
    AST_EXPRESSION_PROPERTIES
    str value;
};

struct ast_boolean_literal {
    AST_EXPRESSION_PROPERTIES
    int value;
};

struct ast_decimal_literal {
    AST_EXPRESSION_PROPERTIES
    str value;
};

struct ast_null_literal {
    AST_EXPRESSION_PROPERTIES
};

struct ast_string_literal {
    AST_EXPRESSION_PROPERTIES
    str value;
};

#endif// BLINK_AST_NODE_H
