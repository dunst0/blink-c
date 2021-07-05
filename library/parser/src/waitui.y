/**
 * @file waitui.y
 * @author rick
 * @date 26.03.20
 * @brief Parser for the waitui language
 */

%define api.pure full
%locations

%verbose
%define parse.trace

%parse-param { parser_extra_parser *extraParser }
%lex-param   { void *scanner }

%initial-action
{
    @$.first_line   = 1;
    @$.first_column = 1;
    @$.last_line    = 1;
    @$.last_column  = 1;
    @$.filename     = extraParser->sourceFileName;
}

%code requires {
#include "waitui/parser_helper.h"

#include <waitui/ast.h>
}

%union{
    int operator;
    str value;
    symbol *symbolValue;
    waitui_ast_program *program;
    waitui_ast_assignment *assignment;
    waitui_ast_binary_expression *binary_expression;
    waitui_ast_block *block;
    waitui_ast_cast *cast;
    waitui_ast_class *class;
    waitui_ast_class_list *classes;
    waitui_ast_constructor_call *constructor_call;
    waitui_ast_expression *expression;
    waitui_ast_expression_list *expressions;
    waitui_ast_formal *formal;
    waitui_ast_formal_list *formals;
    waitui_ast_function *function;
    waitui_ast_if_else *if_else_expression;
    waitui_ast_initialization *initialization;
    waitui_ast_initialization_list *initializations;
    waitui_ast_import *import;
    waitui_ast_import_list *imports;
    waitui_ast_namespace *namespace;
    waitui_ast_let *let;
    waitui_ast_property *property;
    waitui_ast_reference *reference;
    waitui_ast_unary_expression *unary_expression;
    waitui_ast_while *while_expression;
}

%code requires {
#define YYLTYPE YYLTYPE
typedef struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
    str filename;
} YYLTYPE;

void yyerror(YYLTYPE *locp, parser_extra_parser *extraParser, char const *msg);
}

%code {
#include "waitui/parser_impl.h"

#include "waitui/lexer_impl.h"

#include <stdlib.h>

#define scanner extraParser->scanner

#define YYLLOC_DEFAULT(Current, Rhs, N)                                                     \
    do {                                                                                    \
        if(N) {                                                                             \
            (Current).first_line   = YYRHSLOC(Rhs, 1).first_line;                           \
            (Current).first_column = YYRHSLOC(Rhs, 1).first_column;                         \
            (Current).last_line    = YYRHSLOC(Rhs, N).last_line;                            \
            (Current).last_column  = YYRHSLOC(Rhs, N).last_column;                          \
            (Current).filename     = YYRHSLOC(Rhs, 1).filename;                             \
        } else {                                                                            \
            /* empty RHS */                                                                 \
            (Current).first_line = (Current).last_line = YYRHSLOC(Rhs, 0).last_line;        \
            (Current).first_column = (Current).last_column = YYRHSLOC (Rhs, 0).last_column; \
            (Current).filename.s = NULL ; /* new */                                         \
            (Current).filename.len = 0 ;  /* new */                                         \
        }                                                                                   \
    } while(0)

}

%token <value> INTEGER_LITERAL
%token <value> DECIMAL_LITERAL
%token <value> STRING_LITERAL
%token FALSE_LITERAL
%token TRUE_LITERAL
%token NULL_LITERAL
%token SUPER_LITERAL
%token THIS_LITERAL

%token <symbolValue> IDENTIFIER
%token <symbolValue> NAMESPACE_NAME
%token <symbolValue> IMPORT_NAME

/* keywords */
%token ABSTRACT_KEYWORD
%token AS_KEYWORD
%token CLASS_KEYWORD
%token ELSE_KEYWORD
%token EXTENDS_KEYWORD
%token FINAL_KEYWORD
%token FUNC_KEYWORD
%token IF_KEYWORD
%token IN_KEYWORD
%token IMPORT_KEYWORD
%token LAZY_KEYWORD
%token LET_KEYWORD
%token NAMESPACE_KEYWORD
%token NEW_KEYWORD
%token OVERWRITE_KEYWORD
%token PUBLIC_KEYWORD
%token PRIVATE_KEYWORD
%token PROTECTED_KEYWORD
%token VAR_KEYWORD
%token WHILE_KEYWORD

/* mathematical operators */
%token DOUBLE_PLUS_OPERATOR
%token DOUBLE_MINUS_OPERATOR

/* boolean operators */
%token DOUBLE_AND_OPERATOR
%token DOUBLE_PIPE_OPERATOR
%token NOT_OPERATOR

%left       ','
%right      <operator> ASSIGNMENT '='
%nonassoc   NO_ELSE
%nonassoc   ELSE_KEYWORD
%left       DOUBLE_PIPE_OPERATOR
%left       DOUBLE_AND_OPERATOR
%left       '|'
%left       '^'
%left       '&'
%left       <operator> EQUALITY
%left       <operator> RELATIONAL
%left       '+' '-'
%left       '*' '/' '%'
%right      DOUBLE_PLUS_OPERATOR DOUBLE_MINUS_OPERATOR UMINUS NOT_OPERATOR '~' AS_KEYWORD
%nonassoc   IN_KEYWORD
%left       '(' ')' '.'

%type <program>            program
%type <assignment>         assignment
%type <binary_expression>  binary_expression
%type <block>              block
%type <constructor_call>   constructor_call
%type <cast>               cast
%type <class>              class_body class_definition
%type <classes>            classes
%type <expression>         dispatch expression literal
%type <expressions>        expressions expression_list actuals class_actuals actual_list
%type <formal>             formal
%type <formals>            formals class_formals formal_list
%type <function>           function_signature function_definition
%type <if_else_expression> if_else
%type <initialization>     initialization
%type <initializations>    initialization_list initializations
%type <import>             import
%type <imports>            import_list imports
%type <namespace>          namespace
%type <operator>           function_visibility function_overwrite function_final
%type <let>                let
%type <property>           property_definition
%type <symbolValue>        class_head function_head property_head
%type <unary_expression>   unary_expression
%type <while_expression>   while

%destructor { waitui_ast_node_destroy((waitui_ast_node **)&$$); }    <assignment> <binary_expression> <block>
                                                                     <constructor_call> <cast> <class> <expression>
                                                                     <formal> <function> <if_else_expression>
                                                                     <initialization> <let> <property>
                                                                     <unary_expression> <while_expression>
%destructor { waitui_ast_expression_list_destroy(&$$); }       <expressions>
%destructor { waitui_ast_formal_list_destroy(&$$); }           <formals>
%destructor { waitui_ast_class_list_destroy(&$$); }            <classes>
%destructor { waitui_ast_initialization_list_destroy(&$$); }   <initializations>

%start program


%%


/* --- definitions -------------------------------------------------------------------------------------------------- */

/* program definitions */
program                         : namespace
                                    {
                                        waitui_ast_namespace_list *namespaces = waitui_ast_namespace_list_new();
                                        waitui_ast_namespace_list_push(namespaces, $1);
                                        $$ = waitui_ast_program_new(namespaces);
                                        extraParser->resultAst = waitui_ast_new($$);
                                    }
                                ;

namespace                       : NAMESPACE_KEYWORD NAMESPACE_NAME imports classes
                                    {
                                        $$ = waitui_ast_namespace_new($2, $3, $4);
                                    }
                                ;

imports                         : /* empty */
                                    {
                                        $$ = waitui_ast_import_list_new();
                                    }
                                | import_list
                                    {
                                        $$ = $1;
                                    }
                                ;

import_list                     : import
                                    {
                                        $$ = waitui_ast_import_list_new();
                                        waitui_ast_import_list_push($$, $1);
                                    }
                                | import_list import
                                    {
                                        $$ = $1;
                                        waitui_ast_import_list_push($$, $2);
                                    }
                                ;

import                          : IMPORT_KEYWORD IMPORT_NAME ';'
                                    {
                                        $$ = waitui_ast_import_new($2, NULL);
                                    }
                                | IMPORT_KEYWORD IMPORT_NAME AS_KEYWORD IDENTIFIER ';'
                                    {
                                        $$ = waitui_ast_import_new($2, $4);
                                    }
                                ;

classes                         : class_definition
                                    {
                                        $$ = waitui_ast_class_list_new();
                                        waitui_ast_class_list_push($$, $1);
                                    }
                                | classes class_definition
                                    {
                                        $$ = $1;
                                        waitui_ast_class_list_push($$, $2);
                                    }
                                ;

/* class definitions */
class_definition                : class_head class_formals '{' class_body '}' ';'
                                    {
                                        $$ = $4;

                                        waitui_ast_class_setName($$, $1);
                                        waitui_ast_class_setParameters($$, $2);
                                    }
                                | class_head class_formals EXTENDS_KEYWORD IDENTIFIER class_actuals '{' class_body '}' ';'
                                    {
                                        $$ = $7;

                                        waitui_ast_class_setName($$, $1);
                                        waitui_ast_class_setParameters($$, $2);
                                        waitui_ast_class_setSuperClass($$, $4);
                                        waitui_ast_class_setSuperClassArgs($$, $5);
                                    }
                                ;

class_head                      : CLASS_KEYWORD IDENTIFIER
                                    {
                                        $$ = $2;
                                    }
                                ;

class_formals                   : /* empty */
                                    {
                                        $$ = waitui_ast_formal_list_new();
                                    }
                                | '(' formals ')'
                                    {
                                        $$ = $2;
                                    }
                                ;

class_actuals                   : /* empty */
                                    {
                                        $$ = waitui_ast_expression_list_new();
                                    }
                                | '(' actuals ')'
                                    {
                                        $$ = $2;
                                    }
                                ;

class_body                      : /* empty */
                                    {
                                        waitui_ast_property_list *properties = waitui_ast_property_list_new();
                                        waitui_ast_function_list *functions = waitui_ast_function_list_new();
                                        $$ = waitui_ast_class_new(NULL, NULL, NULL, NULL, properties, functions);
                                    }
                                | class_body property_definition ';'
                                    {
                                        $$ = $1;
                                        waitui_ast_property_list *properties = waitui_ast_class_getProperties($$);
                                        waitui_ast_property_list_push(properties, $2);
                                    }
                                | class_body function_definition ';'
                                    {
                                        $$ = $1;
                                        waitui_ast_function_list *functions = waitui_ast_class_getFunctions($$);
                                        waitui_ast_function_list_push(functions, $2);
                                    }
                                ;

/* properties definitions */
property_definition             : property_head ':' IDENTIFIER '=' expression
                                    {
                                        $$ = waitui_ast_property_new($1, $3, $5);
                                    }
                                | property_head ':' IDENTIFIER
                                    {
                                        $$ = waitui_ast_property_new($1, $3, NULL);
                                    }
                                | property_head '=' expression
                                    {
                                        $$ = waitui_ast_property_new($1, NULL, $3);
                                    }
                                ;

property_head                   : VAR_KEYWORD IDENTIFIER
                                    {
                                        $$ = $2;
                                    }
                                ;

/* functions definitions */
function_definition             : ABSTRACT_KEYWORD function_visibility function_signature
                                    {
                                        $$ = $3;

                                        waitui_ast_function_setAbstract($$, true);
                                        waitui_ast_function_setVisibility($$, $2);
                                    }
                                | function_final function_overwrite function_visibility function_signature '=' expression
                                    {
                                        $$ = $4;

                                        waitui_ast_function_setAbstract($$, false);
                                        waitui_ast_function_setFinal($$, $1);
                                        waitui_ast_function_setOverwrite($$, $2);
                                        waitui_ast_function_setVisibility($$, $3);

                                        waitui_ast_function_setBody($$, $6);
                                    }
                                ;

function_final                  : /* empty */
                                    {
                                        $$ = false;
                                    }
                                | FINAL_KEYWORD
                                    {
                                        $$ = true;
                                    }
                                ;

function_overwrite              : /* empty */
                                    {
                                        $$ = false;
                                    }
                                | OVERWRITE_KEYWORD
                                    {
                                        $$ = true;
                                    }
                                ;


function_visibility             : /* empty */
                                    {
                                        $$ = WAITUI_AST_FUNCTION_VISIBILITY_PRIVATE;
                                    }
                                | PUBLIC_KEYWORD
                                    {
                                        $$ = WAITUI_AST_FUNCTION_VISIBILITY_PUBLIC;
                                    }
                                | PROTECTED_KEYWORD
                                    {
                                        $$ = WAITUI_AST_FUNCTION_VISIBILITY_PROTECTED;
                                    }
                                | PRIVATE_KEYWORD
                                    {
                                        $$ = WAITUI_AST_FUNCTION_VISIBILITY_PRIVATE;
                                    }
                                ;

function_signature              : function_head '(' formals ')'
                                    {
                                        $$ = waitui_ast_function_new($1, $3, NULL, NULL, WAITUI_AST_FUNCTION_VISIBILITY_PRIVATE, 0, 0, 0);
                                    }
                                | function_head '(' formals ')' ':' IDENTIFIER
                                    {
                                        $$ = waitui_ast_function_new($1, $3, $6, NULL, WAITUI_AST_FUNCTION_VISIBILITY_PRIVATE, 0, 0, 0);
                                    }
                                ;

function_head                   : FUNC_KEYWORD IDENTIFIER
                                    {
                                        $$ = $2;
                                    }
                                ;


/* --- expressions -------------------------------------------------------------------------------------------------- */

expression                      : assignment            { $$ = (waitui_ast_expression *) $1; }
                                | binary_expression     { $$ = (waitui_ast_expression *) $1; }
                                | block                 { $$ = (waitui_ast_expression *) $1; }
                                | cast                  { $$ = (waitui_ast_expression *) $1; }
                                | constructor_call      { $$ = (waitui_ast_expression *) $1; }
                                | dispatch              { $$ = (waitui_ast_expression *) $1; }
                                | if_else               { $$ = (waitui_ast_expression *) $1; }
                                | let                   { $$ = (waitui_ast_expression *) $1; }
                                | literal               { $$ = (waitui_ast_expression *) $1; }
                                | unary_expression      { $$ = (waitui_ast_expression *) $1; }
                                | while                 { $$ = (waitui_ast_expression *) $1; }
                                | '(' expression ')'    { $$ = (waitui_ast_expression *) $2; }
                                ;

assignment                      : IDENTIFIER ASSIGNMENT expression
                                    {
                                        $$ = waitui_ast_assignment_new($1, $2, $3);
                                    }
                                | IDENTIFIER '=' expression
                                    {
                                        $$ = waitui_ast_assignment_new($1, WAITUI_AST_ASSIGNMENT_OPERATOR_EQUAL, $3);
                                    }
                                ;

binary_expression               : expression '+' expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, WAITUI_AST_BINARY_OPERATOR_PLUS, $3);
                                    }
                                | expression '-' expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, WAITUI_AST_BINARY_OPERATOR_MINUS, $3);
                                    }
                                | expression '*' expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, WAITUI_AST_BINARY_OPERATOR_TIMES, $3);
                                    }
                                | expression '/' expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, WAITUI_AST_BINARY_OPERATOR_DIV, $3);
                                    }
                                | expression '%' expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, WAITUI_AST_BINARY_OPERATOR_MODULO, $3);
                                    }
                                | expression '&' expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, WAITUI_AST_BINARY_OPERATOR_AND, $3);
                                    }
                                | expression '^' expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, WAITUI_AST_BINARY_OPERATOR_CARET, $3);
                                    }
                                | expression '~' expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, WAITUI_AST_BINARY_OPERATOR_TILDE, $3);
                                    }
                                | expression '|' expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, WAITUI_AST_BINARY_OPERATOR_PIPE, $3);
                                    }
                                | expression DOUBLE_AND_OPERATOR expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, WAITUI_AST_BINARY_OPERATOR_DOUBLE_AND, $3);
                                    }
                                | expression DOUBLE_PIPE_OPERATOR expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, WAITUI_AST_BINARY_OPERATOR_DOUBLE_PIPE, $3);
                                    }
                                | expression RELATIONAL expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, $2, $3);
                                    }
                                | expression EQUALITY expression
                                    {
                                        $$ = waitui_ast_binary_expression_new($1, $2, $3);
                                    }
                                ;

block                           : '{' expressions '}'
                                    {
                                        $$ = waitui_ast_block_new($2);
                                    }
                                ;

cast                            : expression AS_KEYWORD IDENTIFIER
                                    {
                                        $$ = waitui_ast_cast_new($1, $3);
                                    }
                                ;

constructor_call                : NEW_KEYWORD IDENTIFIER '(' actuals ')'
                                    {
                                        $$ = waitui_ast_constructor_call_new($2, $4);
                                    }
                                ;

dispatch                        : expression '.' IDENTIFIER '(' actuals ')'
                                    {
                                        $$ = (waitui_ast_expression *) waitui_ast_function_call_new($1, $3, $5);
                                    }
                                | SUPER_LITERAL '.' IDENTIFIER '(' actuals ')'
                                    {
                                        $$ = (waitui_ast_expression *) waitui_ast_super_function_call_new($3, $5);
                                    }
                                ;

if_else                         : IF_KEYWORD '(' expression ')' expression %prec NO_ELSE
                                    {
                                        $$ = waitui_ast_if_else_new($3, $5, NULL);
                                    }
                                | IF_KEYWORD '(' expression ')' expression ELSE_KEYWORD expression
                                    {
                                        $$ = waitui_ast_if_else_new($3, $5, $7);
                                    }
                                ;

let                             : LET_KEYWORD initializations IN_KEYWORD expression
                                    {
                                        $$ = waitui_ast_let_new($2, $4);
                                    }
                                ;

literal                         : INTEGER_LITERAL
                                    {
                                        $$ = (waitui_ast_expression *) waitui_ast_integer_literal_new($1);
                                    }
                                | DECIMAL_LITERAL
                                    {
                                        $$ = (waitui_ast_expression *) waitui_ast_decimal_literal_new($1);
                                    }
                                | STRING_LITERAL
                                    {
                                        $$ = (waitui_ast_expression *) waitui_ast_string_literal_new($1);
                                    }
                                | NULL_LITERAL
                                    {
                                        $$ = (waitui_ast_expression *) waitui_ast_null_literal_new();
                                    }
                                | THIS_LITERAL
                                    {
                                        $$ = (waitui_ast_expression *) waitui_ast_this_literal_new();
                                    }
                                | TRUE_LITERAL
                                    {
                                        $$ = (waitui_ast_expression *) waitui_ast_boolean_literal_new(true);
                                    }
                                | FALSE_LITERAL
                                    {
                                        $$ = (waitui_ast_expression *) waitui_ast_boolean_literal_new(false);
                                    }
                                | IDENTIFIER
                                    {
                                        $$ = (waitui_ast_expression *) waitui_ast_reference_new($1);
                                    }
                                ;

unary_expression                : '-' expression %prec UMINUS
                                    {
                                        $$ = waitui_ast_unary_expression_new(WAITUI_AST_UNARY_OPERATOR_MINUS, $2);
                                    }
                                | NOT_OPERATOR expression
                                    {
                                        $$ = waitui_ast_unary_expression_new(WAITUI_AST_UNARY_OPERATOR_NOT, $2);
                                    }
                                | DOUBLE_PLUS_OPERATOR expression
                                    {
                                        $$ = waitui_ast_unary_expression_new(WAITUI_AST_UNARY_OPERATOR_DOUBLE_PLUS, $2);
                                    }
                                | DOUBLE_MINUS_OPERATOR expression
                                    {
                                        $$ = waitui_ast_unary_expression_new(WAITUI_AST_UNARY_OPERATOR_DOUBLE_MINUS, $2);
                                    }
                                ;

while                           : WHILE_KEYWORD '(' expression ')' expression
                                    {
                                        $$ = waitui_ast_while_new($3, $5);
                                    }
                                ;


/* --- general ------------------------------------------------------------------------------------------------------ */

formals                         : /* empty */
                                    {
                                        $$ = waitui_ast_formal_list_new();
                                    }
                                | formal_list
                                    {
                                        $$ = $1;
                                    }
                                ;

formal_list                     : formal
                                    {
                                        $$ = waitui_ast_formal_list_new();
                                        waitui_ast_formal_list_push($$, $1);
                                    }
                                | formal_list ',' formal
                                    {
                                        $$ = $1;
                                        waitui_ast_formal_list_push($$, $3);
                                    }
                                ;

formal                          : LAZY_KEYWORD IDENTIFIER ':' IDENTIFIER
                                    {
                                        $$ = waitui_ast_formal_new($2, $4, true);
                                    }
                                | IDENTIFIER ':' IDENTIFIER
                                    {
                                        $$ = waitui_ast_formal_new($1, $3, false);
                                    }
                                ;

expressions                     : /* empty */
                                    {
                                        $$ = waitui_ast_expression_list_new();
                                    }
                                | expression
                                    {
                                        $$ = waitui_ast_expression_list_new();
                                        waitui_ast_expression_list_push($$, $1);
                                    }
                                | expression_list
                                    {
                                        $$ = $1;
                                    }
                                ;

expression_list                 : expression ';'
                                    {
                                        $$ = waitui_ast_expression_list_new();
                                        waitui_ast_expression_list_push($$, $1);
                                    }
                                | expression_list expression ';'
                                    {
                                        $$ = $1;
                                        waitui_ast_expression_list_push($$, $2);
                                    }
                                ;

actuals                         : /* empty */
                                    {
                                        $$ = waitui_ast_expression_list_new();
                                    }
                                | actual_list
                                    {
                                        $$ = $1;
                                    }
                                ;

actual_list                     : expression
                                    {
                                        $$ = waitui_ast_expression_list_new();
                                        waitui_ast_expression_list_push($$, $1);
                                    }
                                | actual_list ',' expression
                                    {
                                        $$ = $1;
                                        waitui_ast_expression_list_push($$, $3);
                                    }
                                ;

initializations                 : initialization
                                    {
                                        $$ = waitui_ast_initialization_list_new();
                                        waitui_ast_initialization_list_push ($$, $1);
                                    }
                                | initialization_list
                                    {
                                        $$ = $1;
                                    }
                                ;

initialization_list             : initialization ','
                                    {
                                        $$ = waitui_ast_initialization_list_new();
                                        waitui_ast_initialization_list_push ($$, $1);
                                    }
                                | initialization_list initialization
                                    {
                                        $$ = $1;
                                        waitui_ast_initialization_list_push($$, $2);
                                    }
                                ;

initialization                  : IDENTIFIER ':' IDENTIFIER '=' expression
                                    {
                                        $$ = waitui_ast_initialization_new($1, $3, $5);
                                    }
                                | IDENTIFIER ':' IDENTIFIER
                                    {
                                        $$ = waitui_ast_initialization_new($1, $3, NULL);
                                    }
                                | IDENTIFIER '=' expression
                                    {
                                        $$ = waitui_ast_initialization_new($1, NULL, $3);
                                    }
                                ;


%%


void yyerror(YYLTYPE *locp, parser_extra_parser *extraParser, char const *msg) {
    fprintf(stderr, "ERROR: %.*s (%d:%d): %s in this line:\n%s\n",
            STR_FMT(&locp->filename), locp->first_line, locp->first_column + 1, msg, "");
    /*fprintf(stderr, "%*s\n", yylloc.first_column + 1, "^");*/
}
