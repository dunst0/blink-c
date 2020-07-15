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

static int blink_needs_semicolon();

static int lastToken = -1;
#define RETURN(token)                                                       \
    lastToken = token;                                                      \
    fprintf(stdout, "Token: %s\tType: %d\tLine: %d\tColumn: %d\n",          \
            yytext, token, yylloc.first_line + 1, yylloc.first_column + 1); \
    return token;

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

                                                                                            if (!blink_push_yy_state(yytext, yyleng)) {
                                                                                                exit(1);
                                                                                            }

                                                                                            BEGIN(INITIAL);
                                                                                            RETURN(IMPORT_KEYWORD);
                                                                                        }
<PACKAGE>{not_whitespace}+                                                              {
                                                                                            printf("<PACKAGE_KEYWORD: %s>\n", yytext);
                                                                                            BEGIN(INITIAL);
                                                                                            RETURN(PACKAGE_KEYWORD);
                                                                                        }

<<EOF>>                                                                                 {
                                                                                            if (!blink_pop_yy_state()) {
                                                                                                yyterminate();
                                                                                            }
                                                                                        }


"abstract"                                                                              RETURN(ABSTRACT_KEYWORD);
"as"                                                                                    RETURN(AS_KEYWORD);
"class"                                                                                 RETURN(CLASS_KEYWORD);
"else"                                                                                  RETURN(ELSE_KEYWORD);
"extends"                                                                               RETURN(EXTENDS_KEYWORD);
"false"                                                                                 { yylval.value.s = yytext; yylval.value.len = yyleng; RETURN(FALSE_LITERAL); }
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
"private"                                                                               RETURN(PRIVATE_KEYWORD);
"protected"                                                                             RETURN(PROTECTED_KEYWORD);
"super"                                                                                 RETURN(SUPER_LITERAL);
"this"                                                                                  RETURN(THIS_LITERAL);
"true"                                                                                  { yylval.value.s = yytext; yylval.value.len = yyleng; RETURN(TRUE_LITERAL); }
"var"                                                                                   RETURN(VAR_KEYWORD);
"while"                                                                                 RETURN(WHILE_KEYWORD);

"++"                                                                                    RETURN(DOUBLE_PLUS_OPERATOR);
"--"                                                                                    RETURN(DOUBLE_MINUS_OPERATOR);

"+="                                                                                    { yylval.operator = PLUS_EQUAL_OPERATOR; RETURN(ASSIGNMENT); }
"-="                                                                                    { yylval.operator = MINUS_EQUAL_OPERATOR; RETURN(ASSIGNMENT); }
"*="                                                                                    { yylval.operator = TIMES_EQUAL_OPERATOR; RETURN(ASSIGNMENT); }
"/="                                                                                    { yylval.operator = DIV_EQUAL_OPERATOR; RETURN(ASSIGNMENT); }
"%="                                                                                    { yylval.operator = MODULO_EQUAL_OPERATOR; RETURN(ASSIGNMENT); }
"&="                                                                                    { yylval.operator = AND_EQUAL_OPERATOR; RETURN(ASSIGNMENT); }
"^="                                                                                    { yylval.operator = CARET_EQUAL_OPERATOR; RETURN(ASSIGNMENT); }
"~="                                                                                    { yylval.operator = TILDE_EQUAL_OPERATOR; RETURN(ASSIGNMENT); }
"|="                                                                                    { yylval.operator = PIPE_EQUAL_OPERATOR; RETURN(ASSIGNMENT); }

"&&"                                                                                    RETURN(DOUBLE_AND_OPERATOR);
"||"                                                                                    RETURN(DOUBLE_PIPE_OPERATOR);
"!"                                                                                     RETURN(NOT_OPERATOR);

"=="                                                                                    { yylval.operator = EQUAL_OPERATOR; RETURN(EQUALITY); }
"!="                                                                                    { yylval.operator = NOT_EQUAL_OPERATOR; RETURN(EQUALITY); }

"<"                                                                                     { yylval.operator = LESS_OPERATOR; RETURN(RELATIONAL); }
"<="                                                                                    { yylval.operator = LESS_EQUAL_OPERATOR; RETURN(RELATIONAL); }
">="                                                                                    { yylval.operator = GREATER_EQUAL_OPERATOR; RETURN(RELATIONAL); }
">"                                                                                     { yylval.operator = GREATER_OPERATOR; RETURN(RELATIONAL); }

[-.=+*%/&^~|:,{\[(}\]);]                                                                RETURN(yytext[0]);

0|{positive_number}                                                                     { yylval.value.s = yytext; yylval.value.len = yyleng; RETURN(INTEGER_LITERAL); }
((0|{positive_number})?\.{digit}+{exponent_part}?)|({positive_number}{exponent_part})   { yylval.value.s = yytext; yylval.value.len = yyleng; RETURN(DECIMAL_LITERAL); }
\"(\\.|[^"\\])*\"                                                                       { yylval.value.s = yytext + 1; yylval.value.len = yyleng - 1; RETURN(STRING_LITERAL); }

<METHOD>(operator{operator})|(({letter}|_)({letter}|{digit}|_)*)                        {
                                                                                            printf("<METHOD_NAME: %s>\n", yytext);
                                                                                            BEGIN(INITIAL);
                                                                                            RETURN(METHOD_NAME);
                                                                                        }
({letter}|_)({letter}|{digit}|_)*                                                       {
                                                                                            printf("<IDENTIFIER: %s>\n", yytext);
                                                                                            RETURN(IDENTIFIER);
                                                                                        }

\n+                                                                                     {
                                                                                            if (blink_needs_semicolon()) {
                                                                                                printf("$insert: ';'\n");
                                                                                                RETURN(';');
                                                                                            }
                                                                                            /* else skip */
                                                                                        }

"//".*                                                                                  /* this is a comment skip */

.                                                                                       { yyerror("unexpected input"); exit(1); }

%%

static int blink_needs_semicolon() {
    switch (lastToken) {
        case INTEGER_LITERAL:
        case DECIMAL_LITERAL:
        case STRING_LITERAL:
        case NULL_LITERAL:
        case THIS_LITERAL:
        case TRUE_LITERAL:
        case FALSE_LITERAL:
        case IDENTIFIER:
        case ')':
        case '}':
            return 1;
    }
    return 0;
}

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
