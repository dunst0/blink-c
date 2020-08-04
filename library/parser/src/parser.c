/**
 * @file parser.c
 * @author rick
 * @date 30.07.20
 * @brief File for the Parser implementation
 */

#include "blink/parser.h"

#include "blink/parser_impl.h"

#include "blink/lexer_impl.h"


// -----------------------------------------------------------------------------
//  Local variables
// -----------------------------------------------------------------------------

/**
 * @brief The source file name for input from stdin.
 */
static str parser_source_stdin = STR_STATIC_INIT("stdin");


// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------

parser *parser_new(str sourceFileName, str currentDirectory,
                   unsigned int debug) {
    parser *this = NULL;

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->debug = debug;

    /* make string length one longer to have '\0' for fopen */
    sourceFileName.len++;
    STR_COPY(&this->sourceFileName, &sourceFileName);
    STR_COPY(&this->currentDirectory, &currentDirectory);
    if (!this->sourceFileName.s || !this->currentDirectory.s) {
        parser_destroy(&this);
        return NULL;
    }
    sourceFileName.len--;

    this->extraParser.sourceFileName = this->sourceFileName;

    this->extraLexer.extraParser = &this->extraParser;
    this->extraLexer.lastToken   = -1;

    if (yylex_init_extra(&this->extraLexer, &this->extraParser.scanner) != 0) {
        parser_destroy(&this);
        return NULL;
    }

    FILE *sourceFile = NULL;

    if (parser_source_stdin.len == sourceFileName.len &&
        memcmp(parser_source_stdin.s, sourceFileName.s,
               parser_source_stdin.len) == 0) {
        sourceFile = stdin;
    } else {
        this->sourceFile = fopen(sourceFileName.s, "r");
        sourceFile       = this->sourceFile;

        if (!sourceFile) {
            fprintf(stderr, "Error: could not open filename: '%s'\n",
                    this->sourceFileName.s);
            parser_destroy(&this);
            return NULL;
        }
    }

    yyset_in(sourceFile, this->extraParser.scanner);
    if (this->debug & PARSER_DEBUG_LEXER) {
        yyset_debug(1, this->extraParser.scanner);
    }

    return this;
}

void parser_destroy(parser **this) {
    if (!this || !(*this)) { return; }

    if ((*this)->extraParser.scanner) {
        yylex_destroy((*this)->extraParser.scanner);
    }
    if ((*this)->sourceFile) { fclose((*this)->sourceFile); }
    if ((*this)->sourceFileName.s) { free((*this)->sourceFileName.s); }
    if ((*this)->currentDirectory.s) { free((*this)->currentDirectory.s); }

    free(*this);
    *this = NULL;
}

int parser_parse(parser *this) {
    if (!this) { return 0; }

    if (this->debug & PARSER_DEBUG_PARSER) { yydebug = 1; }

    if (yyparse(&this->extraParser) != 0) { return 0; }

    return 1;
}

ast *parser_get_ast(parser *this) {
    if (!this) { return 0; }

    return this->extraParser.resultAst;
}
