/**
 * @file ast_node.c
 * @author rick
 * @date 08.03.20
 * @brief File for the AST node implementation
 */

#include "blink/ast_node.h"

#include <assert.h>
#include <stdlib.h>


// -----------------------------------------------------------------------------
// Local definitions
// -----------------------------------------------------------------------------

#define IMPLEMENTATION_LIST_NEW(type)                                          \
    type##_list *type##_list_new() {                                           \
        return (type##_list *) list_new(                                       \
                (list_element_destroy) type##_destroy);                        \
    }

#define IMPLEMENTATION_LIST_DESTROY(type, elem)                                \
    void type##_list_destroy(type##_list **this) {                             \
        list_destroy((list **) this);                                          \
    }

#define IMPLEMENTATION_LIST_PUSH(type, elem)                                   \
    int type##_list_push(type##_list *this, type *elem) {                      \
        return list_push((list *) this, (void *) elem);                        \
    }

#define IMPLEMENTATION_LIST_POP(type, elem)                                    \
    type *type##_list_pop(type##_list *this) {                                 \
        return (type *) list_pop((list *) this);                               \
    }

#define IMPLEMENTATION_LIST_UNSHIFT(type, elem)                                \
    int type##_list_unshift(type##_list *this, type *elem) {                   \
        return list_unshift((list *) this, (void *) elem);                     \
    }

#define IMPLEMENTATION_LIST_SHIFT(type, elem)                                  \
    type *type##_list_shift(type##_list *this) {                               \
        return (type *) list_shift((list *) this);                             \
    }

#define AST_NODE_DEFINITION_INIT(node_subtype)                                 \
    this->astDefinitionType = AST_DEFINITION_TYPE_##node_subtype

#define AST_NODE_EXPRESSION_INIT(node_subtype)                                 \
    this->astExpressionType = AST_EXPRESSION_TYPE_##node_subtype

#define AST_NODE_ALLOC_INIT(type, node_type, node_subtype, node_line,          \
                            node_column)                                       \
    type *this = calloc(1, sizeof(*this));                                     \
    if (!this) { return NULL; }                                                \
                                                                               \
    this->astNodeType = AST_NODE_TYPE_##node_type;                             \
    AST_NODE_##node_type##_INIT(node_subtype);                                 \
    this->line   = node_line;                                                  \
    this->column = node_column

#define AST_NODE_FREE()                                                        \
    free(*this);                                                               \
    *this = NULL


// -----------------------------------------------------------------------------
// Local functions
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Public functions
// -----------------------------------------------------------------------------

void ast_definition_destroy(ast_definition **this) {
    if (!this || !(*this)) { return; }

    switch ((*this)->astDefinitionType) {
        case AST_DEFINITION_TYPE_FORMAL:
            ast_formal_destroy((ast_formal **) this);
            break;
        case AST_DEFINITION_TYPE_PROPERTY:
            ast_property_destroy((ast_property **) this);
            break;
        case AST_DEFINITION_TYPE_FUNCTION:
            ast_function_destroy((ast_function **) this);
            break;
        case AST_DEFINITION_TYPE_CLASS:
            ast_property_destroy((ast_property **) this);
            break;
        case AST_DEFINITION_TYPE_PROGRAM:
            ast_program_destroy((ast_program **) this);
            break;
    }
}

void ast_expression_destroy(ast_expression **this) {
    if (!this || !(*this)) { return; }

    switch ((*this)->astExpressionType) {
        case AST_EXPRESSION_TYPE_INTEGER_LITERAL:
            ast_integer_literal_destroy((ast_integer_literal **) this);
            break;
        case AST_EXPRESSION_TYPE_BOOLEAN_LITERAL:
            ast_boolean_literal_destroy((ast_boolean_literal **) this);
            break;
        case AST_EXPRESSION_TYPE_DECIMAL_LITERAL:
            ast_decimal_literal_destroy((ast_decimal_literal **) this);
            break;
        case AST_EXPRESSION_TYPE_NULL_LITERAL:
            ast_null_literal_destroy((ast_null_literal **) this);
            break;
        case AST_EXPRESSION_TYPE_STRING_LITERAL:
            ast_string_literal_destroy((ast_string_literal **) this);
            break;
        case AST_EXPRESSION_TYPE_ASSIGNMENT:
            ast_assignment_destroy((ast_assignment **) this);
            break;
        case AST_EXPRESSION_TYPE_REFERENCE:
            ast_reference_destroy((ast_reference **) this);
            break;
        case AST_EXPRESSION_TYPE_THIS_LITERAL:
            ast_this_literal_destroy((ast_this_literal **) this);
            break;
        case AST_EXPRESSION_TYPE_CAST:
            ast_cast_destroy((ast_cast **) this);
            break;
        case AST_EXPRESSION_TYPE_LET:
            ast_let_destroy((ast_let **) this);
            break;
        case AST_EXPRESSION_TYPE_BLOCK:
            ast_block_destroy((ast_block **) this);
            break;
        case AST_EXPRESSION_TYPE_CONSTRUCTOR_CALL:
            ast_constructor_call_destroy((ast_constructor_call **) this);
            break;
        case AST_EXPRESSION_TYPE_FUNCTION_CALL:
            ast_function_call_destroy((ast_function_call **) this);
            break;
        case AST_EXPRESSION_TYPE_SUPER_FUNCTION_CALL:
            ast_super_function_call_destroy((ast_super_function_call **) this);
            break;
        case AST_EXPRESSION_TYPE_BINARY_EXPRESSION:
            ast_binary_expression_destroy((ast_binary_expression **) this);
            break;
        case AST_EXPRESSION_TYPE_UNARY_EXPRESSION:
            ast_unary_expression_destroy((ast_unary_expression **) this);
            break;
        case AST_EXPRESSION_TYPE_IF_ELSE:
            ast_if_else_destroy((ast_if_else **) this);
            break;
        case AST_EXPRESSION_TYPE_WHILE:
            ast_while_destroy((ast_while **) this);
            break;
        case AST_EXPRESSION_TYPE_LAZY_EXPRESSION:
            ast_lazy_expression_destroy((ast_lazy_expression **) this);
            break;
        case AST_EXPRESSION_TYPE_NATIVE_EXPRESSION:
            ast_native_expression_destroy((ast_native_expression **) this);
            break;
        case AST_EXPRESSION_TYPE_INITIALIZATION:
            ast_initialization_destroy((ast_initialization **) this);
            break;
    }
}

ast_program *ast_program_new(ast_class_list *classes) {
    AST_NODE_ALLOC_INIT(ast_program, DEFINITION, PROGRAM, 0, 0);

    this->classes = classes;

    return this;
}

void ast_program_destroy(ast_program **this) {
    if (!this || !(*this)) { return; }

    ast_class_list_destroy(&(*this)->classes);

    AST_NODE_FREE();
}

ast_class *ast_class_new(unsigned long int line, unsigned long int column,
                         str name, ast_formal_list *parameters, str superClass,
                         ast_expression_list *superClassArgs,
                         ast_property_list *properties,
                         ast_function_list *functions) {
    AST_NODE_ALLOC_INIT(ast_class, DEFINITION, CLASS, line, column);

    this->name           = name;
    this->parameters     = parameters;
    this->superClass     = superClass;
    this->superClassArgs = superClassArgs;
    this->properties     = properties;
    this->functions      = functions;

    return this;
}

void ast_class_destroy(ast_class **this) {
    if (!this || !(*this)) { return; }

    ast_formal_list_destroy(&(*this)->parameters);
    ast_expression_list_destroy(&(*this)->superClassArgs);
    ast_property_list_destroy(&(*this)->properties);
    ast_function_list_destroy(&(*this)->functions);

    AST_NODE_FREE();
}

ast_formal *ast_formal_new(unsigned long int line, unsigned long int column,
                           str identifier, str type, int isLazy) {
    AST_NODE_ALLOC_INIT(ast_formal, DEFINITION, FORMAL, line, column);

    this->identifier = identifier;
    this->type       = type;
    this->isLazy     = isLazy;

    return this;
}

void ast_formal_destroy(ast_formal **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

ast_function *ast_function_new(unsigned long int line, unsigned long int column,
                               str name, ast_formal_list *parameters,
                               str returnType, ast_expression *body,
                               ast_function_visibility visibility,
                               int isAbstract, int isFinal, int isOverwrite) {
    AST_NODE_ALLOC_INIT(ast_function, DEFINITION, FUNCTION, line, column);

    this->name        = name;
    this->parameters  = parameters;
    this->returnType  = returnType;
    this->body        = body;
    this->visibility  = visibility;
    this->isAbstract  = isAbstract;
    this->isFinal     = isFinal;
    this->isOverwrite = isOverwrite;

    return this;
}

void ast_function_destroy(ast_function **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->body);

    AST_NODE_FREE();
}

ast_property *ast_property_new(unsigned long int line, unsigned long int column,
                               str name, str type, ast_expression *value) {
    AST_NODE_ALLOC_INIT(ast_property, DEFINITION, PROPERTY, line, column);

    this->name  = name;
    this->type  = type;
    this->value = value;

    return this;
}

void ast_property_destroy(ast_property **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->value);

    AST_NODE_FREE();
}

ast_block *ast_block_new(unsigned long int line, unsigned long int column,
                         ast_expression_list *expressions) {
    AST_NODE_ALLOC_INIT(ast_block, EXPRESSION, BLOCK, line, column);

    this->expressions = expressions;

    return this;
}

void ast_block_destroy(ast_block **this) {
    if (!this || !(*this)) { return; }

    ast_expression_list_destroy(&(*this)->expressions);

    AST_NODE_FREE();
}

ast_let *ast_let_new(unsigned long int line, unsigned long int column,
                     ast_initialization_list *initializations,
                     ast_expression *body) {
    AST_NODE_ALLOC_INIT(ast_let, EXPRESSION, LET, line, column);

    this->initializations = initializations;
    this->body            = body;

    return this;
}

void ast_let_destroy(ast_let **this) {
    if (!this || !(*this)) { return; }

    ast_initialization_list_destroy(&(*this)->initializations);
    ast_expression_destroy(&(*this)->body);

    AST_NODE_FREE();
}

ast_initialization *ast_initialization_new(unsigned long int line,
                                           unsigned long int column,
                                           str identifier, str type,
                                           ast_expression *value) {
    AST_NODE_ALLOC_INIT(ast_initialization, EXPRESSION, INITIALIZATION, line,
                        column);

    this->identifier = identifier;
    this->type       = type;
    this->value      = value;

    return this;
}

void ast_initialization_destroy(ast_initialization **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->value);

    AST_NODE_FREE();
}

ast_assignment *ast_assignment_new(unsigned long int line,
                                   unsigned long int column, str identifier,
                                   str operator, ast_expression * value) {
    AST_NODE_ALLOC_INIT(ast_assignment, EXPRESSION, ASSIGNMENT, line, column);

    this->identifier = identifier;
    this->operator   = operator;
    this->value      = value;

    return this;
}

void ast_assignment_destroy(ast_assignment **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->value);

    AST_NODE_FREE();
}

ast_cast *ast_cast_new(unsigned long int line, unsigned long int column,
                       ast_expression *object, str type) {
    AST_NODE_ALLOC_INIT(ast_cast, EXPRESSION, CAST, line, column);

    this->object = object;
    this->type   = type;

    return this;
}

void ast_cast_destroy(ast_cast **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->object);

    AST_NODE_FREE();
}

ast_if_else *ast_if_else_new(unsigned long int line, unsigned long int column,
                             ast_expression *condition,
                             ast_expression *thenBranch,
                             ast_expression *elseBranch) {
    AST_NODE_ALLOC_INIT(ast_if_else, EXPRESSION, IF_ELSE, line, column);

    this->condition  = condition;
    this->thenBranch = thenBranch;
    this->elseBranch = elseBranch;

    return this;
}

void ast_if_else_destroy(ast_if_else **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->condition);
    ast_expression_destroy(&(*this)->thenBranch);
    ast_expression_destroy(&(*this)->elseBranch);

    AST_NODE_FREE();
}

ast_while *ast_while_new(unsigned long int line, unsigned long int column,
                         ast_expression *condition, ast_expression *body) {
    AST_NODE_ALLOC_INIT(ast_while, EXPRESSION, WHILE, line, column);

    this->condition = condition;
    this->body      = body;

    return this;
}

void ast_while_destroy(ast_while **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->condition);
    ast_expression_destroy(&(*this)->body);

    AST_NODE_FREE();
}

ast_binary_expression *ast_binary_expression_new(unsigned long int line,
                                                 unsigned long int column,
                                                 ast_expression *left,
                                                 str operator,
                                                 ast_expression * right) {
    AST_NODE_ALLOC_INIT(ast_binary_expression, EXPRESSION, BINARY_EXPRESSION,
                        line, column);

    this->left    = left;
    this->operator= operator;
    this->right   = right;

    return this;
}

void ast_binary_expression_destroy(ast_binary_expression **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->left);
    ast_expression_destroy(&(*this)->right);

    AST_NODE_FREE();
}

ast_unary_expression *ast_unary_expression_new(unsigned long int line,
                                               unsigned long int column,
                                               str operator,
                                               ast_expression * expression) {
    AST_NODE_ALLOC_INIT(ast_unary_expression, EXPRESSION, UNARY_EXPRESSION,
                        line, column);

    this->operator   = operator;
    this->expression = expression;

    return this;
}

void ast_unary_expression_destroy(ast_unary_expression **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->expression);

    AST_NODE_FREE();
}

ast_lazy_expression *ast_lazy_expression_new(unsigned long int line,
                                             unsigned long int column,
                                             ast_expression *expression,
                                             void *context) {
    AST_NODE_ALLOC_INIT(ast_lazy_expression, EXPRESSION, LAZY_EXPRESSION, line,
                        column);

    this->expression = expression;
    this->context    = context;

    return this;
}

void ast_lazy_expression_destroy(ast_lazy_expression **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->expression);
    // TODO: context??

    AST_NODE_FREE();
}

ast_native_expression *ast_native_expression_new(unsigned long int line,
                                                 unsigned long int column,
                                                 void *func) {
    AST_NODE_ALLOC_INIT(ast_native_expression, EXPRESSION, NATIVE_EXPRESSION,
                        line, column);

    this->func = func;

    return this;
}

void ast_native_expression_destroy(ast_native_expression **this) {
    if (!this || !(*this)) { return; }

    // TODO: func??

    AST_NODE_FREE();
}

ast_constructor_call *ast_constructor_call_new(unsigned long int line,
                                               unsigned long int column,
                                               str type,
                                               ast_expression_list *args) {
    AST_NODE_ALLOC_INIT(ast_constructor_call, EXPRESSION, CONSTRUCTOR_CALL,
                        line, column);

    this->type = type;
    this->args = args;

    return this;
}

void ast_constructor_call_destroy(ast_constructor_call **this) {
    if (!this || !(*this)) { return; }

    ast_expression_list_destroy(&(*this)->args);

    AST_NODE_FREE();
}

ast_function_call *ast_function_call_new(unsigned long int line,
                                         unsigned long int column,
                                         ast_expression *object,
                                         str functionName,
                                         ast_expression_list *args) {
    AST_NODE_ALLOC_INIT(ast_function_call, EXPRESSION, FUNCTION_CALL, line,
                        column);

    this->object       = object;
    this->functionName = functionName;
    this->args         = args;

    return this;
}

void ast_function_call_destroy(ast_function_call **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->object);
    ast_expression_list_destroy(&(*this)->args);

    AST_NODE_FREE();
}

ast_super_function_call *
ast_super_function_call_new(unsigned long int line, unsigned long int column,
                            str functionName, ast_expression_list *args) {
    AST_NODE_ALLOC_INIT(ast_super_function_call, EXPRESSION,
                        SUPER_FUNCTION_CALL, line, column);

    this->functionName = functionName;
    this->args         = args;

    return this;
}

void ast_super_function_call_destroy(ast_super_function_call **this) {
    if (!this || !(*this)) { return; }

    ast_expression_list_destroy(&(*this)->args);

    AST_NODE_FREE();
}

ast_reference *ast_reference_new(unsigned long int line,
                                 unsigned long int column, str identifier) {
    AST_NODE_ALLOC_INIT(ast_reference, EXPRESSION, REFERENCE, line, column);

    this->identifier = identifier;

    return this;
}

void ast_reference_destroy(ast_reference **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

ast_this_literal *ast_this_literal_new(unsigned long int line,
                                       unsigned long int column) {
    AST_NODE_ALLOC_INIT(ast_this_literal, EXPRESSION, THIS_LITERAL, line,
                        column);

    return this;
}

void ast_this_literal_destroy(ast_this_literal **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

ast_integer_literal *ast_integer_literal_new(unsigned long int line,
                                             unsigned long int column,
                                             str value) {
    AST_NODE_ALLOC_INIT(ast_integer_literal, EXPRESSION, INTEGER_LITERAL, line,
                        column);

    this->value = value;

    return this;
}

void ast_integer_literal_destroy(ast_integer_literal **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

ast_boolean_literal *ast_boolean_literal_new(unsigned long int line,
                                             unsigned long int column,
                                             str value) {
    AST_NODE_ALLOC_INIT(ast_boolean_literal, EXPRESSION, BOOLEAN_LITERAL, line,
                        column);

    this->value = value;

    return this;
}

void ast_boolean_literal_destroy(ast_boolean_literal **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

ast_decimal_literal *ast_decimal_literal_new(unsigned long int line,
                                             unsigned long int column,
                                             str value) {
    AST_NODE_ALLOC_INIT(ast_decimal_literal, EXPRESSION, DECIMAL_LITERAL, line,
                        column);

    this->value = value;

    return this;
}

void ast_decimal_literal_destroy(ast_decimal_literal **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

ast_null_literal *ast_null_literal_new(unsigned long int line,
                                       unsigned long int column) {
    AST_NODE_ALLOC_INIT(ast_null_literal, EXPRESSION, NULL_LITERAL, line,
                        column);

    return this;
}

void ast_null_literal_destroy(ast_null_literal **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

ast_string_literal *ast_string_literal_new(unsigned long int line,
                                           unsigned long int column,
                                           str value) {
    AST_NODE_ALLOC_INIT(ast_string_literal, EXPRESSION, STRING_LITERAL, line,
                        column);

    this->value = value;

    return this;
}

void ast_string_literal_destroy(ast_string_literal **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

CREATE_LIST_TYPE(IMPLEMENTATION, ast_class, class)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_expression, expression)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_formal, formal)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_property, property)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_function, function)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_initialization, initialization)
