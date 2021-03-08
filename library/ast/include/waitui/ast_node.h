/**
 * @file ast_node.h
 * @author rick
 * @date 08.03.20
 * @brief File for the AST node implementation
 */

#ifndef WAITUI_AST_NODE_H
#define WAITUI_AST_NODE_H

#include <waitui/list.h>
#include <waitui/str.h>
#include <waitui/symbol.h>

#include <stdbool.h>


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
    AST_DEFINITION_TYPE_IMPORT,
    AST_DEFINITION_TYPE_NAMESPACE,
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
 * @brief Callback type for executing actions on an AST node.
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
 * @brief Type representing a namespace AST node.
 */
typedef struct ast_namespace ast_namespace;

/**
 * @brief Type representing a import AST node.
 */
typedef struct ast_import ast_import;

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
 * @brief Unary operator types possible for a unary expression AST node.
 */
typedef enum ast_unary_operator {
    AST_UNARY_OPERATOR_MINUS,
    AST_UNARY_OPERATOR_NOT,
    AST_UNARY_OPERATOR_DOUBLE_PLUS,
    AST_UNARY_OPERATOR_DOUBLE_MINUS,
} ast_unary_operator;

/**
 * @brief Type representing an unary expression AST node.
 */
typedef struct ast_unary_expression ast_unary_expression;

/**
 * @brief Type representing a lazy expression.
 */
typedef struct ast_lazy_expression ast_lazy_expression;

/**
 * @brief Type representing a native expression.
 */
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

CREATE_LIST_TYPE(INTERFACE, ast_namespace, namespace)
CREATE_LIST_TYPE(INTERFACE, ast_import, import)
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
#define AST_NODE_PROPERTIES ast_node_type astNodeType;

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
 * @brief Destroy a node from the AST.
 * @param[in,out] this The node to destroy
 */
extern void ast_node_destroy(ast_node **this);

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
 * @param[in] namespaces The list of namespaces for the program
 * @return On success a pointer to ast_program, else NULL
 */
extern ast_program *ast_program_new(ast_namespace_list *namespaces);

/**
 * @brief Destroy a program node and its content.
 * @param[in,out] this The program node to destroy
 */
extern void ast_program_destroy(ast_program **this);

/**
 * @brief Create a namespace node for the AST.
 * @param[in] name The namespace name
 * @param[in] imports The list of imports for the namespace
 * @param[in] classes The list of classes for the namespace
 * @return On success a pointer to ast_namespace, else NULL
 */
extern ast_namespace *ast_namespace_new(symbol *name, ast_import_list *imports,
                                        ast_class_list *classes);

/**
 * @brief Destroy a namespace node and its content.
 * @param[in,out] this The namespace node to destroy
 */
extern void ast_namespace_destroy(ast_namespace **this);

/**
 * @brief Create a import node for the AST.
 * @return On success a pointer to ast_import, else NULL
 */
extern ast_import *ast_import_new(void);

/**
 * @brief Destroy a import node and its content.
 * @param[in,out] this The import node to destroy
 */
extern void ast_import_destroy(ast_import **this);

/**
 * @brief Create a class node for the AST.
 * @param[in] name The name of the class
 * @param[in] parameters The list of the formals for the class
 * @param[in] superClass The name of the super class
 * @param[in] superClassArgs The list of the actuals for the superclass
 * @param[in] properties The list of properties for the class
 * @param[in] functions The list of function for the class
 * @return On success a pointer to ast_class, else NULL
 */
extern ast_class *ast_class_new(symbol *name, ast_formal_list *parameters,
                                symbol *superClass,
                                ast_expression_list *superClassArgs,
                                ast_property_list *properties,
                                ast_function_list *functions);

/**
 * @brief Set the name for the class node for the AST.
 * @param[in,out] this The class node to set the name
 * @param[in] name The name to set for the class node
 */
extern void ast_class_set_name(ast_class *this, symbol *name);

/**
 * @brief Set the parameters for the class node for the AST.
 * @param[in,out] this The class node to set the name
 * @param[in] parameters The parameters to set for the class node
 */
extern void ast_class_set_parameters(ast_class *this,
                                     ast_formal_list *parameters);

/**
 * @brief Set the super class for the class node for the AST.
 * @param[in,out] this The class node to set the super class
 * @param[in] superClass The super class to set for the class node
 */
extern void ast_class_set_super_class(ast_class *this, symbol *superClass);

/**
 * @brief Set the super class args for the class node for the AST.
 * @param[in,out] this The class node to set the name
 * @param[in] superClassArgs The super class args to set for the class node
 */
extern void ast_class_set_super_class_args(ast_class *this,
                                           ast_expression_list *superClassArgs);

/**
 * @brief Destroy a class node and its content.
 * @param[in,out] this The class node to destroy
 */
extern void ast_class_destroy(ast_class **this);

/**
 * @brief Create a formal node for the AST.
 * @param[in] identifier The name of the formal
 * @param[in] type The Type for the formal
 * @param[in] isLazy If this formal is a lazy one
 * @note Will steal the pointer for the identifier symbol.
 * @return On success a pointer to ast_formal, else NULL
 */
extern ast_formal *ast_formal_new(symbol *identifier, symbol *type,
                                  bool isLazy);

/**
 * @brief Destroy a formal node and its content.
 * @param[in,out] this The formal node to destroy
 */
extern void ast_formal_destroy(ast_formal **this);

/**
 * @brief Create a function node for the AST.
 * @param[in] functionName The name for the function
 * @param[in] parameters The list of the formals for the function
 * @param[in] returnType The return type for the function
 * @param[in] body The body for the function
 * @param[in] visibility The visibility for the function
 * @param[in] isAbstract If this function is abstract
 * @param[in] isFinal If this function is final
 * @param[in] isOverwrite If this function is overwrite
 * @return On success a pointer to ast_function, else NULL
 */
extern ast_function *ast_function_new(symbol *functionName,
                                      ast_formal_list *parameters,
                                      symbol *returnType, ast_expression *body,
                                      ast_function_visibility visibility,
                                      bool isAbstract, bool isFinal,
                                      bool isOverwrite);

/**
 * @brief Set the body for the function node for the AST.
 * @param[in,out] this The function node to set the body
 * @param[in] body The body for the function
 */
extern void ast_function_set_body(ast_function *this, ast_expression *body);

/**
 * @brief Set the visibility for the function node for the AST.
 * @param[in,out] this The function node to set the visibility
 * @param[in] visibility The visibility for the function
 */
extern void ast_function_set_visibility(ast_function *this,
                                        ast_function_visibility visibility);

/**
 * @brief Set the abstract for the function node for the AST.
 * @param[in,out] this The function node to set the abstract
 * @param isAbstract
 */
extern void ast_function_set_abstract(ast_function *this, bool isAbstract);

/**
 * @brief Set the final for the function node for the AST.
 * @param[in,out] this The function node to set the final
 * @param isFinal
 */
extern void ast_function_set_final(ast_function *this, bool isFinal);

/**
 * @brief Set the overwrite for the function node for the AST.
 * @param[in,out] this The function node to set the overwrite
 * @param isOverwrite
 */
extern void ast_function_set_overwrite(ast_function *this, bool isOverwrite);

/**
 * @brief Destroy a function node and its content.
 * @param[in,out] this The function node to destroy
 */
extern void ast_function_destroy(ast_function **this);

/**
 * @brief Create a property node for the AST.
 * @param[in] name The name of the property
 * @param[in] type The type of the property
 * @param[in] value The value of the property
 * @return On success a pointer to ast_function, else NULL
 */
extern ast_property *ast_property_new(symbol *name, symbol *type,
                                      ast_expression *value);

/**
 * @brief Destroy a property node and its content.
 * @param[in,out] this The property node to destroy
 */
extern void ast_property_destroy(ast_property **this);

/**
 * @brief Create a block node for the AST.
 * @param[in] expressions The list of the expressions for the block
 * @return On success a pointer to ast_block, else NULL
 */
extern ast_block *ast_block_new(ast_expression_list *expressions);

/**
 * @brief Destroy a block node and its content.
 * @param[in,out] this The block node to destroy
 */
extern void ast_block_destroy(ast_block **this);

/**
 * @brief Create a let node for the AST.
 * @param[in] initializations The list of the initializations for the let
 * @param[in] body The body for the let
 * @return On success a pointer to ast_let, else NULL
 */
extern ast_let *ast_let_new(ast_initialization_list *initializations,
                            ast_expression *body);

/**
 * @brief Destroy a let node and its content.
 * @param[in,out] this The let node to destroy
 */
extern void ast_let_destroy(ast_let **this);

/**
 * @brief Create an initialization node for the AST.
 * @param[in] identifier The identifier of the initialization
 * @param[in] type The type of the initialization
 * @param[in] value The value of the initialization
 * @return On success a pointer to ast_initialization, else NULL
 */
extern ast_initialization *
ast_initialization_new(symbol *identifier, symbol *type, ast_expression *value);

/**
 * @brief Destroy an initialization node and its content.
 * @param[in,out] this The initialization node to destroy
 */
extern void ast_initialization_destroy(ast_initialization **this);

/**
 * @brief Create an assignment node for the AST.
 * @param[in] identifier The identifier of the assignment
 * @param[in] operator The operator of the assignment
 * @param[in] value The value of the assignment
 * @return On success a pointer to ast_assignment, else NULL
 */
extern ast_assignment *ast_assignment_new(symbol *identifier,
                                          ast_assignment_operator operator,
                                          ast_expression * value);

/**
 * @brief Destroy an assignment node and its content.
 * @param[in,out] this The assignment node to destroy
 */
extern void ast_assignment_destroy(ast_assignment **this);

/**
 * @brief Create a cast node for the AST.
 * @param[in] object The object of the cast
 * @param[in] type The type of the cast
 * @return On success a pointer to ast_cast, else NULL
 */
extern ast_cast *ast_cast_new(ast_expression *object, symbol *type);

/**
 * @brief Destroy a cast node and its content.
 * @param[in,out] this The cast node to destroy
 */
extern void ast_cast_destroy(ast_cast **this);

/**
 * @brief Create a if_else node for the AST.
 * @param[in] condition The condition for the if_else node
 * @param thenBranch The thenBranch for the if_else node
 * @param elseBranch The elseBranch for the if_else node or NULL
 * @return On success a pointer to ast_if_else, else NULL
 */
extern ast_if_else *ast_if_else_new(ast_expression *condition,
                                    ast_expression *thenBranch,
                                    ast_expression *elseBranch);

/**
 * @brief Destroy a if_else node and its content.
 * @param[in,out] this The if_else node to destroy
 */
extern void ast_if_else_destroy(ast_if_else **this);

/**
 * @brief Create a while node for the AST.
 * @param[in] condition The condition for the while node
 * @param[in] body The body for the while node
 * @return On success a pointer to ast_while, else NULL
 */
extern ast_while *ast_while_new(ast_expression *condition,
                                ast_expression *body);

/**
 * @brief Destroy a while node and its content.
 * @param[in,out] this The while node to destroy
 */
extern void ast_while_destroy(ast_while **this);

/**
 * @brief TODO
 * @param left
 * @param operator
 * @param right
 * @return
 */
extern ast_binary_expression *
ast_binary_expression_new(ast_expression *left, ast_binary_operator operator,
                          ast_expression * right);

/**
 * @brief TODO
 * @param this
 */
extern void ast_binary_expression_destroy(ast_binary_expression **this);

/**
 * @brief TODO
 * @param operator
 * @param expression
 * @return
 */
extern ast_unary_expression *
        ast_unary_expression_new(ast_unary_operator operator,
                                 ast_expression * expression);

/**
 * @brief TODO
 * @param this
 */
extern void ast_unary_expression_destroy(ast_unary_expression **this);

/**
 * @brief TODO
 * @param expression
 * @param context
 * @return
 */
extern ast_lazy_expression *ast_lazy_expression_new(ast_expression *expression,
                                                    void *context);

/**
 * @brief TODO
 * @param this
 */
extern void ast_lazy_expression_destroy(ast_lazy_expression **this);

/**
 * @brief TODO
 * @param func
 * @return
 */
extern ast_native_expression *ast_native_expression_new(void *func);

/**
 * @brief TODO
 * @param this
 */
extern void ast_native_expression_destroy(ast_native_expression **this);

/**
 * @brief TODO
 * @param name
 * @param args
 * @return
 */
extern ast_constructor_call *
ast_constructor_call_new(symbol *name, ast_expression_list *args);

/**
 * @brief TODO
 * @param this
 */
extern void ast_constructor_call_destroy(ast_constructor_call **this);

/**
 * @brief TODO
 * @param object
 * @param functionName
 * @param args
 * @return
 */
extern ast_function_call *ast_function_call_new(ast_expression *object,
                                                symbol *functionName,
                                                ast_expression_list *args);

/**
 * @brief TODO
 * @param this
 */
extern void ast_function_call_destroy(ast_function_call **this);

/**
 * @brief TODO
 * @param functionName
 * @param args
 * @return
 */
extern ast_super_function_call *
ast_super_function_call_new(symbol *functionName, ast_expression_list *args);

/**
 * @brief TODO
 * @param this
 */
extern void ast_super_function_call_destroy(ast_super_function_call **this);

/**
 * @brief Create a reference node for the AST.
 * @param[in] identifier The identifier to use for the reference node
 * @return On success a pointer to ast_reference, else NULL
 */
extern ast_reference *ast_reference_new(symbol *value);

/**
 * @brief Destroy a reference node and its content.
 * @param[in,out] this The reference node to destroy
 */
extern void ast_reference_destroy(ast_reference **this);

/**
 * @brief Create a this literal node for the AST.
 * @return On success a pointer to ast_this_literal, else NULL
 */
extern ast_this_literal *ast_this_literal_new(void);

/**
 * @brief Destroy a this literal node and its content.
 * @param[in,out] this The this literal node to destroy
 */
extern void ast_this_literal_destroy(ast_this_literal **this);

/**
 * @brief Create a integer literal node for the AST.
 * @param[in] value The value for the integer literal node
 * @return On success a pointer to ast_integer_literal, else NULL
 */
extern ast_integer_literal *ast_integer_literal_new(str value);

/**
 * @brief Destroy a integer literal node and its content.
 * @param[in,out] this The integer literal node to destroy
 */
extern void ast_integer_literal_destroy(ast_integer_literal **this);

/**
 * @brief Create a boolean literal node for the AST.
 * @param[in] value The value for the boolean literal node
 * @return On success a pointer to ast_boolean_literal, else NULL
 */
extern ast_boolean_literal *ast_boolean_literal_new(bool value);

/**
 * @brief Destroy a boolean literal node and its content.
 * @param[in,out] this The boolean literal node to destroy
 */
extern void ast_boolean_literal_destroy(ast_boolean_literal **this);

/**
 * @brief Create a decimal literal node for the AST.
 * @param[in] value The value for the decimal literal node
 * @return On success a pointer to ast_decimal_literal, else NULL
 */
extern ast_decimal_literal *ast_decimal_literal_new(str value);

/**
 * @brief Destroy a decimal literal node and its content.
 * @param[in,out] this The decimal literal node to destroy
 */
extern void ast_decimal_literal_destroy(ast_decimal_literal **this);

/**
 * @brief Create a null literal node for the AST.
 * @return On success a pointer to ast_null_literal, else NULL
 */
extern ast_null_literal *ast_null_literal_new(void);

/**
 * @brief Destroy a null literal node and its content.
 * @param[in,out] this The null literal node to destroy
 */
extern void ast_null_literal_destroy(ast_null_literal **this);

/**
 * @brief Create a string literal node for the AST.
 * @param[in] value The value for the string literal node
 * @return On success a pointer to ast_string_literal, else NULL
 */
extern ast_string_literal *ast_string_literal_new(str value);

/**
 * @brief Destroy a string literal node and its content.
 * @param[in,out] this The string literal node to destroy
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
    ast_namespace_list *namespaces;
};

struct ast_namespace {
    AST_DEFINITION_PROPERTIES
    symbol *name;
    ast_import_list *imports;
    ast_class_list *classes;
};

struct ast_import {
    AST_DEFINITION_PROPERTIES
};

struct ast_class {
    AST_DEFINITION_PROPERTIES
    symbol *name;
    ast_formal_list *parameters;
    symbol *superClass;
    ast_expression_list *superClassArgs;
    ast_property_list *properties;
    ast_function_list *functions;
};

struct ast_formal {
    AST_DEFINITION_PROPERTIES
    symbol *identifier;
    symbol *type;
    bool isLazy;
};

struct ast_property {
    AST_DEFINITION_PROPERTIES
    symbol *name;
    symbol *type;
    ast_expression *value;
};

struct ast_function {
    AST_DEFINITION_PROPERTIES
    symbol *functionName;
    ast_formal_list *parameters;
    symbol *returnType;
    ast_expression *body;
    ast_function_visibility visibility;
    bool isAbstract;
    bool isFinal;
    bool isOverwrite;
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
    symbol *identifier;
    symbol *type;
    ast_expression *value;
};

struct ast_assignment {
    AST_EXPRESSION_PROPERTIES
    symbol *identifier;
    ast_assignment_operator operator;
    ast_expression *value;
};

struct ast_cast {
    AST_EXPRESSION_PROPERTIES
    symbol *type;
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
    ast_unary_operator operator;
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
    symbol *name;
    ast_expression_list *args;
};

struct ast_function_call {
    AST_EXPRESSION_PROPERTIES
    ast_expression *object;
    symbol *functionName;
    ast_expression_list *args;
};

struct ast_super_function_call {
    AST_EXPRESSION_PROPERTIES
    symbol *functionName;
    ast_expression_list *args;
};

struct ast_reference {
    AST_EXPRESSION_PROPERTIES
    symbol *value;
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
    bool value;
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

#endif// WAITUI_AST_NODE_H
