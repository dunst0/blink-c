/**
 * @file ast_node.c
 * @author rick
 * @date 08.03.20
 * @brief File for the AST node implementation
 */

#include "waitui/ast_node.h"

#include <waitui/log.h>

#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Local defines
// -----------------------------------------------------------------------------

#define AST_NODE_DEFINITION_INIT(node_subtype)                                 \
    this->astDefinitionType = WAITUI_AST_DEFINITION_TYPE_##node_subtype

#define AST_NODE_EXPRESSION_INIT(node_subtype)                                 \
    this->astExpressionType = WAITUI_AST_EXPRESSION_TYPE_##node_subtype

#define AST_NODE_NEW(type, node_type, node_subtype)                            \
    waitui_log_trace("creating new waitui_ast node " #type);                   \
    type *this = calloc(1, sizeof(*this));                                     \
    if (!this) { return NULL; }                                                \
    this->astNodeType = WAITUI_AST_NODE_TYPE_##node_type;                      \
    AST_NODE_##node_type##_INIT(node_subtype)

#define AST_NODE_NEW_DONE(type)                                                \
    waitui_log_trace("new waitui_ast node " #type " successful created");      \
    return this

#define AST_NODE_DESTROY(type)                                                 \
    waitui_log_trace("destroying waitui_ast node " #type);                     \
    if (!this || !(*this)) { return; }                                         \
    waitui_log_trace("doing clean up fo waitui_ast node " #type)

#define AST_NODE_DESTROY_DONE(type)                                            \
    free(*this);                                                               \
    *this = NULL;                                                              \
    waitui_log_trace("waitui_ast node " #type " successful destroyed")

#define WAITUI_AST_NODE_GET(type, error)                                       \
    waitui_log_trace("get data from waitui_ast node " #type);                  \
    if (!this) { return (error); }                                             \
    waitui_log_trace("doing get data from waitui_ast node " #type)

#define WAITUI_AST_NODE_SET(type)                                              \
    waitui_log_trace("set data in waitui_ast node " #type);                    \
    if (!this) { return; }                                                     \
    waitui_log_trace("doing set data in waitui_ast node " #type)

#define WAITUI_AST_NODE_SET_DONE(type)                                         \
    waitui_log_trace("successful set data in waitui_ast node " #type)

/**
 * @brief Properties for an AST node in general.
 */
#define WAITUI_AST_NODE_PROPERTIES waitui_ast_node_type astNodeType;

/**
 * @brief Properties for a definition AST node in general.
 */
#define WAITUI_AST_DEFINITION_PROPERTIES                                       \
    WAITUI_AST_NODE_PROPERTIES                                                 \
    waitui_ast_definition_type astDefinitionType;

/**
 * @brief Properties for an expression AST node in general.
 */
#define WAITUI_AST_EXPRESSION_PROPERTIES                                       \
    WAITUI_AST_NODE_PROPERTIES                                                 \
    waitui_ast_expression_type astExpressionType;                              \
    void *expressionType;// FIXME: not specified


// -----------------------------------------------------------------------------
//  Local types
// -----------------------------------------------------------------------------

/**
 * @brief Struct representing an AST node.
 */
struct waitui_ast_node {
    WAITUI_AST_NODE_PROPERTIES
};

/**
 * @brief Struct representing an AST definition.
 */
struct waitui_ast_definition {
    WAITUI_AST_DEFINITION_PROPERTIES
};

/**
 * @brief Struct representing an AST expression.
 */
struct waitui_ast_expression {
    WAITUI_AST_EXPRESSION_PROPERTIES
};

/**
 * @brief Struct representing an AST program.
 */
struct waitui_ast_program {
    WAITUI_AST_DEFINITION_PROPERTIES
    waitui_ast_namespace_list *namespaces;
};

/**
 * @brief Struct representing an AST namespace.
 */
struct waitui_ast_namespace {
    WAITUI_AST_DEFINITION_PROPERTIES
    symbol *name;
    waitui_ast_import_list *imports;
    waitui_ast_class_list *classes;
};

/**
 * @brief Struct representing an AST import.
 */
struct waitui_ast_import {
    WAITUI_AST_DEFINITION_PROPERTIES
};

/**
 * @brief Struct representing an AST class.
 */
struct waitui_ast_class {
    WAITUI_AST_DEFINITION_PROPERTIES
    symbol *name;
    waitui_ast_formal_list *parameters;
    symbol *superClass;
    waitui_ast_expression_list *superClassArgs;
    waitui_ast_property_list *properties;
    waitui_ast_function_list *functions;
};

/**
 * @brief Struct representing an AST formal.
 */
struct waitui_ast_formal {
    WAITUI_AST_DEFINITION_PROPERTIES
    symbol *identifier;
    symbol *type;
    bool isLazy;
};

/**
 * @brief Struct representing an AST property.
 */
struct waitui_ast_property {
    WAITUI_AST_DEFINITION_PROPERTIES
    symbol *name;
    symbol *type;
    waitui_ast_expression *value;
};

/**
 * @brief Struct representing an AST function.
 */
struct waitui_ast_function {
    WAITUI_AST_DEFINITION_PROPERTIES
    symbol *functionName;
    waitui_ast_formal_list *parameters;
    symbol *returnType;
    waitui_ast_expression *body;
    waitui_ast_function_visibility visibility;
    bool isAbstract;
    bool isFinal;
    bool isOverwrite;
};

/**
 * @brief Struct representing an AST block.
 */
struct waitui_ast_block {
    WAITUI_AST_EXPRESSION_PROPERTIES
    waitui_ast_expression_list *expressions;
};

/**
 * @brief Struct representing an AST let.
 */
struct waitui_ast_let {
    WAITUI_AST_EXPRESSION_PROPERTIES
    waitui_ast_initialization_list *initializations;
    waitui_ast_expression *body;
};

/**
 * @brief Struct representing an AST initialization.
 */
struct waitui_ast_initialization {
    WAITUI_AST_EXPRESSION_PROPERTIES
    symbol *identifier;
    symbol *type;
    waitui_ast_expression *value;
};

/**
 * @brief Struct representing an AST assignment.
 */
struct waitui_ast_assignment {
    WAITUI_AST_EXPRESSION_PROPERTIES
    symbol *identifier;
    waitui_ast_assignment_operator operator;
    waitui_ast_expression *value;
};

/**
 * @brief Struct representing an AST cast.
 */
struct waitui_ast_cast {
    WAITUI_AST_EXPRESSION_PROPERTIES
    symbol *type;
    waitui_ast_expression *object;
};

/**
 * @brief Struct representing an AST if else.
 */
struct waitui_ast_if_else {
    WAITUI_AST_EXPRESSION_PROPERTIES
    waitui_ast_expression *condition;
    waitui_ast_expression *thenBranch;
    waitui_ast_expression *elseBranch;
};

/**
 * @brief Struct representing an AST while.
 */
struct waitui_ast_while {
    WAITUI_AST_EXPRESSION_PROPERTIES
    waitui_ast_expression *condition;
    waitui_ast_expression *body;
};

/**
 * @brief Struct representing an AST binary expression.
 */
struct waitui_ast_binary_expression {
    WAITUI_AST_EXPRESSION_PROPERTIES
    waitui_ast_expression *left;
    waitui_ast_binary_operator operator;
    waitui_ast_expression *right;
};

/**
 * @brief Struct representing an AST unary expression.
 */
struct waitui_ast_unary_expression {
    WAITUI_AST_EXPRESSION_PROPERTIES
    waitui_ast_unary_operator operator;
    waitui_ast_expression *expression;
};

/**
 * @brief Struct representing an AST lazy expression.
 */
struct waitui_ast_lazy_expression {
    WAITUI_AST_EXPRESSION_PROPERTIES
    waitui_ast_expression *expression;
    void *context;// FIXME: not specified
};

/**
 * @brief Struct representing an AST native expression.
 */
struct waitui_ast_native_expression {
    WAITUI_AST_EXPRESSION_PROPERTIES
    void *func;// FIXME: not specified
};

/**
 * @brief Struct representing an AST constructor call.
 */
struct waitui_ast_constructor_call {
    WAITUI_AST_EXPRESSION_PROPERTIES
    symbol *name;
    waitui_ast_expression_list *args;
};

/**
 * @brief Struct representing an AST function call.
 */
struct waitui_ast_function_call {
    WAITUI_AST_EXPRESSION_PROPERTIES
    waitui_ast_expression *object;
    symbol *functionName;
    waitui_ast_expression_list *args;
};

/**
 * @brief Struct representing an AST super function call.
 */
struct waitui_ast_super_function_call {
    WAITUI_AST_EXPRESSION_PROPERTIES
    symbol *functionName;
    waitui_ast_expression_list *args;
};

/**
 * @brief Struct representing an AST reference.
 */
struct waitui_ast_reference {
    WAITUI_AST_EXPRESSION_PROPERTIES
    symbol *value;
};

/**
 * @brief Struct representing an AST reference.
 */
struct waitui_ast_this_literal {
    WAITUI_AST_EXPRESSION_PROPERTIES
};

/**
 * @brief Struct representing an AST integer literal.
 */
struct waitui_ast_integer_literal {
    WAITUI_AST_EXPRESSION_PROPERTIES
    str value;
};

/**
 * @brief Struct representing an AST boolean literal.
 */
struct waitui_ast_boolean_literal {
    WAITUI_AST_EXPRESSION_PROPERTIES
    bool value;
};

/**
 * @brief Struct representing an AST decimal literal.
 */
struct waitui_ast_decimal_literal {
    WAITUI_AST_EXPRESSION_PROPERTIES
    str value;
};

/**
 * @brief Struct representing an AST null literal.
 */
struct waitui_ast_null_literal {
    WAITUI_AST_EXPRESSION_PROPERTIES
};

/**
 * @brief Struct representing an AST string literal.
 */
struct waitui_ast_string_literal {
    WAITUI_AST_EXPRESSION_PROPERTIES
    str value;
};


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

CREATE_LIST_TYPE(IMPLEMENTATION, waitui_ast_namespace)
CREATE_LIST_TYPE(IMPLEMENTATION, waitui_ast_import)
CREATE_LIST_TYPE(IMPLEMENTATION, waitui_ast_class)
CREATE_LIST_TYPE(IMPLEMENTATION, waitui_ast_expression)
CREATE_LIST_TYPE(IMPLEMENTATION, waitui_ast_formal)
CREATE_LIST_TYPE(IMPLEMENTATION, waitui_ast_property)
CREATE_LIST_TYPE(IMPLEMENTATION, waitui_ast_function)
CREATE_LIST_TYPE(IMPLEMENTATION, waitui_ast_initialization)

waitui_ast_node_type waitui_ast_node_getNodeType(const waitui_ast_node *this) {
    WAITUI_AST_NODE_GET(waitui_ast_node, WAITUI_AST_NODE_TYPE_UNDEFINED);
    return this->astNodeType;
}

void waitui_ast_node_destroy(waitui_ast_node **this) {
    if (!this || !(*this)) { return; }

    switch ((*this)->astNodeType) {
        case WAITUI_AST_NODE_TYPE_EXPRESSION:
            waitui_ast_expression_destroy((waitui_ast_expression **) this);
            break;
        case WAITUI_AST_NODE_TYPE_DEFINITION:
            waitui_ast_definition_destroy((waitui_ast_definition **) this);
            break;
        default:
            waitui_log_trace(
                    "trying to destroy a node with an undefined node type");
            break;
    }
}

waitui_ast_definition_type
waitui_ast_definition_getDefinitionType(const waitui_ast_definition *this) {
    WAITUI_AST_NODE_GET(waitui_ast_definition,
                        WAITUI_AST_DEFINITION_TYPE_UNDEFINED);
    return this->astDefinitionType;
}

void waitui_ast_definition_destroy(waitui_ast_definition **this) {
    if (!this || !(*this)) { return; }

    switch ((*this)->astDefinitionType) {
        case WAITUI_AST_DEFINITION_TYPE_FORMAL:
            waitui_ast_formal_destroy((waitui_ast_formal **) this);
            break;
        case WAITUI_AST_DEFINITION_TYPE_PROPERTY:
            waitui_ast_property_destroy((waitui_ast_property **) this);
            break;
        case WAITUI_AST_DEFINITION_TYPE_FUNCTION:
            waitui_ast_function_destroy((waitui_ast_function **) this);
            break;
        case WAITUI_AST_DEFINITION_TYPE_CLASS:
            waitui_ast_class_destroy((waitui_ast_class **) this);
            break;
        case WAITUI_AST_DEFINITION_TYPE_PROGRAM:
            waitui_ast_program_destroy((waitui_ast_program **) this);
            break;
        case WAITUI_AST_DEFINITION_TYPE_IMPORT:
            waitui_ast_import_destroy((waitui_ast_import **) this);
            break;
        case WAITUI_AST_DEFINITION_TYPE_NAMESPACE:
            waitui_ast_namespace_destroy((waitui_ast_namespace **) this);
            break;
        default:
            waitui_log_trace("trying to destroy a definition with an undefined "
                             "definition type");
            break;
    }
}

waitui_ast_expression_type
waitui_ast_expression_getExpressionType(const waitui_ast_expression *this) {
    WAITUI_AST_NODE_GET(waitui_ast_expression,
                        WAITUI_AST_EXPRESSION_TYPE_UNDEFINED);
    return this->astExpressionType;
}

void waitui_ast_expression_destroy(waitui_ast_expression **this) {
    if (!this || !(*this)) { return; }

    switch ((*this)->astExpressionType) {
        case WAITUI_AST_EXPRESSION_TYPE_INTEGER_LITERAL:
            waitui_ast_integer_literal_destroy(
                    (waitui_ast_integer_literal **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_BOOLEAN_LITERAL:
            waitui_ast_boolean_literal_destroy(
                    (waitui_ast_boolean_literal **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_DECIMAL_LITERAL:
            waitui_ast_decimal_literal_destroy(
                    (waitui_ast_decimal_literal **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_NULL_LITERAL:
            waitui_ast_null_literal_destroy((waitui_ast_null_literal **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_STRING_LITERAL:
            waitui_ast_string_literal_destroy(
                    (waitui_ast_string_literal **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_ASSIGNMENT:
            waitui_ast_assignment_destroy((waitui_ast_assignment **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_REFERENCE:
            waitui_ast_reference_destroy((waitui_ast_reference **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_THIS_LITERAL:
            waitui_ast_this_literal_destroy((waitui_ast_this_literal **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_CAST:
            waitui_ast_cast_destroy((waitui_ast_cast **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_LET:
            waitui_ast_let_destroy((waitui_ast_let **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_BLOCK:
            waitui_ast_block_destroy((waitui_ast_block **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_CONSTRUCTOR_CALL:
            waitui_ast_constructor_call_destroy(
                    (waitui_ast_constructor_call **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_FUNCTION_CALL:
            waitui_ast_function_call_destroy(
                    (waitui_ast_function_call **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_SUPER_FUNCTION_CALL:
            waitui_ast_super_function_call_destroy(
                    (waitui_ast_super_function_call **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_BINARY_EXPRESSION:
            waitui_ast_binary_expression_destroy(
                    (waitui_ast_binary_expression **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_UNARY_EXPRESSION:
            waitui_ast_unary_expression_destroy(
                    (waitui_ast_unary_expression **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_IF_ELSE:
            waitui_ast_if_else_destroy((waitui_ast_if_else **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_WHILE:
            waitui_ast_while_destroy((waitui_ast_while **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_LAZY_EXPRESSION:
            waitui_ast_lazy_expression_destroy(
                    (waitui_ast_lazy_expression **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_NATIVE_EXPRESSION:
            waitui_ast_native_expression_destroy(
                    (waitui_ast_native_expression **) this);
            break;
        case WAITUI_AST_EXPRESSION_TYPE_INITIALIZATION:
            waitui_ast_initialization_destroy(
                    (waitui_ast_initialization **) this);
            break;
        default:
            waitui_log_trace("trying to destroy a expression with an undefined "
                             "expression type");
            break;
    }
}

waitui_ast_program *
waitui_ast_program_new(waitui_ast_namespace_list *namespaces) {
    AST_NODE_NEW(waitui_ast_program, DEFINITION, PROGRAM);

    this->namespaces = namespaces;

    AST_NODE_NEW_DONE(waitui_ast_program);
}

waitui_ast_namespace_list *
waitui_ast_program_getNamespaces(waitui_ast_program *this) {
    WAITUI_AST_NODE_GET(waitui_ast_program, NULL);
    return this->namespaces;
}

void waitui_ast_program_destroy(waitui_ast_program **this) {
    AST_NODE_DESTROY(waitui_ast_program);

    waitui_ast_namespace_list_destroy(&(*this)->namespaces);

    AST_NODE_DESTROY_DONE(waitui_ast_program);
}

waitui_ast_namespace *waitui_ast_namespace_new(symbol *name,
                                               waitui_ast_import_list *imports,
                                               waitui_ast_class_list *classes) {
    AST_NODE_NEW(waitui_ast_namespace, DEFINITION, NAMESPACE);

    this->name    = name;
    this->imports = imports;
    this->classes = classes;

    symbol_increment_refcount(name);

    AST_NODE_NEW_DONE(waitui_ast_namespace);
}

symbol *waitui_ast_namespace_getName(waitui_ast_namespace *this) {
    WAITUI_AST_NODE_GET(waitui_ast_namespace, NULL);
    return this->name;
}

waitui_ast_import_list *
waitui_ast_namespace_getImports(waitui_ast_namespace *this) {
    WAITUI_AST_NODE_GET(waitui_ast_namespace, NULL);
    return this->imports;
}

waitui_ast_class_list *
waitui_ast_namespace_getClasses(waitui_ast_namespace *this) {
    WAITUI_AST_NODE_GET(waitui_ast_namespace, NULL);
    return this->classes;
}

void waitui_ast_namespace_destroy(waitui_ast_namespace **this) {
    AST_NODE_DESTROY(waitui_ast_namespace);

    symbol_decrement_refcount(&(*this)->name);

    waitui_ast_import_list_destroy(&(*this)->imports);
    waitui_ast_class_list_destroy(&(*this)->classes);

    AST_NODE_DESTROY_DONE(waitui_ast_namespace);
}

waitui_ast_import *waitui_ast_import_new(void) {
    AST_NODE_NEW(waitui_ast_import, DEFINITION, IMPORT);

    AST_NODE_NEW_DONE(waitui_ast_import);
}

void waitui_ast_import_destroy(waitui_ast_import **this) {
    AST_NODE_DESTROY(waitui_ast_import);

    AST_NODE_DESTROY_DONE(waitui_ast_import);
}

waitui_ast_class *
waitui_ast_class_new(symbol *name, waitui_ast_formal_list *parameters,
                     symbol *superClass,
                     waitui_ast_expression_list *superClassArgs,
                     waitui_ast_property_list *properties,
                     waitui_ast_function_list *functions) {
    AST_NODE_NEW(waitui_ast_class, DEFINITION, CLASS);

    this->name           = name;
    this->parameters     = parameters;
    this->superClass     = superClass;
    this->superClassArgs = superClassArgs;
    this->properties     = properties;
    this->functions      = functions;

    symbol_increment_refcount(name);
    symbol_increment_refcount(superClass);

    AST_NODE_NEW_DONE(waitui_ast_class);
}

symbol *waitui_ast_class_getName(waitui_ast_class *this) {
    WAITUI_AST_NODE_GET(waitui_ast_class, NULL);
    return this->name;
}

void waitui_ast_class_setName(waitui_ast_class *this, symbol *name) {
    WAITUI_AST_NODE_SET(waitui_ast_class);

    if (!name) { return; }

    if (this->name) { symbol_decrement_refcount(&this->name); }
    this->name = name;

    symbol_increment_refcount(name);

    WAITUI_AST_NODE_SET_DONE(waitui_ast_class);
}

waitui_ast_formal_list *waitui_ast_class_getParameters(waitui_ast_class *this) {
    WAITUI_AST_NODE_GET(waitui_ast_class, NULL);
    return this->parameters;
}

void waitui_ast_class_setParameters(waitui_ast_class *this,
                                    waitui_ast_formal_list *parameters) {
    WAITUI_AST_NODE_SET(waitui_ast_class);

    if (!parameters) { return; }

    if (this->parameters) { waitui_ast_formal_list_destroy(&this->parameters); }
    this->parameters = parameters;

    WAITUI_AST_NODE_SET_DONE(waitui_ast_class);
}

symbol *waitui_ast_class_getSuperClass(waitui_ast_class *this) {
    WAITUI_AST_NODE_GET(waitui_ast_class, NULL);
    return this->superClass;
}

void waitui_ast_class_setSuperClass(waitui_ast_class *this,
                                    symbol *superClass) {
    WAITUI_AST_NODE_SET(waitui_ast_class);

    if (!superClass) { return; }

    if (this->superClass) { symbol_decrement_refcount(&this->superClass); }
    this->superClass = superClass;

    symbol_increment_refcount(superClass);

    WAITUI_AST_NODE_SET_DONE(waitui_ast_class);
}

waitui_ast_expression_list *
waitui_ast_class_getSuperClassArgs(waitui_ast_class *this) {
    WAITUI_AST_NODE_GET(waitui_ast_class, NULL);
    return this->superClassArgs;
}

void waitui_ast_class_setSuperClassArgs(
        waitui_ast_class *this, waitui_ast_expression_list *superClassArgs) {
    WAITUI_AST_NODE_SET(waitui_ast_class);

    if (!superClassArgs) { return; }

    if (this->superClassArgs) {
        waitui_ast_expression_list_destroy(&this->superClassArgs);
    }
    this->superClassArgs = superClassArgs;

    WAITUI_AST_NODE_SET_DONE(waitui_ast_class);
}

waitui_ast_property_list *
waitui_ast_class_getProperties(waitui_ast_class *this) {
    WAITUI_AST_NODE_GET(waitui_ast_class, NULL);
    return this->properties;
}

waitui_ast_function_list *
waitui_ast_class_getFunctions(waitui_ast_class *this) {
    WAITUI_AST_NODE_GET(waitui_ast_class, NULL);
    return this->functions;
}

void waitui_ast_class_destroy(waitui_ast_class **this) {
    AST_NODE_DESTROY(waitui_ast_class);

    symbol_decrement_refcount(&(*this)->superClass);
    symbol_decrement_refcount(&(*this)->name);

    waitui_ast_formal_list_destroy(&(*this)->parameters);
    waitui_ast_expression_list_destroy(&(*this)->superClassArgs);
    waitui_ast_property_list_destroy(&(*this)->properties);
    waitui_ast_function_list_destroy(&(*this)->functions);

    AST_NODE_DESTROY_DONE(waitui_ast_class);
}

waitui_ast_formal *waitui_ast_formal_new(symbol *identifier, symbol *type,
                                         bool isLazy) {
    AST_NODE_NEW(waitui_ast_formal, DEFINITION, FORMAL);

    this->identifier = identifier;
    this->type       = type;
    this->isLazy     = isLazy;

    symbol_increment_refcount(identifier);
    symbol_increment_refcount(type);

    AST_NODE_NEW_DONE(waitui_ast_formal);
}

symbol *waitui_ast_formal_getIdentifier(waitui_ast_formal *this) {
    WAITUI_AST_NODE_GET(waitui_ast_formal, NULL);
    return this->identifier;
}

symbol *waitui_ast_formal_getType(waitui_ast_formal *this) {
    WAITUI_AST_NODE_GET(waitui_ast_formal, NULL);
    return this->type;
}

bool waitui_ast_formal_isLazy(waitui_ast_formal *this) {
    WAITUI_AST_NODE_GET(waitui_ast_formal, NULL);
    return this->isLazy;
}

void waitui_ast_formal_destroy(waitui_ast_formal **this) {
    AST_NODE_DESTROY(waitui_ast_formal);

    symbol_decrement_refcount(&(*this)->type);
    symbol_decrement_refcount(&(*this)->identifier);

    AST_NODE_DESTROY_DONE(waitui_ast_formal);
}

waitui_ast_function *
waitui_ast_function_new(symbol *functionName,
                        waitui_ast_formal_list *parameters, symbol *returnType,
                        waitui_ast_expression *body,
                        waitui_ast_function_visibility visibility,
                        bool isAbstract, bool isFinal, bool isOverwrite) {
    AST_NODE_NEW(waitui_ast_function, DEFINITION, FUNCTION);

    this->functionName = functionName;
    this->parameters   = parameters;
    this->returnType   = returnType;
    this->body         = body;
    this->visibility   = visibility;
    this->isAbstract   = isAbstract;
    this->isFinal      = isFinal;
    this->isOverwrite  = isOverwrite;

    symbol_increment_refcount(functionName);
    symbol_increment_refcount(returnType);

    AST_NODE_NEW_DONE(waitui_ast_function);
}

symbol *waitui_ast_function_getFunctionName(waitui_ast_function *this) {
    WAITUI_AST_NODE_GET(waitui_ast_function, NULL);
    return this->functionName;
}

waitui_ast_formal_list *
waitui_ast_function_getParameters(waitui_ast_function *this) {
    WAITUI_AST_NODE_GET(waitui_ast_function, NULL);
    return this->parameters;
}

symbol *waitui_ast_function_getReturnType(waitui_ast_function *this) {
    WAITUI_AST_NODE_GET(waitui_ast_function, NULL);
    return this->returnType;
}

waitui_ast_expression *waitui_ast_function_getBody(waitui_ast_function *this) {
    WAITUI_AST_NODE_GET(waitui_ast_function, NULL);
    return this->body;
}

void waitui_ast_function_setBody(waitui_ast_function *this,
                                 waitui_ast_expression *body) {
    WAITUI_AST_NODE_SET(waitui_ast_function);

    if (this->body) { waitui_ast_expression_destroy(&this->body); }
    this->body = body;

    WAITUI_AST_NODE_SET_DONE(waitui_ast_function);
}

waitui_ast_function_visibility
waitui_ast_function_getVisibility(waitui_ast_function *this) {
    WAITUI_AST_NODE_GET(waitui_ast_function,
                        WAITUI_AST_FUNCTION_VISIBILITY_UNDEFINED);
    return this->visibility;
}

void waitui_ast_function_setVisibility(
        waitui_ast_function *this, waitui_ast_function_visibility visibility) {
    WAITUI_AST_NODE_SET(waitui_ast_function);

    this->visibility = visibility;

    WAITUI_AST_NODE_SET_DONE(waitui_ast_function);
}

bool waitui_ast_function_isAbstract(waitui_ast_function *this) {
    WAITUI_AST_NODE_GET(waitui_ast_function, NULL);
    return this->isAbstract;
}

void waitui_ast_function_setAbstract(waitui_ast_function *this,
                                     bool isAbstract) {
    WAITUI_AST_NODE_SET(waitui_ast_function);

    this->isAbstract = isAbstract;

    WAITUI_AST_NODE_SET_DONE(waitui_ast_function);
}

bool waitui_ast_function_isFinal(waitui_ast_function *this) {
    WAITUI_AST_NODE_GET(waitui_ast_function, NULL);
    return this->isFinal;
}

void waitui_ast_function_setFinal(waitui_ast_function *this, bool isFinal) {
    WAITUI_AST_NODE_SET(waitui_ast_function);

    this->isFinal = isFinal;

    WAITUI_AST_NODE_SET_DONE(waitui_ast_function);
}

bool waitui_ast_function_isOverwrite(waitui_ast_function *this) {
    WAITUI_AST_NODE_GET(waitui_ast_function, NULL);
    return this->isOverwrite;
}

void waitui_ast_function_setOverwrite(waitui_ast_function *this,
                                      bool isOverwrite) {
    WAITUI_AST_NODE_SET(waitui_ast_function);

    this->isOverwrite = isOverwrite;

    WAITUI_AST_NODE_SET_DONE(waitui_ast_function);
}

void waitui_ast_function_destroy(waitui_ast_function **this) {
    AST_NODE_DESTROY(waitui_ast_function);

    symbol_decrement_refcount(&(*this)->returnType);
    symbol_decrement_refcount(&(*this)->functionName);

    waitui_ast_formal_list_destroy(&(*this)->parameters);
    waitui_ast_expression_destroy(&(*this)->body);

    AST_NODE_DESTROY_DONE(waitui_ast_function);
}

waitui_ast_property *waitui_ast_property_new(symbol *name, symbol *type,
                                             waitui_ast_expression *value) {
    AST_NODE_NEW(waitui_ast_property, DEFINITION, PROPERTY);

    this->name  = name;
    this->type  = type;
    this->value = value;

    symbol_increment_refcount(name);
    symbol_increment_refcount(type);

    AST_NODE_NEW_DONE(waitui_ast_property);
}

symbol *waitui_ast_property_getName(waitui_ast_property *this) {
    WAITUI_AST_NODE_GET(waitui_ast_property, NULL);
    return this->name;
}

symbol *waitui_ast_property_getType(waitui_ast_property *this) {
    WAITUI_AST_NODE_GET(waitui_ast_property, NULL);
    return this->type;
}

waitui_ast_expression *waitui_ast_property_getValue(waitui_ast_property *this) {
    WAITUI_AST_NODE_GET(waitui_ast_property, NULL);
    return this->value;
}

void waitui_ast_property_destroy(waitui_ast_property **this) {
    AST_NODE_DESTROY(waitui_ast_property);

    symbol_decrement_refcount(&(*this)->name);
    symbol_decrement_refcount(&(*this)->type);

    waitui_ast_expression_destroy(&(*this)->value);

    AST_NODE_DESTROY_DONE(waitui_ast_property);
}

waitui_ast_block *
waitui_ast_block_new(waitui_ast_expression_list *expressions) {
    AST_NODE_NEW(waitui_ast_block, EXPRESSION, BLOCK);

    this->expressions = expressions;

    AST_NODE_NEW_DONE(waitui_ast_block);
}

waitui_ast_expression_list *
waitui_ast_block_getExpressions(waitui_ast_block *this) {
    WAITUI_AST_NODE_GET(waitui_ast_block, NULL);
    return this->expressions;
}

void waitui_ast_block_destroy(waitui_ast_block **this) {
    AST_NODE_DESTROY(waitui_ast_block);

    waitui_ast_expression_list_destroy(&(*this)->expressions);

    AST_NODE_DESTROY_DONE(waitui_ast_block);
}

waitui_ast_let *
waitui_ast_let_new(waitui_ast_initialization_list *initializations,
                   waitui_ast_expression *body) {
    AST_NODE_NEW(waitui_ast_let, EXPRESSION, LET);

    this->initializations = initializations;
    this->body            = body;

    AST_NODE_NEW_DONE(waitui_ast_let);
}

waitui_ast_initialization_list *
waitui_ast_let_getInitializations(waitui_ast_let *this) {
    WAITUI_AST_NODE_GET(waitui_ast_let, NULL);
    return this->initializations;
}

waitui_ast_expression *waitui_ast_let_getBody(waitui_ast_let *this) {
    WAITUI_AST_NODE_GET(waitui_ast_let, NULL);
    return this->body;
}

void waitui_ast_let_destroy(waitui_ast_let **this) {
    AST_NODE_DESTROY(waitui_ast_let);

    waitui_ast_initialization_list_destroy(&(*this)->initializations);
    waitui_ast_expression_destroy(&(*this)->body);

    AST_NODE_DESTROY_DONE(waitui_ast_let);
}

waitui_ast_initialization *
waitui_ast_initialization_new(symbol *identifier, symbol *type,
                              waitui_ast_expression *value) {
    AST_NODE_NEW(waitui_ast_initialization, EXPRESSION, INITIALIZATION);

    this->identifier = identifier;
    this->type       = type;
    this->value      = value;

    symbol_increment_refcount(identifier);
    symbol_increment_refcount(type);

    AST_NODE_NEW_DONE(waitui_ast_initialization);
}

symbol *
waitui_ast_initialization_getIdentifier(waitui_ast_initialization *this) {
    WAITUI_AST_NODE_GET(waitui_ast_initialization, NULL);
    return this->identifier;
}

symbol *waitui_ast_initialization_getType(waitui_ast_initialization *this) {
    WAITUI_AST_NODE_GET(waitui_ast_initialization, NULL);
    return this->type;
}

waitui_ast_expression *
waitui_ast_initialization_getValue(waitui_ast_initialization *this) {
    WAITUI_AST_NODE_GET(waitui_ast_initialization, NULL);
    return this->value;
}

void waitui_ast_initialization_destroy(waitui_ast_initialization **this) {
    AST_NODE_DESTROY(waitui_ast_initialization);

    symbol_decrement_refcount(&(*this)->identifier);
    symbol_decrement_refcount(&(*this)->type);

    waitui_ast_expression_destroy(&(*this)->value);

    AST_NODE_DESTROY_DONE(waitui_ast_initialization);
}

waitui_ast_assignment *
waitui_ast_assignment_new(symbol *identifier,
                          waitui_ast_assignment_operator operator,
                          waitui_ast_expression * value) {
    AST_NODE_NEW(waitui_ast_assignment, EXPRESSION, ASSIGNMENT);

    this->identifier = identifier;
    this->operator   = operator;
    this->value      = value;

    symbol_increment_refcount(identifier);

    AST_NODE_NEW_DONE(waitui_ast_assignment);
}

symbol *waitui_ast_assignment_getIdentifier(waitui_ast_assignment *this) {
    WAITUI_AST_NODE_GET(waitui_ast_assignment, NULL);
    return this->identifier;
}

waitui_ast_assignment_operator
waitui_ast_assignment_getOperator(waitui_ast_assignment *this) {
    WAITUI_AST_NODE_GET(waitui_ast_assignment,
                        WAITUI_AST_ASSIGNMENT_OPERATOR_UNDEFINED);
    return this->operator;
}

waitui_ast_expression *
waitui_ast_assignment_getValue(waitui_ast_assignment *this) {
    WAITUI_AST_NODE_GET(waitui_ast_assignment, NULL);
    return this->value;
}

void waitui_ast_assignment_destroy(waitui_ast_assignment **this) {
    AST_NODE_DESTROY(waitui_ast_assignment);

    symbol_decrement_refcount(&(*this)->identifier);

    waitui_ast_expression_destroy(&(*this)->value);

    AST_NODE_DESTROY_DONE(waitui_ast_assignment);
}

waitui_ast_cast *waitui_ast_cast_new(waitui_ast_expression *object,
                                     symbol *type) {
    AST_NODE_NEW(waitui_ast_cast, EXPRESSION, CAST);

    this->object = object;
    this->type   = type;

    symbol_increment_refcount(type);

    AST_NODE_NEW_DONE(waitui_ast_cast);
}

waitui_ast_expression *waitui_ast_cast_getObject(waitui_ast_cast *this) {
    WAITUI_AST_NODE_GET(waitui_ast_cast, NULL);
    return this->object;
}

symbol *waitui_ast_cast_getType(waitui_ast_cast *this) {
    WAITUI_AST_NODE_GET(waitui_ast_cast, NULL);
    return this->type;
}

void waitui_ast_cast_destroy(waitui_ast_cast **this) {
    AST_NODE_DESTROY(waitui_ast_cast);

    symbol_decrement_refcount(&(*this)->type);

    waitui_ast_expression_destroy(&(*this)->object);

    AST_NODE_DESTROY_DONE(waitui_ast_cast);
}

waitui_ast_if_else *waitui_ast_if_else_new(waitui_ast_expression *condition,
                                           waitui_ast_expression *thenBranch,
                                           waitui_ast_expression *elseBranch) {
    AST_NODE_NEW(waitui_ast_if_else, EXPRESSION, IF_ELSE);

    this->condition  = condition;
    this->thenBranch = thenBranch;
    this->elseBranch = elseBranch;

    AST_NODE_NEW_DONE(waitui_ast_if_else);
}

waitui_ast_expression *
waitui_ast_if_else_getCondition(waitui_ast_if_else *this) {
    WAITUI_AST_NODE_GET(waitui_ast_if_else, NULL);
    return this->condition;
}

waitui_ast_expression *
waitui_ast_if_else_getThenBranch(waitui_ast_if_else *this) {
    WAITUI_AST_NODE_GET(waitui_ast_if_else, NULL);
    return this->thenBranch;
}

waitui_ast_expression *
waitui_ast_if_else_getElseBranch(waitui_ast_if_else *this) {
    WAITUI_AST_NODE_GET(waitui_ast_if_else, NULL);
    return this->elseBranch;
}

void waitui_ast_if_else_destroy(waitui_ast_if_else **this) {
    AST_NODE_DESTROY(waitui_ast_if_else);

    waitui_ast_expression_destroy(&(*this)->condition);
    waitui_ast_expression_destroy(&(*this)->thenBranch);
    waitui_ast_expression_destroy(&(*this)->elseBranch);

    AST_NODE_DESTROY_DONE(waitui_ast_if_else);
}

waitui_ast_while *waitui_ast_while_new(waitui_ast_expression *condition,
                                       waitui_ast_expression *body) {
    AST_NODE_NEW(waitui_ast_while, EXPRESSION, WHILE);

    this->condition = condition;
    this->body      = body;

    AST_NODE_NEW_DONE(waitui_ast_while);
}

waitui_ast_expression *waitui_ast_while_getCondition(waitui_ast_while *this) {
    WAITUI_AST_NODE_GET(waitui_ast_while, NULL);
    return this->condition;
}

waitui_ast_expression *waitui_ast_while_getBody(waitui_ast_while *this) {
    WAITUI_AST_NODE_GET(waitui_ast_while, NULL);
    return this->body;
}

void waitui_ast_while_destroy(waitui_ast_while **this) {
    AST_NODE_DESTROY(waitui_ast_while);

    waitui_ast_expression_destroy(&(*this)->condition);
    waitui_ast_expression_destroy(&(*this)->body);

    AST_NODE_DESTROY_DONE(waitui_ast_while);
}

waitui_ast_binary_expression *
waitui_ast_binary_expression_new(waitui_ast_expression *left,
                                 waitui_ast_binary_operator operator,
                                 waitui_ast_expression * right) {
    AST_NODE_NEW(waitui_ast_binary_expression, EXPRESSION, BINARY_EXPRESSION);

    this->left    = left;
    this->operator= operator;
    this->right   = right;

    AST_NODE_NEW_DONE(waitui_ast_binary_expression);
}

waitui_ast_expression *
waitui_ast_binary_expression_getLeft(waitui_ast_binary_expression *this) {
    WAITUI_AST_NODE_GET(waitui_ast_binary_expression, NULL);
    return this->left;
}

waitui_ast_binary_operator
waitui_ast_binary_expression_getOperator(waitui_ast_binary_expression *this) {
    WAITUI_AST_NODE_GET(waitui_ast_binary_expression,
                        WAITUI_AST_BINARY_OPERATOR_UNDEFINED);
    return this->operator;
}

waitui_ast_expression *
waitui_ast_binary_expression_getRight(waitui_ast_binary_expression *this) {
    WAITUI_AST_NODE_GET(waitui_ast_binary_expression, NULL);
    return this->right;
}

void waitui_ast_binary_expression_destroy(waitui_ast_binary_expression **this) {
    AST_NODE_DESTROY(waitui_ast_binary_expression);

    waitui_ast_expression_destroy(&(*this)->left);
    waitui_ast_expression_destroy(&(*this)->right);

    AST_NODE_DESTROY_DONE(waitui_ast_binary_expression);
}

waitui_ast_unary_expression *
waitui_ast_unary_expression_new(waitui_ast_unary_operator operator,
                                waitui_ast_expression * expression) {
    AST_NODE_NEW(waitui_ast_unary_expression, EXPRESSION, UNARY_EXPRESSION);

    this->operator   = operator;
    this->expression = expression;

    AST_NODE_NEW_DONE(waitui_ast_unary_expression);
}

waitui_ast_unary_operator
waitui_ast_unary_expression_getOperator(waitui_ast_unary_expression *this) {
    WAITUI_AST_NODE_GET(waitui_ast_unary_expression,
                        WAITUI_AST_UNARY_OPERATOR_UNDEFINED);
    return this->operator;
}

waitui_ast_expression *
waitui_ast_unary_expression_getExpression(waitui_ast_unary_expression *this) {
    WAITUI_AST_NODE_GET(waitui_ast_unary_expression, NULL);
    return this->expression;
}

void waitui_ast_unary_expression_destroy(waitui_ast_unary_expression **this) {
    AST_NODE_DESTROY(waitui_ast_unary_expression);

    waitui_ast_expression_destroy(&(*this)->expression);

    AST_NODE_DESTROY_DONE(waitui_ast_unary_expression);
}

waitui_ast_lazy_expression *
waitui_ast_lazy_expression_new(waitui_ast_expression *expression,
                               void *context) {
    AST_NODE_NEW(waitui_ast_lazy_expression, EXPRESSION, LAZY_EXPRESSION);

    this->expression = expression;
    this->context    = context;

    AST_NODE_NEW_DONE(waitui_ast_lazy_expression);
}

void waitui_ast_lazy_expression_destroy(waitui_ast_lazy_expression **this) {
    AST_NODE_DESTROY(waitui_ast_lazy_expression);

    waitui_ast_expression_destroy(&(*this)->expression);

    AST_NODE_DESTROY_DONE(waitui_ast_lazy_expression);
}

waitui_ast_native_expression *waitui_ast_native_expression_new(void *func) {
    AST_NODE_NEW(waitui_ast_native_expression, EXPRESSION, NATIVE_EXPRESSION);

    this->func = func;

    AST_NODE_NEW_DONE(waitui_ast_native_expression);
}

void waitui_ast_native_expression_destroy(waitui_ast_native_expression **this) {
    AST_NODE_DESTROY(waitui_ast_native_expression);

    AST_NODE_DESTROY_DONE(waitui_ast_native_expression);
}

waitui_ast_constructor_call *
waitui_ast_constructor_call_new(symbol *name,
                                waitui_ast_expression_list *args) {
    AST_NODE_NEW(waitui_ast_constructor_call, EXPRESSION, CONSTRUCTOR_CALL);

    this->name = name;
    this->args = args;

    symbol_increment_refcount(name);

    AST_NODE_NEW_DONE(waitui_ast_constructor_call);
}

symbol *waitui_ast_constructor_call_getName(waitui_ast_constructor_call *this) {
    WAITUI_AST_NODE_GET(waitui_ast_constructor_call, NULL);
    return this->name;
}

waitui_ast_expression_list *
waitui_ast_constructor_call_getArgs(waitui_ast_constructor_call *this) {
    WAITUI_AST_NODE_GET(waitui_ast_constructor_call, NULL);
    return this->args;
}

void waitui_ast_constructor_call_destroy(waitui_ast_constructor_call **this) {
    AST_NODE_DESTROY(waitui_ast_constructor_call);

    symbol_decrement_refcount(&(*this)->name);

    waitui_ast_expression_list_destroy(&(*this)->args);

    AST_NODE_DESTROY_DONE(waitui_ast_constructor_call);
}

waitui_ast_function_call *
waitui_ast_function_call_new(waitui_ast_expression *object,
                             symbol *functionName,
                             waitui_ast_expression_list *args) {
    AST_NODE_NEW(waitui_ast_function_call, EXPRESSION, FUNCTION_CALL);

    this->object       = object;
    this->functionName = functionName;
    this->args         = args;

    symbol_increment_refcount(functionName);

    AST_NODE_NEW_DONE(waitui_ast_function_call);
}

waitui_ast_expression *
waitui_ast_function_call_getObject(waitui_ast_function_call *this) {
    WAITUI_AST_NODE_GET(waitui_ast_function_call, NULL);
    return this->object;
}

symbol *
waitui_ast_function_call_getFunctionName(waitui_ast_function_call *this) {
    WAITUI_AST_NODE_GET(waitui_ast_function_call, NULL);
    return this->functionName;
}

waitui_ast_expression_list *
waitui_ast_function_call_getArgs(waitui_ast_function_call *this) {
    WAITUI_AST_NODE_GET(waitui_ast_function_call, NULL);
    return this->args;
}

void waitui_ast_function_call_destroy(waitui_ast_function_call **this) {
    AST_NODE_DESTROY(waitui_ast_function_call);

    symbol_decrement_refcount(&(*this)->functionName);

    waitui_ast_expression_destroy(&(*this)->object);
    waitui_ast_expression_list_destroy(&(*this)->args);

    AST_NODE_DESTROY_DONE(waitui_ast_function_call);
}

waitui_ast_super_function_call *
waitui_ast_super_function_call_new(symbol *functionName,
                                   waitui_ast_expression_list *args) {
    AST_NODE_NEW(waitui_ast_super_function_call, EXPRESSION,
                 SUPER_FUNCTION_CALL);

    this->functionName = functionName;
    this->args         = args;

    symbol_increment_refcount(functionName);

    AST_NODE_NEW_DONE(waitui_ast_super_function_call);
}

symbol *waitui_ast_super_function_call_getFunctionName(
        waitui_ast_super_function_call *this) {
    WAITUI_AST_NODE_GET(waitui_ast_super_function_call, NULL);
    return this->functionName;
}

waitui_ast_expression_list *
waitui_ast_super_function_call_getArgs(waitui_ast_super_function_call *this) {
    WAITUI_AST_NODE_GET(waitui_ast_super_function_call, NULL);
    return this->args;
}

void waitui_ast_super_function_call_destroy(
        waitui_ast_super_function_call **this) {
    AST_NODE_DESTROY(waitui_ast_super_function_call);

    symbol_decrement_refcount(&(*this)->functionName);

    waitui_ast_expression_list_destroy(&(*this)->args);

    AST_NODE_DESTROY_DONE(waitui_ast_super_function_call);
}

waitui_ast_reference *waitui_ast_reference_new(symbol *value) {
    AST_NODE_NEW(waitui_ast_reference, EXPRESSION, REFERENCE);

    this->value = value;

    symbol_increment_refcount(value);

    AST_NODE_NEW_DONE(waitui_ast_reference);
}

symbol *waitui_ast_reference_getValue(waitui_ast_reference *this) {
    WAITUI_AST_NODE_GET(waitui_ast_reference, NULL);
    return this->value;
}

void waitui_ast_reference_destroy(waitui_ast_reference **this) {
    AST_NODE_DESTROY(waitui_ast_reference);

    symbol_decrement_refcount(&(*this)->value);

    AST_NODE_DESTROY_DONE(waitui_ast_reference);
}

waitui_ast_this_literal *waitui_ast_this_literal_new(void) {
    AST_NODE_NEW(waitui_ast_this_literal, EXPRESSION, THIS_LITERAL);

    AST_NODE_NEW_DONE(waitui_ast_this_literal);
}

void waitui_ast_this_literal_destroy(waitui_ast_this_literal **this) {
    AST_NODE_DESTROY(waitui_ast_this_literal);

    AST_NODE_DESTROY_DONE(waitui_ast_this_literal);
}

waitui_ast_integer_literal *waitui_ast_integer_literal_new(str value) {
    AST_NODE_NEW(waitui_ast_integer_literal, EXPRESSION, INTEGER_LITERAL);

    STR_COPY(&this->value, &value);
    if (!this->value.s) {
        waitui_ast_integer_literal_destroy(&this);
        return NULL;
    }

    AST_NODE_NEW_DONE(waitui_ast_integer_literal);
}

str *waitui_ast_integer_literal_getValue(waitui_ast_integer_literal *this) {
    WAITUI_AST_NODE_GET(waitui_ast_integer_literal, NULL);
    return &this->value;
}

void waitui_ast_integer_literal_destroy(waitui_ast_integer_literal **this) {
    AST_NODE_DESTROY(waitui_ast_integer_literal);

    STR_FREE(&(*this)->value);

    AST_NODE_DESTROY_DONE(waitui_ast_integer_literal);
}

waitui_ast_boolean_literal *waitui_ast_boolean_literal_new(bool value) {
    AST_NODE_NEW(waitui_ast_boolean_literal, EXPRESSION, BOOLEAN_LITERAL);

    this->value = value;

    AST_NODE_NEW_DONE(waitui_ast_boolean_literal);
}

bool waitui_ast_boolean_literal_getValue(waitui_ast_boolean_literal *this) {
    WAITUI_AST_NODE_GET(waitui_ast_boolean_literal, true);
    return this->value;
}

void waitui_ast_boolean_literal_destroy(waitui_ast_boolean_literal **this) {
    AST_NODE_DESTROY(waitui_ast_boolean_literal);

    AST_NODE_DESTROY_DONE(waitui_ast_boolean_literal);
}

waitui_ast_decimal_literal *waitui_ast_decimal_literal_new(str value) {
    AST_NODE_NEW(waitui_ast_decimal_literal, EXPRESSION, DECIMAL_LITERAL);

    STR_COPY(&this->value, &value);
    if (!this->value.s) {
        waitui_ast_decimal_literal_destroy(&this);
        return NULL;
    }

    AST_NODE_NEW_DONE(waitui_ast_decimal_literal);
}

str *waitui_ast_decimal_literal_getValue(waitui_ast_decimal_literal *this) {
    WAITUI_AST_NODE_GET(waitui_ast_decimal_literal, NULL);
    return &this->value;
}

void waitui_ast_decimal_literal_destroy(waitui_ast_decimal_literal **this) {
    AST_NODE_DESTROY(waitui_ast_decimal_literal);

    STR_FREE(&(*this)->value);

    AST_NODE_DESTROY_DONE(waitui_ast_decimal_literal);
}

waitui_ast_null_literal *waitui_ast_null_literal_new(void) {
    AST_NODE_NEW(waitui_ast_null_literal, EXPRESSION, NULL_LITERAL);

    AST_NODE_NEW_DONE(waitui_ast_null_literal);
}

void waitui_ast_null_literal_destroy(waitui_ast_null_literal **this) {
    AST_NODE_DESTROY(waitui_ast_null_literal);

    AST_NODE_DESTROY_DONE(waitui_ast_null_literal);
}

waitui_ast_string_literal *waitui_ast_string_literal_new(str value) {
    AST_NODE_NEW(waitui_ast_string_literal, EXPRESSION, STRING_LITERAL);

    STR_COPY(&this->value, &value);
    if (!this->value.s) {
        waitui_ast_string_literal_destroy(&this);
        return NULL;
    }

    AST_NODE_NEW_DONE(waitui_ast_string_literal);
}

str *waitui_ast_string_literal_getValue(waitui_ast_string_literal *this) {
    WAITUI_AST_NODE_GET(waitui_ast_string_literal, NULL);
    return &this->value;
}

void waitui_ast_string_literal_destroy(waitui_ast_string_literal **this) {
    AST_NODE_DESTROY(waitui_ast_string_literal);

    STR_FREE(&(*this)->value);

    AST_NODE_DESTROY_DONE(waitui_ast_string_literal);
}
