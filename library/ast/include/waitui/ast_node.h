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
typedef enum waitui_ast_node_type {
    WAITUI_AST_NODE_TYPE_UNDEFINED,
    WAITUI_AST_NODE_TYPE_EXPRESSION,
    WAITUI_AST_NODE_TYPE_DEFINITION,
} waitui_ast_node_type;

/**
 * @brief Definition node types possible for a definition AST node.
 */
typedef enum waitui_ast_definition_type {
    WAITUI_AST_DEFINITION_TYPE_UNDEFINED,
    WAITUI_AST_DEFINITION_TYPE_FORMAL,
    WAITUI_AST_DEFINITION_TYPE_PROPERTY,
    WAITUI_AST_DEFINITION_TYPE_FUNCTION,
    WAITUI_AST_DEFINITION_TYPE_CLASS,
    WAITUI_AST_DEFINITION_TYPE_IMPORT,
    WAITUI_AST_DEFINITION_TYPE_NAMESPACE,
    WAITUI_AST_DEFINITION_TYPE_PROGRAM,
} waitui_ast_definition_type;

/**
 * @brief Expression node types possible for an expression AST node.
 */
typedef enum waitui_ast_expression_type {
    WAITUI_AST_EXPRESSION_TYPE_UNDEFINED,
    WAITUI_AST_EXPRESSION_TYPE_INTEGER_LITERAL,
    WAITUI_AST_EXPRESSION_TYPE_BOOLEAN_LITERAL,
    WAITUI_AST_EXPRESSION_TYPE_DECIMAL_LITERAL,
    WAITUI_AST_EXPRESSION_TYPE_NULL_LITERAL,
    WAITUI_AST_EXPRESSION_TYPE_STRING_LITERAL,
    WAITUI_AST_EXPRESSION_TYPE_THIS_LITERAL,
    WAITUI_AST_EXPRESSION_TYPE_ASSIGNMENT,
    WAITUI_AST_EXPRESSION_TYPE_REFERENCE,
    WAITUI_AST_EXPRESSION_TYPE_CAST,
    WAITUI_AST_EXPRESSION_TYPE_INITIALIZATION,
    WAITUI_AST_EXPRESSION_TYPE_LET,
    WAITUI_AST_EXPRESSION_TYPE_BLOCK,
    WAITUI_AST_EXPRESSION_TYPE_CONSTRUCTOR_CALL,
    WAITUI_AST_EXPRESSION_TYPE_FUNCTION_CALL,
    WAITUI_AST_EXPRESSION_TYPE_SUPER_FUNCTION_CALL,
    WAITUI_AST_EXPRESSION_TYPE_BINARY_EXPRESSION,
    WAITUI_AST_EXPRESSION_TYPE_UNARY_EXPRESSION,
    WAITUI_AST_EXPRESSION_TYPE_IF_ELSE,
    WAITUI_AST_EXPRESSION_TYPE_WHILE,
    WAITUI_AST_EXPRESSION_TYPE_LAZY_EXPRESSION,
    WAITUI_AST_EXPRESSION_TYPE_NATIVE_EXPRESSION,
} waitui_ast_expression_type;

/**
 * @brief Type representing an AST node.
 */
typedef struct waitui_ast_node waitui_ast_node;

/**
 * @brief Callback type for executing actions on an AST node.
 */
typedef void (*waitui_ast_node_callback)(waitui_ast_node *node, void *args);

/**
 * @brief Type representing a definition AST node.
 */
typedef struct waitui_ast_definition waitui_ast_definition;

/**
 * @brief Type representing an expression AST node.
 */
typedef struct waitui_ast_expression waitui_ast_expression;

/**
 * @brief Type representing a program AST node.
 */
typedef struct waitui_ast_program waitui_ast_program;

/**
 * @brief Type representing a namespace AST node.
 */
typedef struct waitui_ast_namespace waitui_ast_namespace;

/**
 * @brief Type representing a import AST node.
 */
typedef struct waitui_ast_import waitui_ast_import;

/**
 * @brief Type representing a class AST node.
 */
typedef struct waitui_ast_class waitui_ast_class;

/**
 * @brief Type representing a formal AST node.
 */
typedef struct waitui_ast_formal waitui_ast_formal;

/**
 * @brief Type representing a property AST node.
 */
typedef struct waitui_ast_property waitui_ast_property;

/**
 * @brief Function visibility types possible for a function AST node.
 */
typedef enum waitui_ast_function_visibility {
    WAITUI_AST_FUNCTION_VISIBILITY_UNDEFINED,
    WAITUI_AST_FUNCTION_VISIBILITY_PUBLIC,
    WAITUI_AST_FUNCTION_VISIBILITY_PROTECTED,
    WAITUI_AST_FUNCTION_VISIBILITY_PRIVATE,
} waitui_ast_function_visibility;

/**
 * @brief Type representing a function AST node.
 */
typedef struct waitui_ast_function waitui_ast_function;

/**
 * @brief Type representing a block AST node.
 */
typedef struct waitui_ast_block waitui_ast_block;

/**
 * @brief Type representing a let AST node.
 */
typedef struct waitui_ast_let waitui_ast_let;

/**
 * @brief Type representing an initialization AST node.
 */
typedef struct waitui_ast_initialization waitui_ast_initialization;

/**
 * @brief Assignment operator types possible for an assignment AST node.
 */
typedef enum waitui_ast_assignment_operator {
    WAITUI_AST_ASSIGNMENT_OPERATOR_UNDEFINED,
    WAITUI_AST_ASSIGNMENT_OPERATOR_EQUAL,
    WAITUI_AST_ASSIGNMENT_OPERATOR_PLUS_EQUAL,
    WAITUI_AST_ASSIGNMENT_OPERATOR_MINUS_EQUAL,
    WAITUI_AST_ASSIGNMENT_OPERATOR_TIMES_EQUAL,
    WAITUI_AST_ASSIGNMENT_OPERATOR_DIV_EQUAL,
    WAITUI_AST_ASSIGNMENT_OPERATOR_MODULO_EQUAL,
    WAITUI_AST_ASSIGNMENT_OPERATOR_AND_EQUAL,
    WAITUI_AST_ASSIGNMENT_OPERATOR_CARET_EQUAL,
    WAITUI_AST_ASSIGNMENT_OPERATOR_TILDE_EQUAL,
    WAITUI_AST_ASSIGNMENT_OPERATOR_PIPE_EQUAL,
} waitui_ast_assignment_operator;

/**
 * @brief Type representing an assignment AST node.
 */
typedef struct waitui_ast_assignment waitui_ast_assignment;

/**
 * @brief Type representing a cast AST node.
 */
typedef struct waitui_ast_cast waitui_ast_cast;

/**
 * @brief Type representing an if else AST node.
 */
typedef struct waitui_ast_if_else waitui_ast_if_else;

/**
 * @brief Type representing a while AST node.
 */
typedef struct waitui_ast_while waitui_ast_while;

/**
 * @brief Binary operator types possible for a binary expression AST node.
 */
typedef enum waitui_ast_binary_operator {
    WAITUI_AST_BINARY_OPERATOR_UNDEFINED,
    WAITUI_AST_BINARY_OPERATOR_PLUS,
    WAITUI_AST_BINARY_OPERATOR_MINUS,
    WAITUI_AST_BINARY_OPERATOR_TIMES,
    WAITUI_AST_BINARY_OPERATOR_DIV,
    WAITUI_AST_BINARY_OPERATOR_MODULO,
    WAITUI_AST_BINARY_OPERATOR_AND,
    WAITUI_AST_BINARY_OPERATOR_CARET,
    WAITUI_AST_BINARY_OPERATOR_TILDE,
    WAITUI_AST_BINARY_OPERATOR_PIPE,
    WAITUI_AST_BINARY_OPERATOR_LESS,
    WAITUI_AST_BINARY_OPERATOR_LESS_EQUAL,
    WAITUI_AST_BINARY_OPERATOR_GREATER,
    WAITUI_AST_BINARY_OPERATOR_GREATER_EQUAL,
    WAITUI_AST_BINARY_OPERATOR_EQUAL,
    WAITUI_AST_BINARY_OPERATOR_NOT_EQUAL,
    WAITUI_AST_BINARY_OPERATOR_DOUBLE_AND,
    WAITUI_AST_BINARY_OPERATOR_DOUBLE_PIPE,
} waitui_ast_binary_operator;

/**
 * @brief Type representing a binary expression AST node.
 */
typedef struct waitui_ast_binary_expression waitui_ast_binary_expression;

/**
 * @brief Unary operator types possible for a unary expression AST node.
 */
typedef enum waitui_ast_unary_operator {
    WAITUI_AST_UNARY_OPERATOR_UNDEFINED,
    WAITUI_AST_UNARY_OPERATOR_MINUS,
    WAITUI_AST_UNARY_OPERATOR_NOT,
    WAITUI_AST_UNARY_OPERATOR_DOUBLE_PLUS,
    WAITUI_AST_UNARY_OPERATOR_DOUBLE_MINUS,
} waitui_ast_unary_operator;

/**
 * @brief Type representing an unary expression AST node.
 */
typedef struct waitui_ast_unary_expression waitui_ast_unary_expression;

/**
 * @brief Type representing a lazy expression.
 */
typedef struct waitui_ast_lazy_expression waitui_ast_lazy_expression;

/**
 * @brief Type representing a native expression.
 */
typedef struct waitui_ast_native_expression waitui_ast_native_expression;

/**
 * @brief Type representing a constructor call AST node.
 */
typedef struct waitui_ast_constructor_call waitui_ast_constructor_call;

/**
 * @brief Type representing a function call AST node.
 */
typedef struct waitui_ast_function_call waitui_ast_function_call;

/**
 * @brief Type representing a super function call AST node.
 */
typedef struct waitui_ast_super_function_call waitui_ast_super_function_call;

/**
 * @brief Type representing a reference AST node.
 */
typedef struct waitui_ast_reference waitui_ast_reference;

/**
 * @brief Type representing a this literal AST node.
 */
typedef struct waitui_ast_this_literal waitui_ast_this_literal;

/**
 * @brief Type representing an integer literal AST node.
 */
typedef struct waitui_ast_integer_literal waitui_ast_integer_literal;

/**
 * @brief Type representing a boolean literal AST node.
 */
typedef struct waitui_ast_boolean_literal waitui_ast_boolean_literal;

/**
 * @brief Type representing a decimal literal AST node.
 */
typedef struct waitui_ast_decimal_literal waitui_ast_decimal_literal;

/**
 * @brief Type representing a null literal AST node.
 */
typedef struct waitui_ast_null_literal waitui_ast_null_literal;

/**
 * @brief Type representing a string literal AST node.
 */
typedef struct waitui_ast_string_literal waitui_ast_string_literal;

CREATE_LIST_TYPE(INTERFACE, waitui_ast_namespace, namespace)
CREATE_LIST_TYPE(INTERFACE, waitui_ast_import, import)
CREATE_LIST_TYPE(INTERFACE, waitui_ast_class, class)
CREATE_LIST_TYPE(INTERFACE, waitui_ast_expression, expression)
CREATE_LIST_TYPE(INTERFACE, waitui_ast_formal, formal)
CREATE_LIST_TYPE(INTERFACE, waitui_ast_property, property)
CREATE_LIST_TYPE(INTERFACE, waitui_ast_function, function)
CREATE_LIST_TYPE(INTERFACE, waitui_ast_initialization, initialization)


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Return the node type from the node.
 * @param[in] this The node to get the node type from
 * @return The node type from the node
 */
extern waitui_ast_node_type
waitui_ast_node_getNodeType(const waitui_ast_node *this);

/**
 * @brief Destroy a node from the AST.
 * @param[in,out] this The node to destroy
 */
extern void waitui_ast_node_destroy(waitui_ast_node **this);

/**
 * @brief Return the definition type from the definition.
 * @param[in] this The definition to get the definition type from
 * @return The definition type from the definition
 */
extern waitui_ast_definition_type
waitui_ast_definition_getDefinitionType(const waitui_ast_definition *this);

/**
 * @brief Destroy a definition node from the AST.
 * @param[in,out] this The definition node to destroy
 */
extern void waitui_ast_definition_destroy(waitui_ast_definition **this);

/**
 * @brief Return the expression type from the expression.
 * @param[in] this The expression to get the expression type from
 * @return The expression type from the expression
 */
extern waitui_ast_expression_type
waitui_ast_expression_getExpressionType(const waitui_ast_expression *this);

/**
 * @brief Destroy a expression node from the AST.
 * @param[in,out] this The expression node to destroy
 */
extern void waitui_ast_expression_destroy(waitui_ast_expression **this);

/**
 * @brief Create a program node for the AST.
 * @param[in] namespaces The list of namespaces for the program
 * @return On success a pointer to waitui_ast_program, else NULL
 */
extern waitui_ast_program *
waitui_ast_program_new(waitui_ast_namespace_list *namespaces);

/**
 * @brief Return the namespaces for the program node.
 * @param[in] this The program node to get the namespaces from
 * @return A pointer to waitui_ast_namespace_list, else NULL
 */
extern waitui_ast_namespace_list *
waitui_ast_program_getNamespaces(waitui_ast_program *this);

/**
 * @brief Destroy a program node and its content.
 * @param[in,out] this The program node to destroy
 */
extern void waitui_ast_program_destroy(waitui_ast_program **this);

/**
 * @brief Create a namespace node for the AST.
 * @param[in] name The namespace name
 * @param[in] imports The list of imports for the namespace
 * @param[in] classes The list of classes for the namespace
 * @return On success a pointer to waitui_ast_namespace, else NULL
 */
extern waitui_ast_namespace *
waitui_ast_namespace_new(symbol *name, waitui_ast_import_list *imports,
                         waitui_ast_class_list *classes);

/**
 * @brief Return the name for the namespace node.
 * @param[in] this The namespace node to get the name from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_namespace_getName(waitui_ast_namespace *this);

/**
 * @brief Return the imports for the namespace node.
 * @param[in] this The namespace node to get the imports from
 * @return A pointer to waitui_ast_import_list, else NULL
 */
extern waitui_ast_import_list *
waitui_ast_namespace_getImports(waitui_ast_namespace *this);

/**
 * @brief Return the classes for the namespace node.
 * @param[in] this The namespace node to get the classes from
 * @return A pointer to waitui_ast_class_list, else NULL
 */
extern waitui_ast_class_list *
waitui_ast_namespace_getClasses(waitui_ast_namespace *this);

/**
 * @brief Destroy a namespace node and its content.
 * @param[in,out] this The namespace node to destroy
 */
extern void waitui_ast_namespace_destroy(waitui_ast_namespace **this);

/**
 * @brief Create a import node for the AST.
 * @return On success a pointer to waitui_ast_import, else NULL
 */
extern waitui_ast_import *waitui_ast_import_new(void);

/**
 * @brief Destroy a import node and its content.
 * @param[in,out] this The import node to destroy
 */
extern void waitui_ast_import_destroy(waitui_ast_import **this);

/**
 * @brief Create a class node for the AST.
 * @param[in] name The name of the class
 * @param[in] parameters The list of the formals for the class
 * @param[in] superClass The name of the super class
 * @param[in] superClassArgs The list of the actuals for the superclass
 * @param[in] properties The list of properties for the class
 * @param[in] functions The list of function for the class
 * @return On success a pointer to waitui_ast_class, else NULL
 */
extern waitui_ast_class *
waitui_ast_class_new(symbol *name, waitui_ast_formal_list *parameters,
                     symbol *superClass,
                     waitui_ast_expression_list *superClassArgs,
                     waitui_ast_property_list *properties,
                     waitui_ast_function_list *functions);

/**
 * @brief Return the name for the class node.
 * @param[in] this The class node to get the name from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_class_getName(waitui_ast_class *this);

/**
 * @brief Set the name for the class node for the AST.
 * @param[in,out] this The class node to set the name
 * @param[in] name The name to set for the class node
 */
extern void waitui_ast_class_setName(waitui_ast_class *this, symbol *name);

/**
 * @brief Return the parameters for the class node.
 * @param[in] this The class node to get the parameters from
 * @return A pointer to waitui_ast_formal_list, else NULL
 */
extern waitui_ast_formal_list *
waitui_ast_class_getParameters(waitui_ast_class *this);

/**
 * @brief Set the parameters for the class node for the AST.
 * @param[in,out] this The class node to set the name
 * @param[in] parameters The parameters to set for the class node
 */
extern void waitui_ast_class_setParameters(waitui_ast_class *this,
                                           waitui_ast_formal_list *parameters);

/**
 * @brief Return the superClass for the class node.
 * @param[in] this The class node to get the superClass from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_class_getSuperClass(waitui_ast_class *this);

/**
 * @brief Set the super class for the class node for the AST.
 * @param[in,out] this The class node to set the super class
 * @param[in] superClass The super class to set for the class node
 */
extern void waitui_ast_class_setSuperClass(waitui_ast_class *this,
                                           symbol *superClass);

/**
 * @brief Return the superClassArgs for the class node.
 * @param[in] this The class node to get the superClassArgs from
 * @return A pointer to waitui_ast_expression_list, else NULL
 */
extern waitui_ast_expression_list *
waitui_ast_class_getSuperClassArgs(waitui_ast_class *this);

/**
 * @brief Set the super class args for the class node for the AST.
 * @param[in,out] this The class node to set the name
 * @param[in] superClassArgs The super class args to set for the class node
 */
extern void
waitui_ast_class_setSuperClassArgs(waitui_ast_class *this,
                                   waitui_ast_expression_list *superClassArgs);

/**
 * @brief Return the properties for the class node.
 * @param[in] this The class node to get the properties from
 * @return A pointer to waitui_ast_property_list, else NULL
 */
extern waitui_ast_property_list *
waitui_ast_class_getProperties(waitui_ast_class *this);

/**
 * @brief Return the functions for the class node.
 * @param[in] this The class node to get the functions from
 * @return A pointer to waitui_ast_function_list, else NULL
 */
extern waitui_ast_function_list *
waitui_ast_class_getFunctions(waitui_ast_class *this);

/**
 * @brief Destroy a class node and its content.
 * @param[in,out] this The class node to destroy
 */
extern void waitui_ast_class_destroy(waitui_ast_class **this);

/**
 * @brief Create a formal node for the AST.
 * @param[in] identifier The name of the formal
 * @param[in] type The Type for the formal
 * @param[in] isLazy If this formal is a lazy one
 * @note Will steal the pointer for the identifier symbol.
 * @return On success a pointer to waitui_ast_formal, else NULL
 */
extern waitui_ast_formal *waitui_ast_formal_new(symbol *identifier,
                                                symbol *type, bool isLazy);

/**
 * @brief Return the identifier for the formal node.
 * @param[in] this The formal node to get the identifier from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_formal_getIdentifier(waitui_ast_formal *this);

/**
 * @brief Return the type for the formal node.
 * @param[in] this The formal node to get the type from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_formal_getType(waitui_ast_formal *this);

/**
 * @brief Return the isLazy for the formal node.
 * @param[in] this The formal node to get the isLazy from
 * @retval true  The formal is lazy
 * @retval false The formal is not lazy
 */
extern bool waitui_ast_formal_isLazy(waitui_ast_formal *this);

/**
 * @brief Destroy a formal node and its content.
 * @param[in,out] this The formal node to destroy
 */
extern void waitui_ast_formal_destroy(waitui_ast_formal **this);

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
 * @return On success a pointer to waitui_ast_function, else NULL
 */
extern waitui_ast_function *
waitui_ast_function_new(symbol *functionName,
                        waitui_ast_formal_list *parameters, symbol *returnType,
                        waitui_ast_expression *body,
                        waitui_ast_function_visibility visibility,
                        bool isAbstract, bool isFinal, bool isOverwrite);

/**
 * @brief Get the functionName for the function node for the AST.
 * @param[in] this The function node to get the functionName from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_function_getFunctionName(waitui_ast_function *this);

/**
 * @brief Get the parameters for the function node for the AST.
 * @param[in] this The function node to get the parameters from
 * @return A pointer to symbol, else NULL
 */
extern waitui_ast_formal_list *
waitui_ast_function_getParameters(waitui_ast_function *this);

/**
 * @brief Get the returnType for the function node for the AST.
 * @param[in] this The function node to get the returnType from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_function_getReturnType(waitui_ast_function *this);

/**
 * @brief Get the body for the function node for the AST.
 * @param[in] this The function node to get the body from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_function_getBody(waitui_ast_function *this);

/**
 * @brief Set the body for the function node for the AST.
 * @param[in,out] this The function node to set the body
 * @param[in] body The body for the function
 */
extern void waitui_ast_function_setBody(waitui_ast_function *this,
                                        waitui_ast_expression *body);

/**
 * @brief Get the visibility for the function node for the AST.
 * @param[in] this The function node to get the visibility from
 * @return The visibility for the function node
 */
extern waitui_ast_function_visibility
waitui_ast_function_getVisibility(waitui_ast_function *this);

/**
 * @brief Set the visibility for the function node for the AST.
 * @param[in,out] this The function node to set the visibility
 * @param[in] visibility The visibility for the function
 */
extern void
waitui_ast_function_setVisibility(waitui_ast_function *this,
                                  waitui_ast_function_visibility visibility);

/**
 * @brief Get the isAbstract for the function node for the AST.
 * @param[in] this The function node to get the isAbstract from
 * @retval true  The function is abstract
 * @retval false The function is not abstract
 */
extern bool waitui_ast_function_isAbstract(waitui_ast_function *this);

/**
 * @brief Set the abstract for the function node for the AST.
 * @param[in,out] this The function node to set the abstract
 * @param isAbstract
 */
extern void waitui_ast_function_setAbstract(waitui_ast_function *this,
                                            bool isAbstract);

/**
 * @brief Get the isFinal for the function node for the AST.
 * @param[in] this The function node to get the isFinal from
 * @retval true  The function is final
 * @retval false The function is not final
 */
extern bool waitui_ast_function_isFinal(waitui_ast_function *this);

/**
 * @brief Set the final for the function node for the AST.
 * @param[in,out] this The function node to set the final
 * @param isFinal
 */
extern void waitui_ast_function_setFinal(waitui_ast_function *this,
                                         bool isFinal);

/**
 * @brief Get the isOverwrite for the function node for the AST.
 * @param[in] this The function node to get the isOverwrite from
 * @retval true  The function is overwrite
 * @retval false The function is not overwrite
 */
extern bool waitui_ast_function_isOverwrite(waitui_ast_function *this);

/**
 * @brief Set the overwrite for the function node for the AST.
 * @param[in,out] this The function node to set the overwrite
 * @param isOverwrite
 */
extern void waitui_ast_function_setOverwrite(waitui_ast_function *this,
                                             bool isOverwrite);

/**
 * @brief Destroy a function node and its content.
 * @param[in,out] this The function node to destroy
 */
extern void waitui_ast_function_destroy(waitui_ast_function **this);

/**
 * @brief Create a property node for the AST.
 * @param[in] name The name of the property
 * @param[in] type The type of the property
 * @param[in] value The value of the property
 * @return On success a pointer to waitui_ast_function, else NULL
 */
extern waitui_ast_property *
waitui_ast_property_new(symbol *name, symbol *type,
                        waitui_ast_expression *value);

/**
 * @brief Return the name for the property node.
 * @param[in] this The property node to get the name from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_property_getName(waitui_ast_property *this);

/**
 * @brief Return the type for the property node.
 * @param[in] this The property node to get the type from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_property_getType(waitui_ast_property *this);

/**
 * @brief Return the value for the property node.
 * @param[in] this The property node to get the value from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_property_getValue(waitui_ast_property *this);

/**
 * @brief Destroy a property node and its content.
 * @param[in,out] this The property node to destroy
 */
extern void waitui_ast_property_destroy(waitui_ast_property **this);

/**
 * @brief Create a block node for the AST.
 * @param[in] expressions The list of the expressions for the block
 * @return On success a pointer to waitui_ast_block, else NULL
 */
extern waitui_ast_block *
waitui_ast_block_new(waitui_ast_expression_list *expressions);

/**
 * @brief Return the expressions for the block node.
 * @param[in] this The block node to get the expressions from
 * @return A pointer to waitui_ast_expression_list, else NULL
 */
extern waitui_ast_expression_list *
waitui_ast_block_getExpressions(waitui_ast_block *this);

/**
 * @brief Destroy a block node and its content.
 * @param[in,out] this The block node to destroy
 */
extern void waitui_ast_block_destroy(waitui_ast_block **this);

/**
 * @brief Create a let node for the AST.
 * @param[in] initializations The list of the initializations for the let
 * @param[in] body The body for the let
 * @return On success a pointer to waitui_ast_let, else NULL
 */
extern waitui_ast_let *
waitui_ast_let_new(waitui_ast_initialization_list *initializations,
                   waitui_ast_expression *body);

/**
 * @brief Return the initializations for the let node.
 * @param[in] this The let node to get the initializations from
 * @return A pointer to waitui_ast_initialization_list, else NULL
 */
extern waitui_ast_initialization_list *
waitui_ast_let_getInitializations(waitui_ast_let *this);

/**
 * @brief Return the body for the let node.
 * @param[in] this The let node to get the body from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *waitui_ast_let_getBody(waitui_ast_let *this);

/**
 * @brief Destroy a let node and its content.
 * @param[in,out] this The let node to destroy
 */
extern void waitui_ast_let_destroy(waitui_ast_let **this);

/**
 * @brief Create an initialization node for the AST.
 * @param[in] identifier The identifier of the initialization
 * @param[in] type The type of the initialization
 * @param[in] value The value of the initialization
 * @return On success a pointer to waitui_ast_initialization, else NULL
 */
extern waitui_ast_initialization *
waitui_ast_initialization_new(symbol *identifier, symbol *type,
                              waitui_ast_expression *value);

/**
 * @brief Get the identifier for the initialization node for the AST.
 * @param[in] this The initialization node to get the identifier from
 * @return A pointer to symbol, else NULL
 */
extern symbol *
waitui_ast_initialization_getIdentifier(waitui_ast_initialization *this);

/**
 * @brief Get the type for the initialization node for the AST.
 * @param[in] this The initialization node to get the type from
 * @return A pointer to symbol, else NULL
 */
extern symbol *
waitui_ast_initialization_getType(waitui_ast_initialization *this);

/**
 * @brief Get the value for the initialization node for the AST.
 * @param[in] this The initialization node to get the value from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_initialization_getValue(waitui_ast_initialization *this);

/**
 * @brief Destroy an initialization node and its content.
 * @param[in,out] this The initialization node to destroy
 */
extern void waitui_ast_initialization_destroy(waitui_ast_initialization **this);

/**
 * @brief Create an assignment node for the AST.
 * @param[in] identifier The identifier of the assignment
 * @param[in] operator The operator of the assignment
 * @param[in] value The value of the assignment
 * @return On success a pointer to waitui_ast_assignment, else NULL
 */
extern waitui_ast_assignment *
waitui_ast_assignment_new(symbol *identifier,
                          waitui_ast_assignment_operator operator,
                          waitui_ast_expression * value);

/**
 * @brief Get the identifier for the assignment node for the AST.
 * @param[in] this The assignment node to get the identifier from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_assignment_getIdentifier(waitui_ast_assignment *this);

/**
 * @brief Get the operator for the assignment node for the AST.
 * @param[in] this The assignment node to get the operator from
 * @return The operator used with this assignment node
 */
extern waitui_ast_assignment_operator
waitui_ast_assignment_getOperator(waitui_ast_assignment *this);

/**
 * @brief Get the value for the assignment node for the AST.
 * @param[in] this The assignment node to get the value from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_assignment_getValue(waitui_ast_assignment *this);

/**
 * @brief Destroy an assignment node and its content.
 * @param[in,out] this The assignment node to destroy
 */
extern void waitui_ast_assignment_destroy(waitui_ast_assignment **this);

/**
 * @brief Create a cast node for the AST.
 * @param[in] object The object of the cast
 * @param[in] type The type of the cast
 * @return On success a pointer to waitui_ast_cast, else NULL
 */
extern waitui_ast_cast *waitui_ast_cast_new(waitui_ast_expression *object,
                                            symbol *type);

/**
 * @brief Get the object for the cast node for the AST.
 * @param[in] this The cast node to get the object from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *waitui_ast_cast_getObject(waitui_ast_cast *this);

/**
 * @brief Get the type for the cast node for the AST.
 * @param[in] this The cast node to get the type from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_cast_getType(waitui_ast_cast *this);

/**
 * @brief Destroy a cast node and its content.
 * @param[in,out] this The cast node to destroy
 */
extern void waitui_ast_cast_destroy(waitui_ast_cast **this);

/**
 * @brief Create a if_else node for the AST.
 * @param[in] condition The condition for the if_else node
 * @param thenBranch The thenBranch for the if_else node
 * @param elseBranch The elseBranch for the if_else node or NULL
 * @return On success a pointer to waitui_ast_if_else, else NULL
 */
extern waitui_ast_if_else *
waitui_ast_if_else_new(waitui_ast_expression *condition,
                       waitui_ast_expression *thenBranch,
                       waitui_ast_expression *elseBranch);

/**
 * @brief Get the condition expression for the if else node for the AST.
 * @param[in] this The if else node to get the condition expression from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_if_else_getCondition(waitui_ast_if_else *this);

/**
 * @brief Get the then expression for the if else node for the AST.
 * @param[in] this The if else node to get the then expression from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_if_else_getThenBranch(waitui_ast_if_else *this);

/**
 * @brief Get the else expression for the if else node for the AST.
 * @param[in] this The if else node to get the else expression from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_if_else_getElseBranch(waitui_ast_if_else *this);

/**
 * @brief Destroy a if_else node and its content.
 * @param[in,out] this The if_else node to destroy
 */
extern void waitui_ast_if_else_destroy(waitui_ast_if_else **this);

/**
 * @brief Create a while node for the AST.
 * @param[in] condition The condition for the while node
 * @param[in] body The body for the while node
 * @return On success a pointer to waitui_ast_while, else NULL
 */
extern waitui_ast_while *waitui_ast_while_new(waitui_ast_expression *condition,
                                              waitui_ast_expression *body);

/**
 * @brief Get the condition expression for the while node for the AST.
 * @param[in] this The while node to get the condition expression from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_while_getCondition(waitui_ast_while *this);

/**
 * @brief Get the body for the while node for the AST.
 * @param[in] this The while node to get the body from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *waitui_ast_while_getBody(waitui_ast_while *this);

/**
 * @brief Destroy a while node and its content.
 * @param[in,out] this The while node to destroy
 */
extern void waitui_ast_while_destroy(waitui_ast_while **this);

/**
 * @brief Create a binary expression node for the AST.
 * @param[in] left The left expression to use for the binary expression node
 * @param[in] operator The operator to use for the binary expression node
 * @param[in] right The right expression to use for the binary expression node
 * @return On success a pointer to waitui_ast_binary_expression, else NULL
 */
extern waitui_ast_binary_expression *
waitui_ast_binary_expression_new(waitui_ast_expression *left,
                                 waitui_ast_binary_operator operator,
                                 waitui_ast_expression * right);

/**
 * @brief Get the left expression for the binary expression node for the AST.
 * @param[in] this The binary expression node to get the left expression from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_binary_expression_getLeft(waitui_ast_binary_expression *this);

/**
 * @brief Get the binary operator for the binary expression node for the AST.
 * @param[in] this The binary expression node to get the right expression from
 * @return The binary operator used with this binary expression node
 */
extern waitui_ast_binary_operator
waitui_ast_binary_expression_getOperator(waitui_ast_binary_expression *this);

/**
 * @brief Get the right expression for the binary expression node for the AST.
 * @param[in] this The binary expression node to get the right expression from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_binary_expression_getRight(waitui_ast_binary_expression *this);

/**
 * @brief Destroy a binary expression node and its content.
 * @param[in,out] this The binary expression node to destroy
 */
extern void
waitui_ast_binary_expression_destroy(waitui_ast_binary_expression **this);

/**
 * @brief Create a unary expression node for the AST.
 * @param[in] operator The operator to use for the unary expression node
 * @param[in] expression The expression to use for the unary expression node
 * @return On success a pointer to waitui_ast_unary_expression, else NULL
 */
extern waitui_ast_unary_expression *
        waitui_ast_unary_expression_new(waitui_ast_unary_operator operator,
                                        waitui_ast_expression * expression);

/**
 * @brief Get the unary operator for the unary expression node for the AST.
 * @param[in] this The binary expression node to get the right expression from
 * @return The unary operator used with this unary expression node
 */
extern waitui_ast_unary_operator
waitui_ast_unary_expression_getOperator(waitui_ast_unary_expression *this);

/**
 * @brief Get the unary operator for the unary expression node for the AST.
 * @param[in] this The unary expression node to get the expression from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_unary_expression_getExpression(waitui_ast_unary_expression *this);

/**
 * @brief Destroy a unary expression node and its content.
 * @param[in,out] this The unary expression node to destroy
 */
extern void
waitui_ast_unary_expression_destroy(waitui_ast_unary_expression **this);

/**
 * @brief TODO
 * @param expression TODO
 * @param context TODO
 * @return TODO
 */
extern waitui_ast_lazy_expression *
waitui_ast_lazy_expression_new(waitui_ast_expression *expression,
                               void *context);

/**
 * @brief TODO
 * @param this TODO
 */
extern void
waitui_ast_lazy_expression_destroy(waitui_ast_lazy_expression **this);

/**
 * @brief TODO
 * @param func TODO
 * @return TODO
 */
extern waitui_ast_native_expression *
waitui_ast_native_expression_new(void *func);

/**
 * @brief TODO
 * @param this TODO
 */
extern void
waitui_ast_native_expression_destroy(waitui_ast_native_expression **this);

/**
 * @brief Create a constructor call node for the AST.
 * @param[in] name The name to use for the constructor call node
 * @param[in] args The args to use for the constructor call node
 * @return On success a pointer to waitui_ast_constructor_call, else NULL
 */
extern waitui_ast_constructor_call *
waitui_ast_constructor_call_new(symbol *name, waitui_ast_expression_list *args);

/**
 * @brief Get the functionName for the constructor call node for the AST.
 * @param[in] this The constructor call node to get the functionName from
 * @return A pointer to symbol, else NULL
 */
extern symbol *
waitui_ast_constructor_call_getName(waitui_ast_constructor_call *this);

/**
 * @brief Get the args for the constructor call node for the AST.
 * @param[in] this The constructor call node to get the args from
 * @return A pointer to waitui_ast_expression_list, else NULL
 */
extern waitui_ast_expression_list *
waitui_ast_constructor_call_getArgs(waitui_ast_constructor_call *this);

/**
 * @brief Destroy a constructor call node and its content.
 * @param[in,out] this The constructor call node to destroy
 */
extern void
waitui_ast_constructor_call_destroy(waitui_ast_constructor_call **this);

/**
 * @brief Create a function call node for the AST.
 * @param[in] object The object to use for the function call node
 * @param[in] functionName The functionName to use for the function call node
 * @param[in] args The args to use for the function call node
 * @return On success a pointer to waitui_ast_function_call, else NULL
 */
extern waitui_ast_function_call *
waitui_ast_function_call_new(waitui_ast_expression *object,
                             symbol *functionName,
                             waitui_ast_expression_list *args);

/**
 * @brief Get the object for the function call node for the AST.
 * @param[in] this The function call node to get the object from
 * @return A pointer to waitui_ast_expression, else NULL
 */
extern waitui_ast_expression *
waitui_ast_function_call_getObject(waitui_ast_function_call *this);

/**
 * @brief Get the functionName for the function call node for the AST.
 * @param[in] this The function call node to get the functionName from
 * @return A pointer to symbol, else NULL
 */
extern symbol *
waitui_ast_function_call_getFunctionName(waitui_ast_function_call *this);

/**
 * @brief Get the args for the function call node for the AST.
 * @param[in] this The function call node to get the args from
 * @return A pointer to waitui_ast_expression_list, else NULL
 */
extern waitui_ast_expression_list *
waitui_ast_function_call_getArgs(waitui_ast_function_call *this);

/**
 * @brief Destroy a function call node and its content.
 * @param[in,out] this The function call node to destroy
 */
extern void waitui_ast_function_call_destroy(waitui_ast_function_call **this);

/**
 * @brief Create a super function call node for the AST.
 * @param[in] functionName The functionName to use for the super function call node
 * @param[in] args The args to use for the super function call node
 * @return On success a pointer to waitui_ast_super_function_call, else NULL
 */
extern waitui_ast_super_function_call *
waitui_ast_super_function_call_new(symbol *functionName,
                                   waitui_ast_expression_list *args);

/**
 * @brief Get the functionName for the super function call node for the AST.
 * @param[in] this The super function call node to get the functionName from
 * @return A pointer to symbol, else NULL
 */
extern symbol *waitui_ast_super_function_call_getFunctionName(
        waitui_ast_super_function_call *this);

/**
 * @brief Get the args for the super function call node for the AST.
 * @param[in] this The super function call node to get the args from
 * @return A pointer to waitui_ast_expression_list, else NULL
 */
extern waitui_ast_expression_list *
waitui_ast_super_function_call_getArgs(waitui_ast_super_function_call *this);

/**
 * @brief Destroy a super function call node and its content.
 * @param[in,out] this The super function call node to destroy
 */
extern void
waitui_ast_super_function_call_destroy(waitui_ast_super_function_call **this);

/**
 * @brief Create a reference node for the AST.
 * @param[in] value The value to use for the reference node
 * @return On success a pointer to waitui_ast_reference, else NULL
 */
extern waitui_ast_reference *waitui_ast_reference_new(symbol *value);

/**
 * @brief Return the value for the reference node.
 * @param[in] this The reference node to get the value from
 * @return The A pointer to symbol holding the reference value, else NULL
 */
extern symbol *waitui_ast_reference_getValue(waitui_ast_reference *this);

/**
 * @brief Destroy a reference node and its content.
 * @param[in,out] this The reference node to destroy
 */
extern void waitui_ast_reference_destroy(waitui_ast_reference **this);

/**
 * @brief Create a this literal node for the AST.
 * @return On success a pointer to waitui_ast_this_literal, else NULL
 */
extern waitui_ast_this_literal *waitui_ast_this_literal_new(void);

/**
 * @brief Destroy a this literal node and its content.
 * @param[in,out] this The this literal node to destroy
 */
extern void waitui_ast_this_literal_destroy(waitui_ast_this_literal **this);

/**
 * @brief Create a integer literal node for the AST.
 * @param[in] value The value for the integer literal node
 * @return On success a pointer to waitui_ast_integer_literal, else NULL
 */
extern waitui_ast_integer_literal *waitui_ast_integer_literal_new(str value);

/**
 * @brief Return the value for the integer literal node.
 * @param[in] this The integer literal node to get the value from
 * @return The integer literal value
 */
extern str *
waitui_ast_integer_literal_getValue(waitui_ast_integer_literal *this);

/**
 * @brief Destroy a integer literal node and its content.
 * @param[in,out] this The integer literal node to destroy
 */
extern void
waitui_ast_integer_literal_destroy(waitui_ast_integer_literal **this);

/**
 * @brief Create a boolean literal node for the AST.
 * @param[in] value The value for the boolean literal node
 * @return On success a pointer to waitui_ast_boolean_literal, else NULL
 */
extern waitui_ast_boolean_literal *waitui_ast_boolean_literal_new(bool value);

/**
 * @brief Return the value for the boolean literal node.
 * @param[in] this The boolean literal node to get the value from
 * @return The boolean literal value
 */
extern bool
waitui_ast_boolean_literal_getValue(waitui_ast_boolean_literal *this);

/**
 * @brief Destroy a boolean literal node and its content.
 * @param[in,out] this The boolean literal node to destroy
 */
extern void
waitui_ast_boolean_literal_destroy(waitui_ast_boolean_literal **this);

/**
 * @brief Create a decimal literal node for the AST.
 * @param[in] value The value for the decimal literal node
 * @return On success a pointer to waitui_ast_decimal_literal, else NULL
 */
extern waitui_ast_decimal_literal *waitui_ast_decimal_literal_new(str value);

/**
 * @brief Return the value for the decimal literal node.
 * @param[in] this The decimal literal node to get the value from
 * @return The decimal literal value
 */
extern str *
waitui_ast_decimal_literal_getValue(waitui_ast_decimal_literal *this);

/**
 * @brief Destroy a decimal literal node and its content.
 * @param[in,out] this The decimal literal node to destroy
 */
extern void
waitui_ast_decimal_literal_destroy(waitui_ast_decimal_literal **this);

/**
 * @brief Create a null literal node for the AST.
 * @return On success a pointer to waitui_ast_null_literal, else NULL
 */
extern waitui_ast_null_literal *waitui_ast_null_literal_new(void);

/**
 * @brief Destroy a null literal node and its content.
 * @param[in,out] this The null literal node to destroy
 */
extern void waitui_ast_null_literal_destroy(waitui_ast_null_literal **this);

/**
 * @brief Create a string literal node for the AST.
 * @param[in] value The value for the string literal node
 * @return On success a pointer to waitui_ast_string_literal, else NULL
 */
extern waitui_ast_string_literal *waitui_ast_string_literal_new(str value);

/**
 * @brief Return the value for the string literal node.
 * @param[in] this The string literal node to get the value from
 * @return The string literal value
 */
extern str *waitui_ast_string_literal_getValue(waitui_ast_string_literal *this);

/**
 * @brief Destroy a string literal node and its content.
 * @param[in,out] this The string literal node to destroy
 */
extern void waitui_ast_string_literal_destroy(waitui_ast_string_literal **this);

#endif// WAITUI_AST_NODE_H
