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
    this->astDefinitionType = AST_DEFINITION_TYPE_##node_subtype

#define AST_NODE_EXPRESSION_INIT(node_subtype)                                 \
    this->astExpressionType = AST_EXPRESSION_TYPE_##node_subtype

#define AST_NODE_ALLOC_INIT(type, node_type, node_subtype)                     \
    type *this = calloc(1, sizeof(*this));                                     \
    if (!this) { return NULL; }                                                \
                                                                               \
    this->astNodeType = AST_NODE_TYPE_##node_type;                             \
    AST_NODE_##node_type##_INIT(node_subtype)

#define AST_NODE_FREE()                                                        \
    free(*this);                                                               \
    *this = NULL


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

CREATE_LIST_TYPE(IMPLEMENTATION, ast_namespace, namespace)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_import, import)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_class, class)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_expression, expression)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_formal, formal)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_property, property)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_function, function)
CREATE_LIST_TYPE(IMPLEMENTATION, ast_initialization, initialization)

void ast_node_destroy(ast_node **this) {
    if (!this || !(*this)) { return; }

    switch ((*this)->astNodeType) {
        case AST_NODE_TYPE_EXPRESSION:
            ast_expression_destroy((ast_expression **) this);
            break;
        case AST_NODE_TYPE_DEFINITION:
            ast_definition_destroy((ast_definition **) this);
            break;
    }
}

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
            ast_class_destroy((ast_class **) this);
            break;
        case AST_DEFINITION_TYPE_PROGRAM:
            ast_program_destroy((ast_program **) this);
            break;
        case AST_DEFINITION_TYPE_IMPORT:
            ast_import_destroy((ast_import **) this);
            break;
        case AST_DEFINITION_TYPE_NAMESPACE:
            ast_namespace_destroy((ast_namespace **) this);
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

ast_program *ast_program_new(ast_namespace_list *namespaces) {
    log_trace("creating new ast_program");

    AST_NODE_ALLOC_INIT(ast_program, DEFINITION, PROGRAM);

    this->namespaces = namespaces;

    log_trace("new ast_program successful created");

    return this;
}

void ast_program_destroy(ast_program **this) {
    log_trace("destroying ast_program");

    if (!this || !(*this)) { return; }

    ast_namespace_list_destroy(&(*this)->namespaces);

    AST_NODE_FREE();

    log_trace("ast_program successful destroyed");
}

ast_namespace *ast_namespace_new(symbol *name, ast_import_list *imports,
                                 ast_class_list *classes) {
    log_trace("creating new ast_namespace");

    AST_NODE_ALLOC_INIT(ast_namespace, DEFINITION, NAMESPACE);

    this->name    = name;
    this->imports = imports;
    this->classes = classes;

    log_trace("new ast_namespace successful created");

    return this;
}

void ast_namespace_destroy(ast_namespace **this) {
    log_trace("destroying ast_namespace");

    if (!this || !(*this)) { return; }

    ast_import_list_destroy(&(*this)->imports);
    ast_class_list_destroy(&(*this)->classes);

    AST_NODE_FREE();

    log_trace("ast_namespace successful destroyed");
}

ast_import *ast_import_new(void) {
    log_trace("creating new ast_import");

    AST_NODE_ALLOC_INIT(ast_import, DEFINITION, IMPORT);

    log_trace("new ast_import successful created");

    return this;
}

void ast_import_destroy(ast_import **this) {
    log_trace("destroying ast_import");

    if (!this || !(*this)) { return; }

    AST_NODE_FREE();

    log_trace("ast_import successful destroyed");
}

ast_class *ast_class_new(symbol *name, ast_formal_list *parameters,
                         symbol *superClass,
                         ast_expression_list *superClassArgs,
                         ast_property_list *properties,
                         ast_function_list *functions) {
    log_trace("creating new ast_class");

    AST_NODE_ALLOC_INIT(ast_class, DEFINITION, CLASS);

    this->name           = name;
    this->parameters     = parameters;
    this->superClass     = superClass;
    this->superClassArgs = superClassArgs;
    this->properties     = properties;
    this->functions      = functions;

    symbol_increment_refcount(name);
    symbol_increment_refcount(superClass);

    log_trace("new ast_class successful created");

    return this;
}

void ast_class_set_name(ast_class *this, symbol *name) {
    log_trace("ast_class setting name");

    if (!this || !name) { return; }

    if (this->name) { symbol_decrement_refcount(&this->name); }
    this->name = name;
    symbol_increment_refcount(name);

    log_trace("ast_class set name");
}

void ast_class_set_parameters(ast_class *this, ast_formal_list *parameters) {
    log_trace("ast_class setting parameters");

    if (!this || !parameters) { return; }

    if (this->parameters) { ast_formal_list_destroy(&this->parameters); }
    this->parameters = parameters;

    log_trace("ast_class set parameters");
}

void ast_class_set_super_class(ast_class *this, symbol *superClass) {
    log_trace("ast_class setting superClass");

    if (!this || !superClass) { return; }

    if (this->superClass) { symbol_decrement_refcount(&this->superClass); }
    this->superClass = superClass;
    symbol_increment_refcount(superClass);

    log_trace("ast_class set superClass");
}

void ast_class_set_super_class_args(ast_class *this,
                                    ast_expression_list *superClassArgs) {
    log_trace("ast_class setting superClassArgs");

    if (!this || !superClassArgs) { return; }

    if (this->superClassArgs) {
        ast_expression_list_destroy(&this->superClassArgs);
    }
    this->superClassArgs = superClassArgs;

    log_trace("ast_class set superClassArgs");
}

void ast_class_destroy(ast_class **this) {
    log_trace("destroying ast_class");

    if (!this || !(*this)) { return; }

    symbol_decrement_refcount(&(*this)->name);
    ast_formal_list_destroy(&(*this)->parameters);
    symbol_decrement_refcount(&(*this)->superClass);
    ast_expression_list_destroy(&(*this)->superClassArgs);
    ast_property_list_destroy(&(*this)->properties);
    ast_function_list_destroy(&(*this)->functions);

    AST_NODE_FREE();

    log_trace("ast_class successful destroyed");
}

ast_formal *ast_formal_new(symbol *identifier, symbol *type, bool isLazy) {
    AST_NODE_ALLOC_INIT(ast_formal, DEFINITION, FORMAL);

    this->identifier = identifier;
    this->type       = type;
    this->isLazy     = isLazy;

    symbol_increment_refcount(identifier);
    symbol_increment_refcount(type);

    return this;
}

void ast_formal_destroy(ast_formal **this) {
    if (!this || !(*this)) { return; }

    symbol_decrement_refcount(&(*this)->identifier);
    symbol_decrement_refcount(&(*this)->type);

    AST_NODE_FREE();
}

ast_function *ast_function_new(symbol *functionName,
                               ast_formal_list *parameters, symbol *returnType,
                               ast_expression *body,
                               ast_function_visibility visibility,
                               bool isAbstract, bool isFinal,
                               bool isOverwrite) {
    AST_NODE_ALLOC_INIT(ast_function, DEFINITION, FUNCTION);

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

    return this;
}

void ast_function_set_body(ast_function *this, ast_expression *body) {
    if (!this) { return; }

    if (this->body) { ast_expression_destroy(&this->body); }

    this->body = body;
}

void ast_function_set_visibility(ast_function *this,
                                 ast_function_visibility visibility) {
    if (!this) { return; }

    this->visibility = visibility;
}

void ast_function_set_abstract(ast_function *this, bool isAbstract) {
    if (!this) { return; }

    this->isAbstract = isAbstract;
}

void ast_function_set_final(ast_function *this, bool isFinal) {
    if (!this) { return; }

    this->isFinal = isFinal;
}

void ast_function_set_overwrite(ast_function *this, bool isOverwrite) {
    if (!this) { return; }

    this->isOverwrite = isOverwrite;
}

void ast_function_destroy(ast_function **this) {
    if (!this || !(*this)) { return; }

    symbol_decrement_refcount(&(*this)->functionName);
    ast_formal_list_destroy(&(*this)->parameters);
    symbol_decrement_refcount(&(*this)->returnType);
    ast_expression_destroy(&(*this)->body);

    AST_NODE_FREE();
}

ast_property *ast_property_new(symbol *name, symbol *type,
                               ast_expression *value) {
    AST_NODE_ALLOC_INIT(ast_property, DEFINITION, PROPERTY);

    this->name  = name;
    this->type  = type;
    this->value = value;

    symbol_increment_refcount(name);
    symbol_increment_refcount(type);

    return this;
}

void ast_property_destroy(ast_property **this) {
    if (!this || !(*this)) { return; }

    symbol_decrement_refcount(&(*this)->name);
    symbol_decrement_refcount(&(*this)->type);
    ast_expression_destroy(&(*this)->value);

    AST_NODE_FREE();
}

ast_block *ast_block_new(ast_expression_list *expressions) {
    AST_NODE_ALLOC_INIT(ast_block, EXPRESSION, BLOCK);

    this->expressions = expressions;

    return this;
}

void ast_block_destroy(ast_block **this) {
    if (!this || !(*this)) { return; }

    ast_expression_list_destroy(&(*this)->expressions);

    AST_NODE_FREE();
}

ast_let *ast_let_new(ast_initialization_list *initializations,
                     ast_expression *body) {
    AST_NODE_ALLOC_INIT(ast_let, EXPRESSION, LET);

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

ast_initialization *ast_initialization_new(symbol *identifier, symbol *type,
                                           ast_expression *value) {
    AST_NODE_ALLOC_INIT(ast_initialization, EXPRESSION, INITIALIZATION);

    this->identifier = identifier;
    this->type       = type;
    this->value      = value;

    symbol_increment_refcount(identifier);
    symbol_increment_refcount(type);

    return this;
}

void ast_initialization_destroy(ast_initialization **this) {
    if (!this || !(*this)) { return; }

    symbol_decrement_refcount(&(*this)->identifier);
    symbol_decrement_refcount(&(*this)->type);
    ast_expression_destroy(&(*this)->value);

    AST_NODE_FREE();
}

ast_assignment *ast_assignment_new(symbol *identifier,
                                   ast_assignment_operator operator,
                                   ast_expression * value) {
    AST_NODE_ALLOC_INIT(ast_assignment, EXPRESSION, ASSIGNMENT);

    this->identifier = identifier;
    this->operator   = operator;
    this->value      = value;

    symbol_increment_refcount(identifier);

    return this;
}

void ast_assignment_destroy(ast_assignment **this) {
    if (!this || !(*this)) { return; }

    symbol_decrement_refcount(&(*this)->identifier);
    ast_expression_destroy(&(*this)->value);

    AST_NODE_FREE();
}

ast_cast *ast_cast_new(ast_expression *object, symbol *type) {
    AST_NODE_ALLOC_INIT(ast_cast, EXPRESSION, CAST);

    this->object = object;
    this->type   = type;

    symbol_increment_refcount(type);

    return this;
}

void ast_cast_destroy(ast_cast **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->object);
    symbol_decrement_refcount(&(*this)->type);

    AST_NODE_FREE();
}

ast_if_else *ast_if_else_new(ast_expression *condition,
                             ast_expression *thenBranch,
                             ast_expression *elseBranch) {
    AST_NODE_ALLOC_INIT(ast_if_else, EXPRESSION, IF_ELSE);

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

ast_while *ast_while_new(ast_expression *condition, ast_expression *body) {
    AST_NODE_ALLOC_INIT(ast_while, EXPRESSION, WHILE);

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

ast_binary_expression *ast_binary_expression_new(ast_expression *left,
                                                 ast_binary_operator operator,
                                                 ast_expression * right) {
    AST_NODE_ALLOC_INIT(ast_binary_expression, EXPRESSION, BINARY_EXPRESSION);

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

ast_unary_expression *ast_unary_expression_new(ast_unary_operator operator,
                                               ast_expression * expression) {
    AST_NODE_ALLOC_INIT(ast_unary_expression, EXPRESSION, UNARY_EXPRESSION);

    this->operator   = operator;
    this->expression = expression;

    return this;
}

void ast_unary_expression_destroy(ast_unary_expression **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->expression);

    AST_NODE_FREE();
}

ast_lazy_expression *ast_lazy_expression_new(ast_expression *expression,
                                             void *context) {
    AST_NODE_ALLOC_INIT(ast_lazy_expression, EXPRESSION, LAZY_EXPRESSION);

    this->expression = expression;
    this->context    = context;

    return this;
}

void ast_lazy_expression_destroy(ast_lazy_expression **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->expression);

    AST_NODE_FREE();
}

ast_native_expression *ast_native_expression_new(void *func) {
    AST_NODE_ALLOC_INIT(ast_native_expression, EXPRESSION, NATIVE_EXPRESSION);

    this->func = func;

    return this;
}

void ast_native_expression_destroy(ast_native_expression **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

ast_constructor_call *ast_constructor_call_new(symbol *name,
                                               ast_expression_list *args) {
    AST_NODE_ALLOC_INIT(ast_constructor_call, EXPRESSION, CONSTRUCTOR_CALL);

    this->name = name;
    this->args = args;

    symbol_increment_refcount(name);

    return this;
}

void ast_constructor_call_destroy(ast_constructor_call **this) {
    if (!this || !(*this)) { return; }

    symbol_decrement_refcount(&(*this)->name);
    ast_expression_list_destroy(&(*this)->args);

    AST_NODE_FREE();
}

ast_function_call *ast_function_call_new(ast_expression *object,
                                         symbol *functionName,
                                         ast_expression_list *args) {
    AST_NODE_ALLOC_INIT(ast_function_call, EXPRESSION, FUNCTION_CALL);

    this->object       = object;
    this->functionName = functionName;
    this->args         = args;

    symbol_increment_refcount(functionName);

    return this;
}

void ast_function_call_destroy(ast_function_call **this) {
    if (!this || !(*this)) { return; }

    ast_expression_destroy(&(*this)->object);
    symbol_decrement_refcount(&(*this)->functionName);
    ast_expression_list_destroy(&(*this)->args);

    AST_NODE_FREE();
}

ast_super_function_call *
ast_super_function_call_new(symbol *functionName, ast_expression_list *args) {
    AST_NODE_ALLOC_INIT(ast_super_function_call, EXPRESSION,
                        SUPER_FUNCTION_CALL);

    this->functionName = functionName;
    this->args         = args;

    symbol_increment_refcount(functionName);

    return this;
}

void ast_super_function_call_destroy(ast_super_function_call **this) {
    if (!this || !(*this)) { return; }

    symbol_decrement_refcount(&(*this)->functionName);
    ast_expression_list_destroy(&(*this)->args);

    AST_NODE_FREE();
}

ast_reference *ast_reference_new(symbol *value) {
    AST_NODE_ALLOC_INIT(ast_reference, EXPRESSION, REFERENCE);

    this->value = value;
    symbol_increment_refcount(value);

    return this;
}

void ast_reference_destroy(ast_reference **this) {
    if (!this || !(*this)) { return; }

    symbol_decrement_refcount(&(*this)->value);

    AST_NODE_FREE();
}

ast_this_literal *ast_this_literal_new(void) {
    AST_NODE_ALLOC_INIT(ast_this_literal, EXPRESSION, THIS_LITERAL);

    return this;
}

void ast_this_literal_destroy(ast_this_literal **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

ast_integer_literal *ast_integer_literal_new(str value) {
    AST_NODE_ALLOC_INIT(ast_integer_literal, EXPRESSION, INTEGER_LITERAL);

    STR_COPY(&this->value, &value);
    if (!this->value.s) {
        ast_integer_literal_destroy(&this);
        return NULL;
    }

    return this;
}

void ast_integer_literal_destroy(ast_integer_literal **this) {
    if (!this || !(*this)) { return; }

    STR_FREE(&(*this)->value);

    AST_NODE_FREE();
}

ast_boolean_literal *ast_boolean_literal_new(bool value) {
    AST_NODE_ALLOC_INIT(ast_boolean_literal, EXPRESSION, BOOLEAN_LITERAL);

    this->value = value;

    return this;
}

void ast_boolean_literal_destroy(ast_boolean_literal **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

ast_decimal_literal *ast_decimal_literal_new(str value) {
    AST_NODE_ALLOC_INIT(ast_decimal_literal, EXPRESSION, DECIMAL_LITERAL);

    STR_COPY(&this->value, &value);
    if (!this->value.s) {
        ast_decimal_literal_destroy(&this);
        return NULL;
    }

    return this;
}

void ast_decimal_literal_destroy(ast_decimal_literal **this) {
    if (!this || !(*this)) { return; }

    STR_FREE(&(*this)->value);

    AST_NODE_FREE();
}

ast_null_literal *ast_null_literal_new(void) {
    AST_NODE_ALLOC_INIT(ast_null_literal, EXPRESSION, NULL_LITERAL);

    return this;
}

void ast_null_literal_destroy(ast_null_literal **this) {
    if (!this || !(*this)) { return; }

    AST_NODE_FREE();
}

ast_string_literal *ast_string_literal_new(str value) {
    AST_NODE_ALLOC_INIT(ast_string_literal, EXPRESSION, STRING_LITERAL);

    STR_COPY(&this->value, &value);
    if (!this->value.s) {
        ast_string_literal_destroy(&this);
        return NULL;
    }

    return this;
}

void ast_string_literal_destroy(ast_string_literal **this) {
    if (!this || !(*this)) { return; }

    STR_FREE(&(*this)->value);

    AST_NODE_FREE();
}
