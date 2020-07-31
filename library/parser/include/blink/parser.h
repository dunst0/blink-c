/**
 * @file parser.h
 * @author rick
 * @date 30.07.20
 * @brief File for the Parser implementation
 */

#ifndef BLINK_PARSER_H
#define BLINK_PARSER_H

#include "blink/parser_helper.h"

#include <blink/ast.h>
#include <blink/str.h>


// -----------------------------------------------------------------------------
//  Public types
// -----------------------------------------------------------------------------

/**
 * @brief Type TODO
 */
typedef struct parser {
    FILE *sourceFile;
    str sourceFileName;
    str currentDirectory;
    unsigned int debug;
    parser_extra_parser extraParser;
    parser_extra_lexer extraLexer;
} parser;


// -----------------------------------------------------------------------------
//  Public defines
// -----------------------------------------------------------------------------

#define PARSER_DEBUG_NONE 0
#define PARSER_DEBUG_LEXER 1U
#define PARSER_DEBUG_PARSER 2U


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

/**
 * @brief Create a Parser.
 * @param TODO
 * @param TODO
 * @param TODO
 * @return A pointer to Parser or NULL if memory allocation failed
 */
extern parser *parser_new(str sourceFileName, str currentDirectory,
                          unsigned int debug);

/**
 * @brief Destroy a Parser.
 * @param[in,out] this The Parser to destroy
 */
extern void parser_destroy(parser **this);

/**
 * @brief Parse the source file.
 * @param[in] this TODO
 * @return TODO
 */
extern int parser_parse(parser *this);

/**
 * @brief TODO
 * @return TODO
 */
extern ast *parser_get_ast(parser *this);

#endif//BLINK_PARSER_H
