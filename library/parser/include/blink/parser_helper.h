/**
 * @file parser_helper.h
 * @author rick
 * @date 31.07.20
 * @brief Helping file for the Parser implementation
 */

#ifndef BLINK_PARSER_HELPER_H
#define BLINK_PARSER_HELPER_H

#include <blink/ast.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Type TODO
 */
typedef struct parser_extra_parser {
    void *scanner;
    ast *resultAst;
    str sourceFileName;
} parser_extra_parser;

/**
 * @brief Type TODO
 */
typedef struct parser_extra_lexer {
    int lastToken;
    parser_extra_parser *extraParser;
} parser_extra_lexer;

#endif//BLINK_PARSER_HELPER_H
