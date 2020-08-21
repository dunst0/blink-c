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
    ast_program *program;
    ast_assignment *assignment;
    ast_binary_expression *binary_expression;
    ast_block *block;
    ast_cast *cast;
    ast_class *class;
    ast_class_list *classes;
    ast_constructor_call *constructor_call;
    ast_expression *expression;
    ast_expression_list *expressions;
    ast_formal *formal;
    ast_formal_list *formals;
    ast_function *function;
    ast_if_else *if_else_expression;
    ast_initialization *initialization;
    ast_initialization_list *initializations;
    ast_let *let;
    ast_property *property;
    ast_reference *reference;
    ast_unary_expression *unary_expression;
    ast_while *while_expression;
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

%token FUNCTION_NAME
%token <symbolValue> IDENTIFIER

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
%token NEW_KEYWORD
%token OVERWRITE_KEYWORD
%token PACKAGE_KEYWORD
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
%type <classes>            classes package
%type <expression>         dispatch expression literal
%type <expressions>        expressions expressions_list actuals class_actuals actuals_list
%type <formal>             formal
%type <formals>            formals class_formals formals_list
%type <function>           function_signature function_definition
%type <if_else_expression> if_else
%type <initialization>     initialization
%type <initializations>    initialization_list initializations
%type <operator>           function_visibility function_overwrite function_final
%type <let>                let
%type <property>           property_definition
%type <symbolValue>        identifier_definition class_head
%type <unary_expression>   unary_expression
%type <while_expression>   while

%destructor { ast_node_destroy((ast_node **)&$$); } <class> <function>
%destructor { ast_formal_list_destroy(&$$); } <formals>
%destructor { ast_class_list_destroy(&$$); } <classes>
%destructor { ast_initialization_list_destroy(&$$); } <initializations>
%destructor { symbol_destroy(&$$); } <symbolValue>

%start program

%%

/* definitions */

/* program definitions */
program                         : package imports
                                    {
                                        $$ = ast_program_new($1); // FIXME: implement package system use $2
                                        extraParser->resultAst = ast_new($$);
                                    }
                                ;

imports                         : /* empty */
                                | imports IMPORT_KEYWORD package
                                ;

package                         : PACKAGE_KEYWORD
                                    {
                                        symboltable_enter_scope(extraParser->symtable);
                                    }
                                  classes
                                    {
                                        $$ = $3;
                                        symboltable_exit_scope(extraParser->symtable);
                                    }
                                ;

classes                         : class_definition
                                    {
                                        $$ = ast_class_list_new();
                                        ast_class_list_unshift($$, $1);
                                    }
                                | classes class_definition
                                    {
                                        $$ = $1;
                                        ast_class_list_unshift($$, $2);
                                    }
                                ;

/* class definitions */
class_definition                : class_head class_formals '{' class_body '}' ';'
                                    {
                                        $$ = $4;
                                        $$->name = $1;
                                        $$->parameters = $2;
                                        symboltable_exit_scope(extraParser->symtable);
                                    }
                                | class_head class_formals EXTENDS_KEYWORD IDENTIFIER class_actuals '{' class_body '}' ';'
                                    {
                                        $$ = $7;
                                        $$->name = $1;
                                        $$->parameters = $2;
                                        $$->superClass = $4;
                                        $$->superClassArgs = $5;
                                        symboltable_exit_scope(extraParser->symtable);
                                    }
                                ;

class_head                      : CLASS_KEYWORD identifier_definition
                                    {
                                        $$ = $2;
                                        symboltable_enter_scope(extraParser->symtable);
                                    }
                                ;

class_formals                   : /* empty */
                                    {
                                        $$ = ast_formal_list_new();
                                    }
                                | '(' formals ')'
                                    {
                                        $$ = $2;
                                    }
                                ;

class_actuals                   : /* empty */
                                    {
                                        $$ = ast_expression_list_new();
                                    }
                                | '(' actuals ')'
                                    {
                                        $$ = $2;
                                    }
                                ;

class_body                      : /* empty */
                                    {
                                        ast_property_list *properties = ast_property_list_new();
                                        ast_function_list *functions = ast_function_list_new();
                                        $$ = ast_class_new(NULL, NULL, NULL, NULL, properties, functions);
                                    }
                                | class_body property_definition ';'
                                    {
                                        $$ = $1;
                                        ast_property_list_unshift($$->properties, $2);
                                    }
                                | class_body function_definition ';'
                                    {
                                        $$ = $1;
                                        ast_function_list_unshift($$->functions, $2);
                                    }
                                ;

/* properties definitions */
property_definition             : VAR_KEYWORD identifier_definition ':' IDENTIFIER '=' expression
                                    {
                                        $$ = ast_property_new($2, $4, $6);
                                    }
                                | VAR_KEYWORD identifier_definition ':' IDENTIFIER
                                    {
                                        $$ = ast_property_new($2, $4, NULL);
                                    }
                                | VAR_KEYWORD identifier_definition '=' expression
                                    {
                                        $$ = ast_property_new($2, NULL, $4);
                                    }
                                ;

/* functions definitions */
function_definition             : ABSTRACT_KEYWORD function_visibility function_signature
                                    {
                                        $$ = $3;
                                        $$->isAbstract = 1;
                                        $$->visibility = $2;
                                    }
                                | function_final function_overwrite function_visibility function_signature '=' expression
                                    {
                                        $$ = $4;
                                        $$->isFinal = $1;
                                        $$->isOverwrite = $2;
                                        $$->visibility = $3;
                                        $$->body = $6;
                                    }
                                ;

function_final                    : /* empty */
                                    {
                                        $$ = 0;
                                    }
                                | FINAL_KEYWORD
                                    {
                                        $$ = 1;
                                    }
                                ;

function_overwrite                : /* empty */
                                    {
                                        $$ = 0;
                                    }
                                | OVERWRITE_KEYWORD
                                    {
                                        $$ = 1;
                                    }
                                ;


function_visibility               : /* empty */
                                    {
                                        $$ = AST_FUNCTION_VISIBILITY_PRIVATE;
                                    }
                                | PUBLIC_KEYWORD
                                    {
                                        $$ = AST_FUNCTION_VISIBILITY_PUBLIC;
                                    }
                                | PROTECTED_KEYWORD
                                    {
                                        $$ = AST_FUNCTION_VISIBILITY_PROTECTED;
                                    }
                                | PRIVATE_KEYWORD
                                    {
                                        $$ = AST_FUNCTION_VISIBILITY_PRIVATE;
                                    }
                                ;

function_signature              : FUNC_KEYWORD FUNCTION_NAME '(' formals ')'
                                    {
                                        symbol *dummy = NULL; // FIXME: fix me FUNCTION_NAME
                                        $$ = ast_function_new(dummy, $4, NULL, NULL, AST_FUNCTION_VISIBILITY_PRIVATE, 0, 0, 0);
                                    }
                                | FUNC_KEYWORD FUNCTION_NAME '(' formals ')' ':' IDENTIFIER
                                    {
                                        symbol *dummy = NULL; // FIXME: fix me FUNCTION_NAME
                                        $$ = ast_function_new(dummy, $4, $7, NULL, AST_FUNCTION_VISIBILITY_PRIVATE, 0, 0, 0);
                                    }
                                ;

formals                         : /* empty */
                                    {
                                        $$ = ast_formal_list_new();
                                    }
                                | formal
                                    {
                                        $$ = ast_formal_list_new();
                                        ast_formal_list_unshift($$, $1);
                                    }
                                | formals_list
                                    {
                                        $$ = $1;
                                    }
                                ;

formals_list                    : formal ','
                                    {
                                        $$ = ast_formal_list_new();
                                        ast_formal_list_unshift($$, $1);
                                    }
                                | formals_list formal
                                    {
                                        $$ = $1;
                                        ast_formal_list_unshift($$, $2);
                                    }
                                ;

formal                          : LAZY_KEYWORD identifier_definition ':' IDENTIFIER
                                    {
                                        $$ = ast_formal_new($2, $4, 1);
                                    }
                                | identifier_definition ':' IDENTIFIER
                                    {
                                        $$ = ast_formal_new($1, $3, 0);
                                    }
                                ;


/* expressions */

expression                      : assignment            { $$ = (ast_expression *) $1; }
                                | binary_expression     { $$ = (ast_expression *) $1; }
                                | block                 { $$ = (ast_expression *) $1; }
                                | cast                  { $$ = (ast_expression *) $1; }
                                | constructor_call      { $$ = (ast_expression *) $1; }
                                | dispatch              { $$ = (ast_expression *) $1; }
                                | if_else               { $$ = (ast_expression *) $1; }
                                | let                   { $$ = (ast_expression *) $1; }
                                | literal               { $$ = (ast_expression *) $1; }
                                | unary_expression      { $$ = (ast_expression *) $1; }
                                | while                 { $$ = (ast_expression *) $1; }
                                | '(' expression ')'    { $$ = $2; }
                                ;

assignment                      : IDENTIFIER ASSIGNMENT expression
                                    {
                                        $$ = ast_assignment_new($1, $2, $3);
                                    }
                                | IDENTIFIER '=' expression
                                    {
                                        $$ = ast_assignment_new($1, AST_ASSIGNMENT_OPERATOR_EQUAL, $3);
                                    }
                                ;

binary_expression               : expression '+' expression
                                    {
                                        $$ = ast_binary_expression_new($1, AST_BINARY_OPERATOR_PLUS, $3);
                                    }
                                | expression '-' expression
                                    {
                                        $$ = ast_binary_expression_new($1, AST_BINARY_OPERATOR_MINUS, $3);
                                    }
                                | expression '*' expression
                                    {
                                        $$ = ast_binary_expression_new($1, AST_BINARY_OPERATOR_TIMES, $3);
                                    }
                                | expression '/' expression
                                    {
                                        $$ = ast_binary_expression_new($1, AST_BINARY_OPERATOR_DIV, $3);
                                    }
                                | expression '%' expression
                                    {
                                        $$ = ast_binary_expression_new($1, AST_BINARY_OPERATOR_MODULO, $3);
                                    }
                                | expression '&' expression
                                    {
                                        $$ = ast_binary_expression_new($1, AST_BINARY_OPERATOR_AND, $3);
                                    }
                                | expression '^' expression
                                    {
                                        $$ = ast_binary_expression_new($1, AST_BINARY_OPERATOR_CARET, $3);
                                    }
                                | expression '~' expression
                                    {
                                        $$ = ast_binary_expression_new($1, AST_BINARY_OPERATOR_TILDE, $3);
                                    }
                                | expression '|' expression
                                    {
                                        $$ = ast_binary_expression_new($1, AST_BINARY_OPERATOR_PIPE, $3);
                                    }
                                | expression DOUBLE_AND_OPERATOR expression
                                    {
                                        $$ = ast_binary_expression_new($1, AST_BINARY_OPERATOR_DOUBLE_AND, $3);
                                    }
                                | expression DOUBLE_PIPE_OPERATOR expression
                                    {
                                        $$ = ast_binary_expression_new($1, AST_BINARY_OPERATOR_DOUBLE_PIPE, $3);
                                    }
                                | expression RELATIONAL expression
                                    {
                                        $$ = ast_binary_expression_new($1, $2, $3);
                                    }
                                | expression EQUALITY expression
                                    {
                                        $$ = ast_binary_expression_new($1, $2, $3);
                                    }
                                ;

block                           : '{' expressions '}'
                                    {
                                        $$ = ast_block_new($2);
                                    }
                                ;

cast                            : expression AS_KEYWORD IDENTIFIER
                                    {
                                        $$ = ast_cast_new($1, $3);
                                    }
                                ;

constructor_call                : NEW_KEYWORD IDENTIFIER '(' actuals ')'
                                    {
                                        $$ = ast_constructor_call_new($2, $4);
                                    }
                                ;

dispatch                        : expression '.' FUNCTION_NAME '(' actuals ')'
                                    {
                                        symbol *dummy = NULL; // FIXME: fix me FUNCTION_NAME
                                        $$ = (ast_expression *) ast_function_call_new($1, dummy, $5);
                                    }
                                | SUPER_LITERAL '.' FUNCTION_NAME '(' actuals ')'
                                    {
                                        symbol *dummy = NULL; // FIXME: fix me FUNCTION_NAME
                                        $$ = (ast_expression *) ast_super_function_call_new(dummy, $5);
                                    }
                                ;

if_else                         : IF_KEYWORD '(' expression ')' expression %prec NO_ELSE
                                    {
                                        $$ = ast_if_else_new($3, $5, NULL);
                                    }
                                | IF_KEYWORD '(' expression ')' expression ELSE_KEYWORD expression
                                    {
                                        $$ = ast_if_else_new($3, $5, $7);
                                    }
                                ;

let                             : LET_KEYWORD initializations IN_KEYWORD expression
                                    {
                                        $$ = ast_let_new($2, $4);
                                    }
                                ;

literal                         : INTEGER_LITERAL
                                    {
                                        $$ = (ast_expression *) ast_integer_literal_new($1);
                                    }
                                | DECIMAL_LITERAL
                                    {
                                        $$ = (ast_expression *) ast_decimal_literal_new($1);
                                    }
                                | STRING_LITERAL
                                    {
                                        $$ = (ast_expression *) ast_string_literal_new($1);
                                    }
                                | NULL_LITERAL
                                    {
                                        $$ = (ast_expression *) ast_null_literal_new();
                                    }
                                | THIS_LITERAL
                                    {
                                        $$ = (ast_expression *) ast_this_literal_new();
                                    }
                                | TRUE_LITERAL
                                    {
                                        $$ = (ast_expression *) ast_boolean_literal_new(1);
                                    }
                                | FALSE_LITERAL
                                    {
                                        $$ = (ast_expression *) ast_boolean_literal_new(0);
                                    }
                                | IDENTIFIER
                                    {
                                        $$ = (ast_expression *) ast_reference_new($1);
                                    }
                                ;

unary_expression                : '-' expression %prec UMINUS
                                    {
                                        $$ = ast_unary_expression_new(AST_UNARY_OPERATOR_MINUS, $2);
                                    }
                                | NOT_OPERATOR expression
                                    {
                                        $$ = ast_unary_expression_new(AST_UNARY_OPERATOR_NOT, $2);
                                    }
                                | DOUBLE_PLUS_OPERATOR expression
                                    {
                                        $$ = ast_unary_expression_new(AST_UNARY_OPERATOR_DOUBLE_PLUS, $2);
                                    }
                                | DOUBLE_MINUS_OPERATOR expression
                                    {
                                        $$ = ast_unary_expression_new(AST_UNARY_OPERATOR_DOUBLE_MINUS, $2);
                                    }
                                ;

while                           : WHILE_KEYWORD '(' expression ')' expression
                                    {
                                        $$ = ast_while_new($3, $5);
                                    }
                                ;

expressions                     : /* empty */
                                    {
                                        $$ = ast_expression_list_new();
                                    }
                                | expression
                                    {
                                        ast_expression *expression = $1;
                                        $$ = ast_expression_list_new();
                                        ast_expression_list_unshift($$, expression);
                                    }
                                | expressions_list
                                    {
                                        $$ = $1;
                                    }
                                ;

expressions_list                : expression ';'
                                    {
                                        $$ = ast_expression_list_new();
                                        ast_expression_list_unshift($$, $1);
                                    }
                                | expressions_list expression ';'
                                    {
                                        $$ = $1;
                                        ast_expression_list_unshift($$, $2);
                                    }
                                ;


/* general */

actuals                         : /* empty */
                                    {
                                        $$ = ast_expression_list_new();
                                    }
                                | expression
                                    {
                                        $$ = ast_expression_list_new();
                                        ast_expression_list_unshift($$, $1);
                                    }
                                | actuals_list
                                    {
                                        $$ = $1;
                                    }
                                ;

actuals_list                    : expression ','
                                    {
                                        $$ = ast_expression_list_new();
                                        ast_expression_list_unshift($$, $1);
                                    }
                                | actuals_list expression ','
                                    {
                                        //FIXME: found way of removing trailing ,
                                        $$ = $1;
                                        ast_expression_list_unshift($$, $2);
                                    }
                                ;

initializations                 : initialization
                                    {
                                        $$ = ast_initialization_list_new();
                                        ast_initialization_list_unshift ($$, $1);
                                    }
                                | initialization_list
                                    {
                                        $$ = $1;
                                    }
                                ;

initialization_list             : initialization ','
                                    {
                                        $$ = ast_initialization_list_new();
                                        ast_initialization_list_unshift ($$, $1);
                                    }
                                | initialization_list initialization
                                    {
                                        $$ = $1;
                                        ast_initialization_list_unshift($$, $2);
                                    }
                                ;

initialization                  : identifier_definition ':' IDENTIFIER '=' expression
                                    {
                                        $$ = ast_initialization_new($1, $3, $5);
                                    }
                                | identifier_definition ':' IDENTIFIER
                                    {
                                        $$ = ast_initialization_new($1, $3, NULL);
                                    }
                                | identifier_definition '=' expression
                                    {
                                        str emtpy = STR_NULL_INIT;
                                        $$ = ast_initialization_new($1, NULL, $3);
                                    }
                                ;

identifier_definition           :   {
                                        symboltable_enter_declaration_mode(extraParser->symtable);
                                    }
                                  IDENTIFIER
                                    {
                                        symboltable_leave_declaration_mode(extraParser->symtable);
                                        symboltable_mark_symbol_stolen(extraParser->symtable, $2);
                                        $$ = $2;
                                    }
                                ;


%%

void yyerror(YYLTYPE *locp, parser_extra_parser *extraParser, char const *msg) {
    fprintf(stderr, "Error: %.*s (%d:%d): %s in this line:\n%s\n",
            STR_FMT(&locp->filename), locp->first_line, locp->first_column + 1, msg, "");
    /*fprintf(stderr, "%*s\n", yylloc.first_column + 1, "^");*/
}