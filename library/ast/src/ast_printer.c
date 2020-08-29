/**
 * @file ast_printer.c
 * @author rick
 * @date 04.08.20
 * @brief File for the AST printer implementation
 */

#include "waitui/ast_printer.h"


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
 * @brief Print the Program AST node.
 * @param[in] program The Program AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_program_printer(ast_program *program, void *args) {
    ast_printer *printer = (ast_printer *) args;
    list_node *node      = NULL;

    fprintf(printer->outFile, "digraph AST {\n");
    fprintf(printer->outFile, "\tnode [shape=plain]\n");

    fprintf(printer->outFile,
            "\tprogram "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR><TD><B>%s</B></TD></TR>"
            "</TABLE>"
            ">];\n",
            "ast_program");

    fprintf(printer->outFile, "\tprogram -> program_classes;\n");
    fprintf(printer->outFile,
            "\tprogram_classes "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            "classes");

    for (node = program->classes->head; node; node = node->next) {
        ast_class *class = (ast_class *) node->element;
        fprintf(printer->outFile, "\tprogram_classes -> ");
        AST_EXECUTE_CALLBACKS(printer->callbacks, class, args);
    }

    fprintf(printer->outFile, "}\n");
}

/**
 * @brief Print the Class AST node.
 * @param[in] class The Class AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_class_printer(ast_class *class, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    str className      = STR_NULL_INIT;
    str superClassName = STR_NULL_INIT;

    if (class->name) { className = class->name->identifier; }
    if (class->superClass) { superClassName = class->superClass->identifier; }

    fprintf(printer->outFile, "class%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tclass%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">name:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">super:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s "
            "</TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_class", STR_FMT(&className),
            STR_FMT(&superClassName));

    fprintf(printer->outFile, "\tclass%llu -> class_parameters%llu;\n",
            nodeCount, nodeCount);
    fprintf(printer->outFile,
            "\tclass_parameters%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "parameters");

    if (class->parameters) {
        for (node = class->parameters->head; node; node = node->next) {
            ast_formal *formal = (ast_formal *) node->element;
            fprintf(printer->outFile, "\tclass_parameters%llu -> ", nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks, formal, args);
        }
    }

    fprintf(printer->outFile, "\tclass%llu -> class_super_class_args%llu;\n",
            nodeCount, nodeCount);
    fprintf(printer->outFile,
            "\tclass_super_class_args%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "superClassArgs");
    if (class->superClassArgs) {
        for (node = class->superClassArgs->head; node; node = node->next) {
            ast_expression *superClassArg = (ast_expression *) node->element;
            fprintf(printer->outFile, "\tclass_super_class_args%llu -> ",
                    nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks, superClassArg, args);
        }
    }

    fprintf(printer->outFile, "\tclass%llu -> class_properties%llu;\n",
            nodeCount, nodeCount);
    fprintf(printer->outFile,
            "\tclass_properties%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "properties");
    if (class->properties) {
        for (node = class->properties->head; node; node = node->next) {
            ast_property *property = (ast_property *) node->element;
            fprintf(printer->outFile, "\tclass_properties%llu -> ", nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks, property, args);
        }
    }

    fprintf(printer->outFile, "\tclass%llu -> class_functions%llu;\n",
            nodeCount, nodeCount);
    fprintf(printer->outFile,
            "\tclass_functions%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "functions");
    if (class->functions) {
        for (node = class->functions->head; node; node = node->next) {
            ast_function *function = (ast_function *) node->element;
            fprintf(printer->outFile, "\tclass_functions%llu -> ", nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks, function, args);
        }
    }
}

/**
 * @brief Print the Formal AST node.
 * @param[in] formal The Formal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_formal_printer(ast_formal *formal, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str identifier = STR_NULL_INIT;
    str type       = STR_NULL_INIT;

    if (formal->identifier) { identifier = formal->identifier->identifier; }
    if (formal->type) { type = formal->type->identifier; }

    fprintf(printer->outFile, "formal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tformal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">identifier:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">type:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">isLazy:</TD>"
            "<TD ALIGN=\"LEFT\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_formal", STR_FMT(&identifier), STR_FMT(&type),
            (formal->isLazy ? "true" : "false"));
}

/**
 * @brief Print the Property AST node.
 * @param[in] property The Property AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_property_printer(ast_property *property, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str name = STR_NULL_INIT;
    str type = STR_NULL_INIT;

    if (property->name) { name = property->name->identifier; }
    if (property->type) { type = property->type->identifier; }

    fprintf(printer->outFile, "formal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tformal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">name:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">type:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_formal", STR_FMT(&name), STR_FMT(&type));

    if (property->value) {
        fprintf(printer->outFile, "\tformal%llu -> ", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, property->value, args);
    }
}

/**
 * @brief Print the Function AST node.
 * @param[in] function The Function AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_function_printer(ast_function *function, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    str name       = STR_NULL_INIT;
    str returnType = STR_NULL_INIT;
    str visibility = STR_NULL_INIT;

    if (function->functionName) { name = function->functionName->identifier; }
    if (function->returnType) { returnType = function->returnType->identifier; }

    switch (function->visibility) {
        case AST_FUNCTION_VISIBILITY_PUBLIC:
            STR_STATIC_SET(&visibility, "public");
            break;
        case AST_FUNCTION_VISIBILITY_PROTECTED:
            STR_STATIC_SET(&visibility, "protected");
            break;
        case AST_FUNCTION_VISIBILITY_PRIVATE:
            STR_STATIC_SET(&visibility, "private");
            break;
    }

    fprintf(printer->outFile, "function%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tfunction%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">name:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">returnType:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">visibility:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">isAbstract:</TD>"
            "<TD ALIGN=\"LEFT\"> %s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">isFinal:</TD>"
            "<TD ALIGN=\"LEFT\"> %s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">isOverwrite:</TD>"
            "<TD ALIGN=\"LEFT\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_function", STR_FMT(&name), STR_FMT(&returnType),
            STR_FMT(&visibility), (function->isAbstract ? "true" : "false"),
            (function->isFinal ? "true" : "false"),
            (function->isOverwrite ? "true" : "false"));

    fprintf(printer->outFile, "\tfunction%llu -> function_parameters%llu;\n",
            nodeCount, nodeCount);
    fprintf(printer->outFile,
            "\tfunction_parameters%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "parameters");

    if (function->parameters) {
        for (node = function->parameters->head; node; node = node->next) {
            ast_formal *formal = (ast_formal *) node->element;
            fprintf(printer->outFile, "\tfunction_parameters%llu -> ",
                    nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks, formal, args);
        }
    }

    if (function->body) {
        fprintf(printer->outFile, "\tfunction%llu -> ", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, function->body, args);
    }
}

/**
 * @brief Print the Assignment AST node.
 * @param[in] assignment The Assignment AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_assignment_printer(ast_assignment *assignment, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str identifier = STR_NULL_INIT;
    str operator   = STR_NULL_INIT;

    if (assignment->identifier) {
        identifier = assignment->identifier->identifier;
    }

    switch (assignment->operator) {
        case AST_ASSIGNMENT_OPERATOR_EQUAL:
            STR_STATIC_SET(&operator, "=");
            break;
        case AST_ASSIGNMENT_OPERATOR_PLUS_EQUAL:
            STR_STATIC_SET(&operator, "+=");
            break;
        case AST_ASSIGNMENT_OPERATOR_MINUS_EQUAL:
            STR_STATIC_SET(&operator, "-=");
            break;
        case AST_ASSIGNMENT_OPERATOR_TIMES_EQUAL:
            STR_STATIC_SET(&operator, "*=");
            break;
        case AST_ASSIGNMENT_OPERATOR_DIV_EQUAL:
            STR_STATIC_SET(&operator, "/=");
            break;
        case AST_ASSIGNMENT_OPERATOR_MODULO_EQUAL:
            STR_STATIC_SET(&operator, "%=");
            break;
        case AST_ASSIGNMENT_OPERATOR_AND_EQUAL:
            STR_STATIC_SET(&operator, "&=");
            break;
        case AST_ASSIGNMENT_OPERATOR_CARET_EQUAL:
            STR_STATIC_SET(&operator, "^=");
            break;
        case AST_ASSIGNMENT_OPERATOR_TILDE_EQUAL:
            STR_STATIC_SET(&operator, "~=");
            break;
        case AST_ASSIGNMENT_OPERATOR_PIPE_EQUAL:
            STR_STATIC_SET(&operator, "|=");
            break;
    }

    fprintf(printer->outFile, "assignment%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tassignment%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">identifier:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">operator:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_assignment", STR_FMT(&identifier),
            STR_FMT(&operator));

    if (assignment->value) {
        fprintf(printer->outFile, "\tassignment%llu -> ", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, assignment->value, args);
    }
}

/**
 * @brief Print the String Literal AST node.
 * @param[in] stringLiteral The String Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_string_literal_printer(ast_string_literal *stringLiteral,
                                       void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    fprintf(printer->outFile, "string_literal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tstring_literal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">value:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_string_literal", STR_FMT(&stringLiteral->value));
}

/**
 * @brief Print the Integer Literal AST node.
 * @param[in] integerLiteral The Integer Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_integer_literal_printer(ast_integer_literal *integerLiteral,
                                        void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    fprintf(printer->outFile, "integer_literal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tinteger_literal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">value:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_integer_literal", STR_FMT(&integerLiteral->value));
}

/**
 * @brief Print the Null Literal AST node.
 * @param[in] nullLiteral The Null Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_null_literal_printer(ast_null_literal *nullLiteral,
                                     void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    (void) nullLiteral;

    fprintf(printer->outFile, "null_literal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tnull_literal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">value:</TD>"
            "<TD ALIGN=\"LEFT\"> null </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_null_literal");
}

/**
 * @brief Print the Boolean Literal AST node.
 * @param[in] booleanLiteral The Boolean Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_boolean_literal_printer(ast_boolean_literal *booleanLiteral,
                                        void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    fprintf(printer->outFile, "boolean_literal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tboolean_literal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">value:</TD>"
            "<TD ALIGN=\"LEFT\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_boolean_literal",
            booleanLiteral->value ? "true" : "false");
}

/**
 * @brief Print the This Literal AST node.
 * @param[in] thisLiteral The This Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_this_literal_printer(ast_this_literal *thisLiteral,
                                     void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    (void) thisLiteral;

    fprintf(printer->outFile, "this_literal%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tthis_literal%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">value:</TD>"
            "<TD ALIGN=\"LEFT\"> this </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_this_literal");
}

/**
 * @brief Print the Reference Literal AST node.
 * @param[in] reference The Reference Literal AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_reference_printer(ast_reference *reference, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str identifier = STR_NULL_INIT;

    if (reference->value) { identifier = reference->value->identifier; }

    fprintf(printer->outFile, "reference%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\treference%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">value:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_reference", STR_FMT(&identifier));
}

/**
 * @brief Print the Cast AST node.
 * @param[in] cast The Cast AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_cast_printer(ast_cast *cast, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str type = STR_NULL_INIT;

    if (cast->type) { type = cast->type->identifier; }

    fprintf(printer->outFile, "cast%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tcast%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">type:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_cast", STR_FMT(&type));

    if (cast->object) {
        fprintf(printer->outFile, "\tcast%llu -> ", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, cast->object, args);
    }
}

/**
 * @brief Print the Block AST node.
 * @param[in] block The Block AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_block_printer(ast_block *block, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    fprintf(printer->outFile, "block%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tblock%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_block");

    fprintf(printer->outFile, "\tblock%llu -> block_expressions%llu;\n",
            nodeCount, nodeCount);
    fprintf(printer->outFile,
            "\tblock_expressions%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "expressions");

    if (block->expressions) {
        for (node = block->expressions->head; node; node = node->next) {
            ast_formal *formal = (ast_formal *) node->element;
            fprintf(printer->outFile, "\tblock_expressions%llu -> ", nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks, formal, args);
        }
    }
}

/**
 * @brief Print the Constructor Call AST node.
 * @param[in] constructorCall The Constructor Call AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_constructor_call_printer(ast_constructor_call *constructorCall,
                                         void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    str name = STR_NULL_INIT;

    if (constructorCall->name) { name = constructorCall->name->identifier; }

    fprintf(printer->outFile, "constructor_call%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tconstructor_call%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">name:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_constructor_call", STR_FMT(&name));

    fprintf(printer->outFile,
            "\tconstructor_call%llu -> constructor_call_args%llu;\n", nodeCount,
            nodeCount);
    fprintf(printer->outFile,
            "\tconstructor_call_args%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "args");
    if (constructorCall->args) {
        for (node = constructorCall->args->head; node; node = node->next) {
            ast_expression *superClassArg = (ast_expression *) node->element;
            fprintf(printer->outFile, "\tconstructor_call_args%llu -> ",
                    nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks, superClassArg, args);
        }
    }
}

/**
 * @brief Print the Let AST node.
 * @param[in] let The Let AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_let_printer(ast_let *let, void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;
    list_node *node              = NULL;

    fprintf(printer->outFile, "let%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tlet%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_let");

    fprintf(printer->outFile, "\tlet%llu -> let_initializations%llu;\n",
            nodeCount, nodeCount);
    fprintf(printer->outFile,
            "\tlet_initializations%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"> %s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "initializations");
    if (let->initializations) {
        for (node = let->initializations->head; node; node = node->next) {
            ast_expression *superClassArg = (ast_expression *) node->element;
            fprintf(printer->outFile, "\tlet_initializations%llu -> ",
                    nodeCount);
            AST_EXECUTE_CALLBACKS(printer->callbacks, superClassArg, args);
        }
    }

    if (let->body) {
        fprintf(printer->outFile, "\tlet_initializations%llu -> ", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, let->body, args);
    }
}

/**
 * @brief Print the Initialization AST node.
 * @param[in] initialization The Initialization AST node to print
 * @param[in] args The extra args for the callback call
 */
static void ast_initialization_printer(ast_initialization *initialization,
                                       void *args) {
    ast_printer *printer         = (ast_printer *) args;
    unsigned long long nodeCount = printer->nodeCount++;

    str identifier = STR_NULL_INIT;
    str type       = STR_NULL_INIT;

    if (initialization->identifier) {
        identifier = initialization->identifier->identifier;
    }
    if (initialization->type) { type = initialization->type->identifier; }

    fprintf(printer->outFile, "initialization%llu;\n", nodeCount);
    fprintf(printer->outFile,
            "\tinitialization%llu "
            "[label=<"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">"
            "<TR>"
            "<TD COLSPAN=\"2\"><B> %s </B></TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">identifier:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "<TR>"
            "<TD ALIGN=\"LEFT\">type:</TD>"
            "<TD ALIGN=\"LEFT\"> %.*s </TD>"
            "</TR>"
            "</TABLE>"
            ">];\n",
            nodeCount, "ast_initialization", STR_FMT(&identifier),
            STR_FMT(&type));

    if (initialization->value) {
        fprintf(printer->outFile, "\tinitialization%llu -> ", nodeCount);
        AST_EXECUTE_CALLBACKS(printer->callbacks, initialization->value, args);
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
                    break;
                case AST_EXPRESSION_TYPE_SUPER_FUNCTION_CALL:
                    break;
                case AST_EXPRESSION_TYPE_BINARY_EXPRESSION:
                    break;
                case AST_EXPRESSION_TYPE_UNARY_EXPRESSION:
                    break;
                case AST_EXPRESSION_TYPE_IF_ELSE:
                    break;
                case AST_EXPRESSION_TYPE_WHILE:
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

    ast_walk((ast *) this, &callbacks, &printer);
}
