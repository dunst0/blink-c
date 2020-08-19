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
%type <expression>         dispatch expression value literal
%type <expressions>        expressions expressions_list actuals_definition actuals class_actuals
%type <formal>             formal
%type <formals>            formals_definition formals class_formals
%type <function>           function_signature function_definition
%type <if_else_expression> if_else
%type <initialization>     initialization
%type <initializations>    initialization_list;
%type <operator>           function_visibility function_overwrite function_final
%type <let>                let
%type <property>           property_definition
%type <symbolValue>        type identifier_definition
%type <unary_expression>   unary_expression
%type <while_expression>   while

%destructor { ast_node_destroy((ast_node **)&$$); } <class> <function>
%destructor { ast_formal_list_destroy(&$$); } <formals>
%destructor { ast_class_list_destroy(&$$); } <classes>
%destructor { ast_initialization_list_destroy(&$$); } <initializations>

%start program

%%

/* definitions */

/* program definitions */
program                         : imports package
                                    {
                                        $$ = ast_program_new($2); // FIXME: implement package system use $1
                                        extraParser->resultAst = ast_new($$);
                                    }
                                ;

imports                         : /* empty */
                                | imports IMPORT_KEYWORD package
                                ;

package                         : PACKAGE_KEYWORD classes
                                    {
                                        $$ = $2;
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
class_definition                : CLASS_KEYWORD IDENTIFIER class_formals '{' class_body '}' ';'
                                    {
                                        // TODO: mark identifier as definition
                                        $$ = $5;
                                        $$->name = $2;
                                        $$->parameters = $3;
                                    }
                                | CLASS_KEYWORD IDENTIFIER class_formals EXTENDS_KEYWORD IDENTIFIER class_actuals '{' class_body '}' ';'
                                    {
                                        // TODO: mark identifier as definition
                                        $$ = $8;
                                        $$->name = $2;
                                        $$->parameters = $3;
                                        $$->superClass = $5;
                                        $$->superClassArgs = $6;
                                    }
                                ;

class_formals                   : /* empty */
                                    {
                                        $$ = ast_formal_list_new();
                                    }
                                | formals_definition
                                    {
                                        $$ = $1;
                                    }
                                ;

class_actuals                   : /* empty */
                                    {
                                        $$ = ast_expression_list_new();
                                    }
                                | actuals_definition
                                    {
                                        $$ = $1;
                                    }
                                ;

class_body                      : /* empty */
                                    {
                                        ast_property_list *properties = ast_property_list_new();
                                        ast_function_list *functions = ast_function_list_new();
                                        $$ = ast_class_new(NULL, NULL, NULL, NULL, properties, functions);
                                    }
                                | class_body property_definition
                                    {
                                        $$ = $1;
                                        ast_property_list_unshift($$->properties, $2);
                                    }
                                | class_body function_definition
                                    {
                                        $$ = $1;
                                        ast_function_list_unshift($$->functions, $2);
                                    }
                                ;

/* properties definitions */
property_definition             : VAR_KEYWORD IDENTIFIER type value ';'
                                    {
                                        // TODO: mark identifier as definition
                                        $$ = ast_property_new($2, $3, $4);
                                    }
                                | VAR_KEYWORD IDENTIFIER type ';'
                                    {
                                        // TODO: mark identifier as definition
                                        $$ = ast_property_new($2, $3, NULL);
                                    }
                                | VAR_KEYWORD IDENTIFIER value ';'
                                    {
                                        // TODO: mark identifier as definition
                                        $$ = ast_property_new($2, NULL, $3);
                                    }
                                ;

/* functions definitions */
function_definition             : ABSTRACT_KEYWORD function_visibility function_signature ';'
                                    {
                                        $$ = $3;
                                        $$->isAbstract = 1;
                                        $$->visibility = $2;
                                    }
                                | function_final function_overwrite function_visibility function_signature value ';'
                                    {
                                        $$ = $4;
                                        $$->isFinal = $1;
                                        $$->isOverwrite = $2;
                                        $$->visibility = $3;
                                        $$->body = $5;
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

function_signature              : FUNC_KEYWORD FUNCTION_NAME formals_definition
                                    {
                                        symbol *dummy = NULL; // FIXME: fix me FUNCTION_NAME
                                        $$ = ast_function_new(dummy, $3, NULL, NULL, AST_FUNCTION_VISIBILITY_PRIVATE, 0, 0, 0);
                                    }
                                | FUNC_KEYWORD FUNCTION_NAME formals_definition type
                                    {
                                        symbol *dummy = NULL; // FIXME: fix me FUNCTION_NAME
                                        $$ = ast_function_new(dummy, $3, $4, NULL, AST_FUNCTION_VISIBILITY_PRIVATE, 0, 0, 0);
                                    }
                                ;

formals_definition              : '(' formals ')'
                                    {
                                        $$ = $2;
                                    }
                                ;

formals                         : /* empty */
                                    {
                                        $$ = ast_formal_list_new();
                                    }
                                | formals formal ','
                                    {
                                        $$ = $1;
                                        ast_formal_list_unshift($$, $2);
                                    }
                                ;

formal                          : LAZY_KEYWORD IDENTIFIER type
                                    {
                                        // TODO: mark identifier as definition
                                        $$ = ast_formal_new($2, $3, 1);
                                    }
                                | IDENTIFIER type
                                    {
                                        // TODO: mark identifier as definition
                                        $$ = ast_formal_new($1, $2, 0);
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

constructor_call                : NEW_KEYWORD IDENTIFIER actuals_definition
                                    {
                                        $$ = ast_constructor_call_new($2, $3);
                                    }
                                ;

dispatch                        : expression '.' FUNCTION_NAME actuals_definition
                                    {
                                        symbol *dummy = NULL; // FIXME: fix me FUNCTION_NAME
                                        $$ = (ast_expression *) ast_function_call_new($1, dummy, $4);
                                    }
                                | SUPER_LITERAL '.' FUNCTION_NAME actuals_definition
                                    {
                                        symbol *dummy = NULL; // FIXME: fix me FUNCTION_NAME
                                        $$ = (ast_expression *) ast_super_function_call_new(dummy, $4);
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

let                             : LET_KEYWORD initialization_list IN_KEYWORD expression
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
                                        ast_expression *expression = $1;
                                        $$ = ast_expression_list_new();
                                        ast_expression_list_unshift($$, expression);
                                    }
                                | expressions_list expression ';'
                                    {
                                        ast_expression *expression = $2;
                                        $$ = $1;
                                        ast_expression_list_unshift($$, expression);
                                    }
                                ;


/* general */

actuals_definition              : '(' actuals ')'
                                    {
                                        $$ = $2;
                                    }
                                ;

actuals                         : /* empty */
                                    {
                                        $$ = ast_expression_list_new();
                                    }
                                | actuals expression ','
                                    {
                                        ast_expression *expression = $2;
                                        $$ = $1;
                                        ast_expression_list_unshift($$, expression);
                                    }
                                ;

initialization_list             : initialization
                                    {
                                        ast_initialization *initialization = $1;
                                        $$ = ast_initialization_list_new();
                                        ast_initialization_list_unshift ($$, initialization);
                                    }
                                | initialization_list initialization ','
                                    {
                                        ast_initialization *initialization = $2;
                                        $$ = $1;
                                        ast_initialization_list_unshift($$, initialization);
                                    }
                                ;

initialization                  : identifier_definition type value
                                    {
                                        $$ = ast_initialization_new($1, $2, $3);
                                    }
                                | identifier_definition type
                                    {
                                        $$ = ast_initialization_new($1, $2, NULL);
                                    }
                                | identifier_definition value
                                    {
                                        str emtpy = STR_NULL_INIT;
                                        $$ = ast_initialization_new($1, NULL, $2);
                                    }
                                ;

identifier_definition           :   {
                                        symboltable_enter_declaration_mode(extraParser->symtable);
                                    }
                                  IDENTIFIER
                                    {
                                        symboltable_leave_declaration_mode(extraParser->symtable);
                                        $$ = $2;
                                    }
                                ;

type                            : ':' IDENTIFIER
                                    {
                                        $$ = $2;
                                    }
                                ;

value                           : '=' expression
                                    {
                                        $$ = $2;
                                    }
                                ;


%%

void yyerror(YYLTYPE *locp, parser_extra_parser *extraParser, char const *msg) {
    fprintf(stderr, "Error: %.*s (%d:%d): %s in this line:\n%s\n",
            STR_FMT(&locp->filename), locp->first_line, locp->first_column + 1, msg, "");
    /*fprintf(stderr, "%*s\n", yylloc.first_column + 1, "^");*/
}