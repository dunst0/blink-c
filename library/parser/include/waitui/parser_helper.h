/**
 * @file parser_helper.h
 * @author rick
 * @date 31.07.20
 * @brief Helping file for the Parser implementation
 */

#ifndef WAITUI_PARSER_HELPER_H
#define WAITUI_PARSER_HELPER_H

#include <waitui/ast.h>
#include <waitui/list.h>
#include <waitui/symboltable.h>


// -----------------------------------------------------------------------------
//  Public defines
// -----------------------------------------------------------------------------

/**
 * @brief TODO
 */
#define MAX_IMPORT_DEPTH 1024


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief TODO
 */
typedef struct parser_yy_state {
    void *state;
    str filename;
    int first_line;
    int last_line;
    int first_column;
    int last_column;
} parser_yy_state;

extern parser_yy_state *parser_yy_state_new(str filename, int first_line,
                                            int last_line, int first_column,
                                            int last_column, void *state);

extern void parser_yy_state_destroy(parser_yy_state **this);

CREATE_LIST_TYPE(INTERFACE, parser_yy_state)

/**
 * @brief Type for extra parser data.
 */
typedef struct parser_extra_parser {
    void *scanner;
    waitui_ast *resultAst;
    str sourceFileName;
    symboltable *symtable;
} parser_extra_parser;

/**
 * @brief Type for extra lexer data.
 */
typedef struct parser_extra_lexer {
    int lastToken;
    parser_yy_state_list *importStack;
    int import_stack_ptr;
    parser_extra_parser *extraParser;
} parser_extra_lexer;


#endif//WAITUI_PARSER_HELPER_H
