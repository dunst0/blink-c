//
// Created by rick on 08.03.20.
//

#ifndef BLINK_AST_NODE_H
#define BLINK_AST_NODE_H

#include <blink/str.h>


// -----------------------------------------------------------------------------
//  Types
// -----------------------------------------------------------------------------

typedef enum ast_node_type ast_node_type;
typedef struct ast_node ast_node;

typedef enum ast_definition_type ast_definition_type;
typedef struct ast_definition ast_definition;

typedef enum ast_expression_type ast_expression_type;
typedef struct ast_expression ast_expression;
typedef struct ast_expression_list_node ast_expression_list_node;
typedef struct ast_expression_list ast_expression_list;

typedef struct ast_program ast_program;

typedef struct ast_class ast_class;
typedef struct ast_class_list_node ast_class_list_node;
typedef struct ast_class_list ast_class_list;

typedef struct ast_formal ast_formal;
typedef struct ast_formal_list_node ast_formal_list_node;
typedef struct ast_formal_list ast_formal_list;

typedef struct ast_property ast_property;
typedef struct ast_property_list_node ast_property_list_node;
typedef struct ast_property_list ast_property_list;

typedef struct ast_function ast_function;
typedef struct ast_function_list_node ast_function_list_node;
typedef struct ast_function_list ast_function_list;

typedef struct ast_block ast_block;

typedef struct ast_let ast_let;

typedef struct ast_initialization ast_initialization;
typedef struct ast_initialization_list_node ast_initialization_list_node;
typedef struct ast_initialization_list ast_initialization_list;

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


// -----------------------------------------------------------------------------
// Callback types
// -----------------------------------------------------------------------------

typedef void (*ast_program_callback)(ast_program *program, void *args);
typedef void (*ast_class_callback)(ast_class *class, void *args);


// -----------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------

#define INTERFACE_LIST_NEW(type) extern type##_list *type##_list_new()

#define INTERFACE_LIST_DESTROY(type, elem)                                     \
    extern void type##_list_destroy(type##_list **this)

#define INTERFACE_LIST_PUSH(type, elem)                                        \
    extern int type##_list_push(type##_list *this, type *elem)

#define INTERFACE_LIST_POP(type, elem)                                         \
    extern type *type##_list_pop(type##_list *this)

#define INTERFACE_LIST_PREPEND(type, elem)                                     \
    extern int type##_list_prepend(type##_list *this, type *elem)

#define CREATE_LIST_TYPE(kind, type, elem)                                     \
    kind##_LIST_NEW(type);                                                     \
    kind##_LIST_DESTROY(type, elem);                                           \
    kind##_LIST_PUSH(type, elem);                                              \
    kind##_LIST_POP(type, elem);                                               \
    kind##_LIST_PREPEND(type, elem);


// -----------------------------------------------------------------------------
//  Functions
// -----------------------------------------------------------------------------

extern void ast_definition_destroy(ast_definition **this);
extern void ast_expression_destroy(ast_expression **this);

/**
 * Create a program node for the ast.
 * @param[in] classes The list of classes for the program
 * @return On success a pointer to ast_program, else NULL
 */
extern ast_program *ast_program_new(ast_class_list *classes);
/**
 * Destroy a program node and its content.
 * @param[in,out] this The program node to destroy
 */
extern void ast_program_destroy(ast_program **this);

extern ast_class *ast_class_new(unsigned long int line,
                                unsigned long int column, str name,
                                ast_formal_list *parameters, str superClass,
                                ast_expression_list *superClassArgs,
                                ast_property_list *properties,
                                ast_function_list *functions);
/**
 * Destroy a class node and its content.
 * @param[in,out] this The class node to destroy
 */
extern void ast_class_destroy(ast_class **this);

extern ast_formal *ast_formal_new(unsigned long int line,
                                  unsigned long int column, str identifier,
                                  str type, int isLazy);
extern void ast_formal_destroy(ast_formal **this);

extern ast_function *ast_function_new(unsigned long int line,
                                      unsigned long int column, str name,
                                      str returnType, ast_expression *body,
                                      int isPrivate, int isOverride);
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

CREATE_LIST_TYPE(INTERFACE, ast_class, class)
CREATE_LIST_TYPE(INTERFACE, ast_expression, expression)
CREATE_LIST_TYPE(INTERFACE, ast_formal, formal)
CREATE_LIST_TYPE(INTERFACE, ast_property, property)
CREATE_LIST_TYPE(INTERFACE, ast_function, function)
CREATE_LIST_TYPE(INTERFACE, ast_initialization, initialization)


// -----------------------------------------------------------------------------
//  Type implementations
// -----------------------------------------------------------------------------

enum ast_node_type {
    AST_NODE_EXPRESSION,
    AST_NODE_DEFINITION,
};

#define AST_NODE_PROPERTIES                                                    \
    ast_node_type astNodeType;                                                 \
    unsigned long int line;                                                    \
    unsigned long int column;

struct ast_node {
    AST_NODE_PROPERTIES
};

enum ast_definition_type {
    AST_DEFINITION_TYPE_FORMAL,
    AST_DEFINITION_TYPE_PROPERTY,
    AST_DEFINITION_TYPE_FUNCTION,
    AST_DEFINITION_TYPE_CLASS,
    AST_DEFINITION_TYPE_PROGRAM,
};

#define AST_DEFINITION_PROPERTIES                                              \
    AST_NODE_PROPERTIES                                                        \
    ast_definition_type astDefinitionType;

struct ast_definition {
    AST_DEFINITION_PROPERTIES
};

enum ast_expression_type {
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
};

#define AST_EXPRESSION_PROPERTIES                                              \
    AST_NODE_PROPERTIES                                                        \
    ast_expression_type astExpressionType;                                     \
    void *expressionType;// FIXME: not specified

struct ast_expression {
    AST_EXPRESSION_PROPERTIES
};

struct ast_expression_list_node {
    ast_expression *expression;
    ast_expression_list_node *next;
    ast_expression_list_node *prev;
};

struct ast_expression_list {
    ast_expression_list_node *head;
    ast_expression_list_node *tail;
    unsigned long int length;
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

struct ast_class_list_node {
    ast_class *class;
    ast_class_list_node *next;
    ast_class_list_node *prev;
};

struct ast_class_list {
    ast_class_list_node *head;
    ast_class_list_node *tail;
    unsigned long int length;
};

struct ast_formal {
    AST_DEFINITION_PROPERTIES
    str identifier;
    str type;
    int isLazy;
};

struct ast_formal_list_node {
    ast_formal *formal;
    ast_formal_list_node *next;
    ast_formal_list_node *prev;
};

struct ast_formal_list {
    ast_formal_list_node *head;
    ast_formal_list_node *tail;
    unsigned long int length;
};

struct ast_property {
    AST_DEFINITION_PROPERTIES
    str name;
    str type;
    ast_expression *value;
};

struct ast_property_list_node {
    ast_property *property;
    ast_property_list_node *next;
    ast_property_list_node *prev;
};

struct ast_property_list {
    ast_property_list_node *head;
    ast_property_list_node *tail;
    unsigned long int length;
};

struct ast_function {
    AST_DEFINITION_PROPERTIES
    str name;
    ast_formal_list *parameters;
    str returnType;
    ast_expression *body;
    int isPrivate;
    int isOverride;
};

struct ast_function_list_node {
    ast_function *function;
    ast_function_list_node *next;
    ast_function_list_node *prev;
};

struct ast_function_list {
    ast_function_list_node *head;
    ast_function_list_node *tail;
    unsigned long int length;
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

struct ast_initialization_list_node {
    ast_initialization *initialization;
    ast_initialization_list_node *next;
    ast_initialization_list_node *prev;
};

struct ast_initialization_list {
    ast_initialization_list_node *head;
    ast_initialization_list_node *tail;
    unsigned long int length;
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
