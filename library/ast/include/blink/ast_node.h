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
 * @brief Type representing a definition AST node.
 */
typedef struct ast_definition ast_definition;

/**
 * @brief Type representing an expression AST node.
 */
typedef struct ast_expression ast_expression;

typedef struct ast_program ast_program;

typedef struct ast_class ast_class;

typedef struct ast_formal ast_formal;

typedef struct ast_property ast_property;

typedef enum ast_function_visibility {
    AST_FUNCTION_VISIBILITY_PUBLIC,
    AST_FUNCTION_VISIBILITY_PROTECTED,
    AST_FUNCTION_VISIBILITY_PRIVATE,
} ast_function_visibility;

typedef struct ast_function ast_function;

typedef struct ast_block ast_block;

typedef struct ast_let ast_let;

typedef struct ast_initialization ast_initialization;

typedef struct ast_assignment ast_assignment;
typedef struct ast_cast ast_cast;

typedef struct ast_if_else ast_if_else;
typedef struct ast_while ast_while;

typedef struct ast_binary_expression ast_binary_expression;
typedef struct ast_unary_expression ast_unary_expression;

typedef struct ast_lazy_expression ast_lazy_expression;
typedef struct ast_native_expression ast_native_expression;

typedef struct ast_constructor_call ast_constructor_call;
typedef struct ast_function_call ast_function_call;
typedef struct ast_super_function_call ast_super_function_call;

typedef struct ast_reference ast_reference;
typedef struct ast_this_literal ast_this_literal;
typedef struct ast_super_literal ast_super_literal;
typedef struct ast_integer_literal ast_integer_literal;
typedef struct ast_boolean_literal ast_boolean_literal;
typedef struct ast_decimal_literal ast_decimal_literal;
typedef struct ast_null_literal ast_null_literal;
typedef struct ast_string_literal ast_string_literal;

typedef void (*ast_program_callback)(ast_program *program, void *args);
typedef void (*ast_class_callback)(ast_class *class, void *args);
typedef void (*ast_formal_callback)(ast_formal *formal, void *args);
typedef void (*ast_function_callback)(ast_function *function, void *args);
typedef void (*ast_expression_callback)(ast_expression *expression, void *args);
typedef void (*ast_assignment_callback)(ast_assignment *assignment, void *args);
typedef void (*ast_string_literal_callback)(ast_string_literal *stringLiteral,
                                            void *args);

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
 * @brief Create a class node for the AST.
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

extern ast_formal *ast_formal_new(unsigned long int line,
                                  unsigned long int column, str identifier,
                                  str type, int isLazy);
extern void ast_formal_destroy(ast_formal **this);

extern ast_function *
ast_function_new(unsigned long int line, unsigned long int column, str name,
                 ast_formal_list *parameters, str returnType,
                 ast_expression *body, ast_function_visibility visibility,
                 int isAbstract, int isFinal, int isOverwrite);
extern void ast_function_destroy(ast_function **this);

extern ast_property *ast_property_new(unsigned long int line,
                                      unsigned long int column, str name,
                                      str type, ast_expression *value);
extern void ast_property_destroy(ast_property **this);

extern ast_block *ast_block_new(unsigned long int line,
                                unsigned long int column,
                                ast_expression_list *expressions);
extern void ast_block_destroy(ast_block **this);

extern ast_let *ast_let_new(unsigned long int line, unsigned long int column,
                            ast_initialization_list *initializations,
                            ast_expression *body);
extern void ast_let_destroy(ast_let **this);

extern ast_initialization *ast_initialization_new(unsigned long int line,
                                                  unsigned long int column,
                                                  str identifier, str type,
                                                  ast_expression *value);
extern void ast_initialization_destroy(ast_initialization **this);

extern ast_assignment *ast_assignment_new(unsigned long int line,
                                          unsigned long int column,
                                          str identifier, str operator,
                                          ast_expression * value);
extern void ast_assignment_destroy(ast_assignment **this);

extern ast_cast *ast_cast_new(unsigned long int line, unsigned long int column,
                              ast_expression *object, str type);
extern void ast_cast_destroy(ast_cast **this);

extern ast_if_else *ast_if_else_new(unsigned long int line,
                                    unsigned long int column,
                                    ast_expression *condition,
                                    ast_expression *thenBranch,
                                    ast_expression *elseBranch);
extern void ast_if_else_destroy(ast_if_else **this);

extern ast_while *ast_while_new(unsigned long int line,
                                unsigned long int column,
                                ast_expression *condition,
                                ast_expression *body);
extern void ast_while_destroy(ast_while **this);

extern ast_binary_expression *
ast_binary_expression_new(unsigned long int line, unsigned long int column,
                          ast_expression *left, str operator,
                          ast_expression * right);
extern void ast_binary_expression_destroy(ast_binary_expression **this);

extern ast_unary_expression *
ast_unary_expression_new(unsigned long int line, unsigned long int column,
                         str operator, ast_expression * expression);
extern void ast_unary_expression_destroy(ast_unary_expression **this);

extern ast_lazy_expression *ast_lazy_expression_new(unsigned long int line,
                                                    unsigned long int column,
                                                    ast_expression *expression,
                                                    void *context);
extern void ast_lazy_expression_destroy(ast_lazy_expression **this);

extern ast_native_expression *
ast_native_expression_new(unsigned long int line, unsigned long int column,
                          void *func);
extern void ast_native_expression_destroy(ast_native_expression **this);

extern ast_constructor_call *
ast_constructor_call_new(unsigned long int line, unsigned long int column,
                         str type, ast_expression_list *args);
extern void ast_constructor_call_destroy(ast_constructor_call **this);

extern ast_function_call *ast_function_call_new(unsigned long int line,
                                                unsigned long int column,
                                                ast_expression *object,
                                                str functionName,
                                                ast_expression_list *args);
extern void ast_function_call_destroy(ast_function_call **this);

extern ast_super_function_call *
ast_super_function_call_new(unsigned long int line, unsigned long int column,
                            str functionName, ast_expression_list *args);
extern void ast_super_function_call_destroy(ast_super_function_call **this);

extern ast_reference *ast_reference_new(unsigned long int line,
                                        unsigned long int column,
                                        str identifier);
extern void ast_reference_destroy(ast_reference **this);

extern ast_this_literal *ast_this_literal_new(unsigned long int line,
                                              unsigned long int column);
extern void ast_this_literal_destroy(ast_this_literal **this);

extern ast_integer_literal *ast_integer_literal_new(unsigned long int line,
                                                    unsigned long int column,
                                                    str value);
extern void ast_integer_literal_destroy(ast_integer_literal **this);

extern ast_boolean_literal *ast_boolean_literal_new(unsigned long int line,
                                                    unsigned long int column,
                                                    str value);
extern void ast_boolean_literal_destroy(ast_boolean_literal **this);

extern ast_decimal_literal *ast_decimal_literal_new(unsigned long int line,
                                                    unsigned long int column,
                                                    str value);
extern void ast_decimal_literal_destroy(ast_decimal_literal **this);

extern ast_null_literal *ast_null_literal_new(unsigned long int line,
                                              unsigned long int column);
extern void ast_null_literal_destroy(ast_null_literal **this);

extern ast_string_literal *ast_string_literal_new(unsigned long int line,
                                                  unsigned long int column,
                                                  str value);
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
    str operator;
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
    str operator;
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

struct ast_super_literal {
    AST_EXPRESSION_PROPERTIES
};

struct ast_integer_literal {
    AST_EXPRESSION_PROPERTIES
    str value;
};

struct ast_boolean_literal {
    AST_EXPRESSION_PROPERTIES
    str value;
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
