/**
 * @file blink.l
 * @author rick
 * @date 26.03.20
 * @brief Lexer for the blink language
 */

%option noinput nounput noyywrap nodefault
%option reentrant bison-bridge bison-locations
%option debug
%option extra-type="parser_extra_lexer *"

%{
#include "blink/parser_impl.h"

#include "blink/parser_helper.h"

#include <stdio.h>

#define YY_USER_ACTION                                                         \
    yylloc->filename     = yylloc->filename;                                   \
    yylloc->first_line   = yylloc->last_line;                                  \
    yylloc->first_column = yylloc->last_column;                                \
    for(int i = 0; yytext[i] != '\0'; ++i) {                                   \
        if(yytext[i] == '\n') {                                                \
            yylloc->last_line++;                                               \
            yylloc->last_column = 0;                                           \
        } else {                                                               \
            yylloc->last_column++;                                             \
        }                                                                      \
    }

#define RETURN(token)                                                          \
    yyextra->lastToken = token;                                                \
    return (token);

#define RETURN_SEMICOLON_IF_NEEDED                                             \
    switch (yyextra->lastToken) {                                              \
        case INTEGER_LITERAL:                                                  \
        case DECIMAL_LITERAL:                                                  \
        case STRING_LITERAL:                                                   \
        case NULL_LITERAL:                                                     \
        case THIS_LITERAL:                                                     \
        case TRUE_LITERAL:                                                     \
        case FALSE_LITERAL:                                                    \
        case IDENTIFIER:                                                       \
        case ')':                                                              \
        case '}':                                                              \
            RETURN(';');                                                       \
    }

%}

%x IMPORT
%x PACKAGE
%x METHOD

letter              [a-zA-Z]
digit               [0-9]
positive_number     [1-9]{digit}*
exponent_part       [eE][-+]?{positive_number}
operator            ([-+*\/=><!&%~$^]=?)|\+\+|--
whitespace          [ \a\f\t\r\v]
not_whitespace      [^ \a\f\t\r\v\n]

%%

<INITIAL,METHOD,IMPORT,PACKAGE>{whitespace}*                                            /* eat the whitespace */

<INITIAL>"import"                                                                       BEGIN(IMPORT);
<INITIAL>"package"                                                                      BEGIN(PACKAGE);

<IMPORT>{not_whitespace}+                                                               {
                                                                                            printf("<IMPORT_KEYWORD: %s>\n", yytext);

                                                                                            BEGIN(INITIAL);
                                                                                            RETURN(IMPORT_KEYWORD);
                                                                                        }
<PACKAGE>{not_whitespace}+                                                              {
                                                                                            printf("<PACKAGE_KEYWORD: %s>\n", yytext);
                                                                                            BEGIN(INITIAL);
                                                                                            RETURN(PACKAGE_KEYWORD);
                                                                                        }

<<EOF>>                                                                                 {
                                                                                            /*if (!lexer_pop_yy_state()) {*/
                                                                                                yyterminate();
                                                                                            /*}*/
                                                                                        }


"abstract"                                                                              RETURN(ABSTRACT_KEYWORD);
"as"                                                                                    RETURN(AS_KEYWORD);
"class"                                                                                 RETURN(CLASS_KEYWORD);
"else"                                                                                  RETURN(ELSE_KEYWORD);
"extends"                                                                               RETURN(EXTENDS_KEYWORD);
"false"                                                                                 RETURN(FALSE_LITERAL);
"final"                                                                                 RETURN(FINAL_KEYWORD);
"func"                                                                                  {
                                                                                            BEGIN(METHOD);
                                                                                            RETURN(FUNC_KEYWORD);
                                                                                        }
"if"                                                                                    RETURN(IF_KEYWORD);
"in"                                                                                    RETURN(IN_KEYWORD);
"lazy"                                                                                  RETURN(LAZY_KEYWORD);
"let"                                                                                   RETURN(LET_KEYWORD);
"new"                                                                                   RETURN(NEW_KEYWORD);
"null"                                                                                  RETURN(NULL_LITERAL);
"overwrite"                                                                             RETURN(OVERWRITE_KEYWORD);
"public"                                                                                RETURN(PUBLIC_KEYWORD);
"private"                                                                               RETURN(PRIVATE_KEYWORD);
"protected"                                                                             RETURN(PROTECTED_KEYWORD);
"super"                                                                                 RETURN(SUPER_LITERAL);
"this"                                                                                  RETURN(THIS_LITERAL);
"true"                                                                                  RETURN(TRUE_LITERAL);
"var"                                                                                   RETURN(VAR_KEYWORD);
"while"                                                                                 RETURN(WHILE_KEYWORD);

"++"                                                                                    RETURN(DOUBLE_PLUS_OPERATOR);
"--"                                                                                    RETURN(DOUBLE_MINUS_OPERATOR);

"+="                                                                                    { yylval->operator = AST_ASSIGNMENT_OPERATOR_PLUS_EQUAL; RETURN(ASSIGNMENT); }
"-="                                                                                    { yylval->operator = AST_ASSIGNMENT_OPERATOR_MINUS_EQUAL; RETURN(ASSIGNMENT); }
"*="                                                                                    { yylval->operator = AST_ASSIGNMENT_OPERATOR_TIMES_EQUAL; RETURN(ASSIGNMENT); }
"/="                                                                                    { yylval->operator = AST_ASSIGNMENT_OPERATOR_DIV_EQUAL; RETURN(ASSIGNMENT); }
"%="                                                                                    { yylval->operator = AST_ASSIGNMENT_OPERATOR_MODULO_EQUAL; RETURN(ASSIGNMENT); }
"&="                                                                                    { yylval->operator = AST_ASSIGNMENT_OPERATOR_AND_EQUAL; RETURN(ASSIGNMENT); }
"^="                                                                                    { yylval->operator = AST_ASSIGNMENT_OPERATOR_CARET_EQUAL; RETURN(ASSIGNMENT); }
"~="                                                                                    { yylval->operator = AST_ASSIGNMENT_OPERATOR_TILDE_EQUAL; RETURN(ASSIGNMENT); }
"|="                                                                                    { yylval->operator = AST_ASSIGNMENT_OPERATOR_PIPE_EQUAL; RETURN(ASSIGNMENT); }

"&&"                                                                                    RETURN(DOUBLE_AND_OPERATOR);
"||"                                                                                    RETURN(DOUBLE_PIPE_OPERATOR);
"!"                                                                                     RETURN(NOT_OPERATOR);

"=="                                                                                    { yylval->operator = AST_BINARY_OPERATOR_EQUAL; RETURN(EQUALITY); }
"!="                                                                                    { yylval->operator = AST_BINARY_OPERATOR_NOT_EQUAL; RETURN(EQUALITY); }

"<"                                                                                     { yylval->operator = AST_BINARY_OPERATOR_LESS; RETURN(RELATIONAL); }
"<="                                                                                    { yylval->operator = AST_BINARY_OPERATOR_LESS_EQUAL; RETURN(RELATIONAL); }
">="                                                                                    { yylval->operator = AST_BINARY_OPERATOR_GREATER; RETURN(RELATIONAL); }
">"                                                                                     { yylval->operator = AST_BINARY_OPERATOR_GREATER_EQUAL; RETURN(RELATIONAL); }

[-.=+*%/&^~|:,{\[(}\]);]                                                                RETURN(yytext[0]);

0|{positive_number}                                                                     { yylval->value.s = yytext; yylval->value.len = yyleng; RETURN(INTEGER_LITERAL); }
((0|{positive_number})?\.{digit}+{exponent_part}?)|({positive_number}{exponent_part})   { yylval->value.s = yytext; yylval->value.len = yyleng; RETURN(DECIMAL_LITERAL); }
\"(\\.|[^"\\])*\"                                                                       { yylval->value.s = yytext + 1; yylval->value.len = yyleng - 2; RETURN(STRING_LITERAL); }

<METHOD>(operator{operator})|(({letter}|_)({letter}|{digit}|_)*)                        {
                                                                                            printf("<METHOD_NAME: %s>\n", yytext);
                                                                                            BEGIN(INITIAL);
                                                                                            RETURN(METHOD_NAME);
                                                                                        }
({letter}|_)({letter}|{digit}|_)*                                                       {
                                                                                            printf("<IDENTIFIER: %s>\n", yytext);
                                                                                            RETURN(IDENTIFIER);
                                                                                        }

\n+                                                                                     RETURN_SEMICOLON_IF_NEEDED;

"//".*                                                                                  /* this is a comment skip */

<*>.                                                                                    { yyerror(yylloc, yyextra->extraParser, "unexpected input"); exit(1); }

%%
