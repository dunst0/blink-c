%{
#include <stdio.h>
#include "blink/parser.h"

#define MAX_IMPORT_DEPTH 1024
YY_BUFFER_STATE import_stack[MAX_IMPORT_DEPTH];
int import_stack_ptr = 0;

#define YY_USER_ACTION                          \
    yylloc.filename     = filename;             \
    yylloc.first_line   = yylloc.last_line;     \
    yylloc.first_column = yylloc.last_column;   \
    for(int i = 0; yytext[i] != '\0'; ++i) {    \
        if(yytext[i] == '\n') {                 \
            yylloc.last_line++;                 \
            yylloc.last_column = 0;             \
        } else {                                \
            yylloc.last_column++;               \
        }                                       \
    }

%}

%x IMPORT
%x PACKAGE

%option noyywrap

letter              [a-zA-Z]
digit               [0-9]
postive_number      [1-9]{digit}*
exponent_part       [eE][-+]?{postive_number}
operator            ([-+*\/=><!&%~$^]=?)|\+\+|--
whitespace          [ \n\t\r]

%%

"import"                                                                                BEGIN(IMPORT);
<IMPORT,PACKAGE>[ \t]*                                                                  /* eat the whitespace */
<IMPORT>[^ \t\n]+                                                                       {
                                                                                            /* got the include file name */
                                                                                            if(import_stack_ptr >= MAX_IMPORT_DEPTH) {
                                                                                                fprintf(stderr, "Includes nested too deeply");
                                                                                                exit(1);
                                                                                            }

                                                                                            import_stack[import_stack_ptr++] = YY_CURRENT_BUFFER;

                                                                                            int length = snprintf(NULL, 0, "%s\n", yytext);
                                                                                            if(filename) {
                                                                                                free(filename);
                                                                                            }
                                                                                            filename = calloc(length + 1, sizeof(char));
                                                                                            if(!filename) {
                                                                                                yyerror("out of memory");
                                                                                                exit(1);
                                                                                            }
                                                                                            memcpy(filename, yytext, length);
                                                                                            printf("filename: '%s'\n", filename);

                                                                                            /* TODO: need to convert to path */
                                                                                            yyin = fopen(filename, "r");
                                                                                            if(!yyin) {
                                                                                                yyerror("error loading file");
                                                                                                exit(1);
                                                                                            }

                                                                                            yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));

                                                                                            BEGIN(INITIAL);
                                                                                            return IMPORT_KEYWORD;
                                                                                        }
"package"                                                                               BEGIN(PACKAGE);
<PACKAGE>[^ \t\n]+                                                                      {
                                                                                            printf("now in package name: %s\n", yytext);
                                                                                            BEGIN(INITIAL);
                                                                                            return PACKAGE_KEYWORD;
                                                                                        }
<<EOF>>                                                                                 {
                                                                                            if(--import_stack_ptr < 0) {
                                                                                                yyterminate();
                                                                                            } else {
                                                                                                yy_switch_to_buffer(import_stack[import_stack_ptr]);
                                                                                            }
                                                                                        }


"abstract"                                                                              return ABSTRACT_KEYWORD;
"as"                                                                                    return AS_KEYWORD;
"class"                                                                                 return CLASS_KEYWORD;
"else"                                                                                  return ELSE_KEYWORD;
"extends"                                                                               return EXTENDS_KEYWORD;
"false"                                                                                 return FALSE_LITERAL;
"final"                                                                                 return FINAL_KEYWORD;
"func"                                                                                  return FUNC_KEYWORD;
"if"                                                                                    return IF_KEYWORD;
"in"                                                                                    return IN_KEYWORD;
"lazy"                                                                                  return LAZY_KEYWORD;
"let"                                                                                   return LET_KEYWORD;
"new"                                                                                   return NEW_KEYWORD;
"null"                                                                                  return NULL_LITERAL;
"override"                                                                              return OVERRIDE_KEYWORD;
"private"                                                                               return PRIVATE_KEYWORD;
"protected"                                                                             return PROTECTED_KEYWORD;
"super"                                                                                 return SUPER_LITERAL;
"this"                                                                                  return THIS_LITERAL;
"true"                                                                                  return TRUE_LITERAL;
"var"                                                                                   return VAR_KEYWORD;
"while"                                                                                 return WHILE_KEYWORD;

"++"                                                                                    return DOUBLE_PLUS_OPERATOR;
"--"                                                                                    return DOUBLE_MINUS_OPERATOR;

"+="                                                                                    { yylval.operator = PLUS_EQUAL_OPERATOR; return ASSIGNMENT; }
"-="                                                                                    { yylval.operator = MINUS_EQUAL_OPERATOR; return ASSIGNMENT; }
"*="                                                                                    { yylval.operator = TIMES_EQUAL_OPERATOR; return ASSIGNMENT; }
"/="                                                                                    { yylval.operator = DIV_EQUAL_OPERATOR; return ASSIGNMENT; }
"%="                                                                                    { yylval.operator = MODULO_EQUAL_OPERATOR; return ASSIGNMENT; }
"&="                                                                                    { yylval.operator = AND_EQUAL_OPERATOR; return ASSIGNMENT; }
"^="                                                                                    { yylval.operator = CARRET_EQUAL_OPERATOR; return ASSIGNMENT; }
"~="                                                                                    { yylval.operator = TILDE_EQUAL_OPERATOR; return ASSIGNMENT; }
"|="                                                                                    { yylval.operator = PIPE_EQUAL_OPERATOR; return ASSIGNMENT; }

"&&"                                                                                    return DOUBLE_AND_OPERATOR;
"||"                                                                                    return DOUBLE_PIPE_OPERATOR;
"!"                                                                                     return NOT_OPERATOR;

"=="                                                                                    { yylval.operator = EQUAL_OPERATOR; return EQUALITY; }
"!="                                                                                    { yylval.operator = NOT_EQUAL_OPERATOR; return EQUALITY; }

"<"                                                                                     { yylval.operator = LESS_OPERATOR; return RELATIONAL; }
"<="                                                                                    { yylval.operator = LESS_EQUAL_OPERATOR; return RELATIONAL; }
">="                                                                                    { yylval.operator = GREATER_EQUAL_OPERATOR; return RELATIONAL; }
">"                                                                                     { yylval.operator = GREATER_OPERATOR; return RELATIONAL; }

[-.=+*/%&^~|:,{\[(}\])]                                                                 return yytext[0];

0|{postive_number}                                                                      { printf("<INTEGER_LITERAL: %s>\n", yytext); return INTEGER_LITERAL; }
((0|{postive_number})?\.{digit}+{exponent_part}?)|({postive_number}{exponent_part})     { printf("<DECIMAL_LITERAL: %s>\n", yytext); return DECIMAL_LITERAL; }
\"(\\.|[^"\\])*\"                                                                       { printf("<STRING_LITERAL: %s>\n", yytext); return STRING_LITERAL; }

{letter}({letter}|{digit}|_)*                                                           { printf("<CLASS_NAME: %s>\n", yytext); return CLASS_NAME; }
{letter}({letter}|{digit}|_|\.)*({letter}|{digit}|_)*                                   { printf("<FULL_QULIFIED_NAME: %s>\n", yytext); return FULL_QULIFIED_NAME; }
({letter}|_)({letter}|{digit}|_)*                                                       { printf("<IDENTIFIER: %s>\n", yytext); return IDENTIFIER; }
(operator{operator})|(({letter}|_)({letter}|{digit}|_)*)                                { printf("<METHOD_NAME: %s>\n", yytext); return METHOD_NAME; }

{whitespace}                                                                            /* eat the whitespace */
"//".*                                                                                  /* this is a comment skip */

.                                                                                       { yyerror("unexpected input"); exit(1); } /* error */

%%

