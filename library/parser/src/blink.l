/**
 * Lexer for the blink language
 */

%option noyywrap

%{

#include "blink/str.h"
#include "blink/parser.h"

#include <stdio.h>

#define MAX_IMPORT_DEPTH 1024
static struct blink_yy_state {
    YY_BUFFER_STATE state;
    str filename;
    int first_line;
    int last_line;
    int first_column;
    int last_column;
} import_stack[MAX_IMPORT_DEPTH];
int import_stack_ptr = 0;

#define YY_USER_ACTION                          \
    yylloc.filename     = yylloc.filename;      \
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

static int blink_push_yy_state(char *import, int importLength);
static int blink_pop_yy_state();

%}

%x IMPORT
%x PACKAGE

letter              [a-zA-Z]
digit               [0-9]
positive_number     [1-9]{digit}*
exponent_part       [eE][-+]?{positive_number}
operator            ([-+*\/=><!&%~$^]=?)|\+\+|--
whitespace          [ \f\t\r]

%%

<INITIAL>{whitespace}                                                                   /* eat the whitespace */

<INITIAL>"import"                                                                       BEGIN(IMPORT);
<IMPORT,PACKAGE>[ \t]*                                                                  /* eat the whitespace */
<IMPORT>[^ \t\n]+                                                                       {
                                                                                            if (!blink_push_yy_state(yytext, yyleng)) {
                                                                                                exit(1);
                                                                                            }

                                                                                            BEGIN(INITIAL);
                                                                                            return IMPORT_KEYWORD;
                                                                                        }

<INITIAL>"package"                                                                      BEGIN(PACKAGE);
<PACKAGE>[^ \t\n]+                                                                      {
                                                                                            printf("now in package name: %s\n", yytext);
                                                                                            BEGIN(INITIAL);
                                                                                            return PACKAGE_KEYWORD;
                                                                                        }
<<EOF>>                                                                                 {
                                                                                            if (!blink_pop_yy_state()) {
                                                                                                yyterminate();
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

0|{positive_number}                                                                     { printf("<INTEGER_LITERAL: %s>\n", yytext); return INTEGER_LITERAL; }
((0|{positive_number})?\.{digit}+{exponent_part}?)|({positive_number}{exponent_part})   { printf("<DECIMAL_LITERAL: %s>\n", yytext); return DECIMAL_LITERAL; }
\"(\\.|[^"\\])*\"                                                                       { printf("<STRING_LITERAL: %s>\n", yytext); return STRING_LITERAL; }

{letter}({letter}|{digit}|_)*                                                           { printf("<CLASS_NAME: %s>\n", yytext); return CLASS_NAME; }
{letter}({letter}|{digit}|_|\.)*({letter}|{digit}|_)*                                   { printf("<FULL_QULIFIED_NAME: %s>\n", yytext); return FULL_QULIFIED_NAME; }
({letter}|_)({letter}|{digit}|_)*                                                       { printf("<IDENTIFIER: %s>\n", yytext); return IDENTIFIER; }
(operator{operator})|(({letter}|_)({letter}|{digit}|_)*)                                { printf("<METHOD_NAME: %s>\n", yytext); return METHOD_NAME; }

\n+                                                                                     /* return NL; */
{whitespace}                                                                            /* eat the whitespace */
"//".*\n+                                                                               /* this is a comment skip */

.                                                                                       { yyerror("unexpected input"); exit(1); } /* error */

%%

#define FILE_EXTENSION ".blink"

static int blink_getFileNameForImport(char *import, int importLength, str *result) {
    result->len = importLength + sizeof(FILE_EXTENSION);
    result->s   = calloc(result->len, sizeof(*result->s));
    if(!result->s) {
        fprintf(stderr, "Error: could not allocate memory for import filename\n");
        return 0;
    }

    for(int i = 0; i < importLength; ++i) {
        result->s[i] = (import[i] == '.') ? '/' : import[i];
    }
    memcpy(result->s + importLength, FILE_EXTENSION, sizeof(FILE_EXTENSION) - 1);

    return 1;
}

static int blink_push_yy_state(char *import, int importLength) {
    str sourceFileName = STR_NULL;

    if (import_stack_ptr >= MAX_IMPORT_DEPTH) {
        fprintf(stderr, "Error: imports nested too deeply\n");
        return 0;
    }

    if (!blink_getFileNameForImport(import, importLength, &sourceFileName)) {
        return 0;
    }
    fprintf(stdout, "current import filename: '%.*s'\n", STR_FMT(&sourceFileName));

    import_stack[import_stack_ptr].state        = YY_CURRENT_BUFFER;
    import_stack[import_stack_ptr].filename     = yylloc.filename;
    import_stack[import_stack_ptr].first_line   = yylloc.first_line;
    import_stack[import_stack_ptr].last_line    = yylloc.last_line;
    import_stack[import_stack_ptr].first_column = yylloc.first_column;
    import_stack[import_stack_ptr].last_column  = yylloc.last_column;
    import_stack_ptr++;

    yylloc.first_line   = yylloc.last_line   = 0;
    yylloc.first_column = yylloc.last_column = 0;
    yylloc.filename     = sourceFileName;

    yyin = fopen(sourceFileName.s, "r");
    if(!yyin) {
        fprintf(stderr, "Error: could not open file: '%.*s'\n", STR_FMT(&sourceFileName));
        return 0;
    }

    yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));

    return 1;
}

static int blink_pop_yy_state() {
    if (import_stack[import_stack_ptr].filename.s) {
        free(import_stack[import_stack_ptr].filename.s);
        import_stack[import_stack_ptr].filename.s   = NULL;
        import_stack[import_stack_ptr].filename.len = 0;
    }

    if (--import_stack_ptr < 0) {
        return 0;
    }

    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(import_stack[import_stack_ptr].state);

    yylloc.filename     = import_stack[import_stack_ptr].filename;
    yylloc.first_line   = import_stack[import_stack_ptr].first_line;
    yylloc.last_line    = import_stack[import_stack_ptr].last_line;
    yylloc.first_column = import_stack[import_stack_ptr].first_column;
    yylloc.last_column  = import_stack[import_stack_ptr].last_column;

    return 1;
}
