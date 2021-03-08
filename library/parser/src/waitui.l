/**
 * @file waitui.l
 * @author rick
 * @date 26.03.20
 * @brief Lexer for the waitui language
 */

%option noinput nounput noyywrap nodefault
%option reentrant bison-bridge bison-locations
%option debug
%option extra-type="parser_extra_lexer *"

%{
#include "waitui/parser_impl.h"

#include "waitui/parser_helper.h"

#include <stdio.h>

static int parser_push_yy_state(parser_extra_lexer *lexerExtra, char *import, int importLength);
static int parser_pop_yy_state(parser_extra_lexer *lexerExtra);

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
        case IMPORT_KEYWORD:                                                   \
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
%x NAMESPACE

letter              [a-zA-Z]
digit               [0-9]
positive_number     [1-9]{digit}*
exponent_part       [eE][-+]?{positive_number}
operator            ([-+*\/=><!&%~$^]=?)|\+\+|--
whitespace          [ \a\f\t\r\v]
not_whitespace      [^ \a\f\t\r\v\n]
namespace_name      (({letter}|_)({letter}|{digit}|_|\.)*({letter}|{digit}|_))
identifier          (operator{operator})|(({letter}|_)({letter}|{digit}|_)*)

%%

<INITIAL,IMPORT,NAMESPACE>{whitespace}*                                                 /* eat the whitespace */

<INITIAL>"namespace"                                                                    {
                                                                                            BEGIN(NAMESPACE);
                                                                                            RETURN(NAMESPACE_KEYWORD);
                                                                                        }
<INITIAL>"import"                                                                       BEGIN(IMPORT);

<IMPORT>{not_whitespace}+                                                               {
                                                                                            if (!parser_push_yy_state(yyextra, yytext, yyleng)) {
                                                                                                yyerror(yylloc, yyextra->extraParser, "could not push lexer state to stack");
                                                                                                RETURN(YYerror);
                                                                                            }

                                                                                            BEGIN(INITIAL);
                                                                                            RETURN(IMPORT_KEYWORD);
                                                                                        }
<NAMESPACE>{namespace_name}                                                             {
                                                                                            BEGIN(INITIAL);
                                                                                            RETURN(NAMESPACE_NAME);
                                                                                        }

<<EOF>>                                                                                 {
                                                                                            if (!parser_pop_yy_state(yyextra)) {
                                                                                                yyerror(yylloc, yyextra->extraParser, "could not pop lexer state from stack");
                                                                                                RETURN(YYerror);
                                                                                            }

                                                                                            if (--(yyextra->import_stack_ptr) < 0) {
                                                                                                yyterminate();
                                                                                            }
                                                                                        }


"abstract"                                                                              RETURN(ABSTRACT_KEYWORD);
"as"                                                                                    RETURN(AS_KEYWORD);
"class"                                                                                 RETURN(CLASS_KEYWORD);
"else"                                                                                  RETURN(ELSE_KEYWORD);
"extends"                                                                               RETURN(EXTENDS_KEYWORD);
"false"                                                                                 RETURN(FALSE_LITERAL);
"final"                                                                                 RETURN(FINAL_KEYWORD);
"func"                                                                                  RETURN(FUNC_KEYWORD);
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

"&&"                                                                                    { yylval->operator = AST_BINARY_OPERATOR_DOUBLE_AND; RETURN(DOUBLE_AND_OPERATOR); }
"||"                                                                                    { yylval->operator = AST_BINARY_OPERATOR_DOUBLE_PIPE; RETURN(DOUBLE_PIPE_OPERATOR); }
"!"                                                                                     { yylval->operator = AST_UNARY_OPERATOR_NOT; RETURN(NOT_OPERATOR); }

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

{identifier}                                                                            {
                                                                                            str identifier = STR_NULL_INIT;
                                                                                            identifier.s   = yytext;
                                                                                            identifier.len = yyleng;

                                                                                            yylval->symbolValue = symbol_new(identifier, SYMBOL_TYPE_UNDEFINED,
                                                                                                                             yylloc->first_line, yylloc->first_column);
                                                                                            if (yylval->symbolValue) {
                                                                                                RETURN(IDENTIFIER);
                                                                                            } else {
                                                                                                yyerror(yylloc, yyextra->extraParser, "could not allocate memory for symbol");
                                                                                                RETURN(YYerror);
                                                                                            }
                                                                                        }

\n+                                                                                     RETURN_SEMICOLON_IF_NEEDED;

"//".*                                                                                  /* this is a comment skip */

<*>.                                                                                    { yyerror(yylloc, yyextra->extraParser, "unexpected input"); }

%%

static int parser_push_yy_state(parser_extra_lexer *lexerExtra, char *import, int importLength) {
    return 1;
}

static int parser_pop_yy_state(parser_extra_lexer *lexerExtra) {
    return 1;
}
