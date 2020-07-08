/**
 * Parser for the blink language
 */

%code requires {

#include "blink/str.h"

#include <stdio.h>

enum relational_operator {
    LESS_OPERATOR,
    LESS_EQUAL_OPERATOR,
    GREATER_OPERATOR,
    GREATER_EQUAL_OPERATOR,
};

enum equality_operator {
    EQUAL_OPERATOR,
    NOT_EQUAL_OPERATOR,
};

enum assignment_operator {
    PLUS_EQUAL_OPERATOR,
    MINUS_EQUAL_OPERATOR,
    TIMES_EQUAL_OPERATOR,
    DIV_EQUAL_OPERATOR,
    MODULO_EQUAL_OPERATOR,
    AND_EQUAL_OPERATOR,
    CARET_EQUAL_OPERATOR,
    TILDE_EQUAL_OPERATOR,
    PIPE_EQUAL_OPERATOR,
};

extern int yyerror(char *s);
extern int yylex(void);

typedef struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
    str filename;
} YYLTYPE;

/* alert the parser that we have our own definition */
#define YYLTYPE_IS_DECLARED 1

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

%code {
#include <stdlib.h>
}

%verbose
%define parse.trace
%locations

%union{
    int operator;
}

%token INTEGER_LITERAL
%token DECIMAL_LITERAL
%token STRING_LITERAL
%token FALSE_LITERAL
%token NULL_LITERAL
%token SUPER_LITERAL
%token THIS_LITERAL
%token TRUE_LITERAL

%token METHOD_NAME
%token IDENTIFIER

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
%left       '(' ')' '.'


%start program

%%

/* definitions */

/* program definitions */
program                         : imports package
                                ;

imports                         : /* empty */
                                | imports IMPORT_KEYWORD package
                                ;

package                         : PACKAGE_KEYWORD classes
                                ;

classes                         : class_definition
                                | classes class_definition
                                ;

/* class definitions */
class_definition                : CLASS_KEYWORD IDENTIFIER class_formals '{' class_body '}'
                                | CLASS_KEYWORD IDENTIFIER class_formals EXTENDS_KEYWORD class_actuals '{' class_body '}'
                                ;

class_formals                   : /* empty */
                                | formals_definition
                                ;

class_actuals                   : /* empty */
                                | actuals_definition
                                ;

class_body                      : /* empty */
                                | class_body property_definition
                                | class_body method_definition
                                ;


/* properties definitions */
property_definition             : VAR_KEYWORD IDENTIFIER property_signature
                                ;

property_signature              : type property_value
                                | property_value
                                | type
                                ;

property_value                  : '=' expression
                                ;

/* methods definitions */
method_definition               : ABSTRACT_KEYWORD method_visibility method_signature
                                | method_final method_overwrite method_visibility method_signature method_value
                                ;

method_final                    : /* empty */
                                | FINAL_KEYWORD
                                ;

method_overwrite                : /* empty */
                                | OVERWRITE_KEYWORD
                                ;


method_visibility               : /* empty */
                                | PROTECTED_KEYWORD
                                | PRIVATE_KEYWORD
                                ;

method_signature                : FUNC_KEYWORD METHOD_NAME formals_definition
                                | FUNC_KEYWORD METHOD_NAME formals_definition type
                                ;

method_value                    : '=' block_expression
                                ;

formals_definition              : '(' formals ')'
                                ;

formals                         : /* empty */
                                | formals ',' formal
                                ;

formal                          : LAZY_KEYWORD IDENTIFIER type
                                | IDENTIFIER type
                                ;


/* expressions */

block_expression                : expression ';'
                                |'{' expressions '}'
                                ;

expressions                     : /* empty */
                                | expressions expression ';'
                                ;

expression                      : INTEGER_LITERAL
                                | DECIMAL_LITERAL
                                | STRING_LITERAL
                                | NULL_LITERAL
                                | THIS_LITERAL
                                | TRUE_LITERAL
                                | FALSE_LITERAL
                                | IDENTIFIER
                                | assignment_expression
                                | binary_expression
                                | cast_expression
                                | comparison_expression
                                | constructor_call_expression
                                | dispatch_expression
                                | if_else_expression
                                | let_expression
                                | unary_expression
                                | '(' expression ')'
                                | while_expression
                                ;

assignment_expression           : IDENTIFIER ASSIGNMENT expression
                                | IDENTIFIER '=' expression
                                ;

binary_expression               : expression '+' expression
                                | expression '-' expression
                                | expression '*' expression
                                | expression '/' expression
                                | expression '%' expression
                                | expression '&' expression
                                | expression '~' expression
                                | expression '^' expression
                                | expression '|' expression
                                | expression DOUBLE_AND_OPERATOR expression
                                | expression DOUBLE_PIPE_OPERATOR expression
                                ;

cast_expression                 : expression AS_KEYWORD IDENTIFIER
                                ;

comparison_expression           : expression RELATIONAL expression
                                | expression EQUALITY expression
                                ;

constructor_call_expression     : NEW_KEYWORD IDENTIFIER actuals_definition
                                ;

dispatch_expression             : expression '.' method_call_expression
                                | SUPER_LITERAL '.' method_call_expression
                                ;

if_else_expression              : IF_KEYWORD '(' expression ')' block_expression %prec NO_ELSE
                                | IF_KEYWORD '(' expression ')' block_expression ELSE_KEYWORD block_expression
                                ;

let_expression                  : LET_KEYWORD initializations IN_KEYWORD block_expression
                                ;

method_call_expression          : METHOD_NAME actuals_definition
                                ;

unary_expression                : '-' expression %prec UMINUS
                                | NOT_OPERATOR expression
                                | DOUBLE_PLUS_OPERATOR expression
                                | DOUBLE_MINUS_OPERATOR expression
                                ;

while_expression                : WHILE_KEYWORD '(' expression ')' block_expression
                                ;


/* general */

actuals_definition              : '(' actuals ')'
                                ;

actuals                         : /* empty */
                                | actuals ',' expression
                                ;

initializations                 : initialization_expression
                                | initializations ',' initialization_expression
                                ;

initialization_expression       : IDENTIFIER initialization_definition
                                ;

initialization_definition       : type '=' expression
                                | type
                                | '=' expression
                                ;

type                            : ':' IDENTIFIER
                                ;

%%

int yyerror(char *s)
{
    printf("%.*s (%d:%d): %s in this line:\n%s\n", STR_FMT(&yylloc.filename), yylloc.first_line + 1, yylloc.first_column + 1, s, "");
    printf("%*s\n", yylloc.first_column + 1, "^");

    return 1;
}
