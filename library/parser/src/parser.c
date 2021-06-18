/**
 * @file parser.c
 * @author rick
 * @date 30.07.20
 * @brief File for the Parser implementation
 */

#include "waitui/parser.h"

// clang-format off
#include "waitui/parser_impl.h"
#include "waitui/lexer_impl.h"
// clang-format on

#include <waitui/log.h>


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

parser *parser_new(str sourceFileName, str workingDirectory,
                   unsigned int debug) {
    parser *this = NULL;

    log_trace("creating new parser");

    this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }

    this->debug = debug;

    STR_COPY_WITH_NUL(&this->sourceFileName, &sourceFileName);
    if (!this->sourceFileName.s) {
        log_fatal("could not allocate memory for sourceFileName");
        parser_destroy(&this);
        return NULL;
    }
    this->extraParser.sourceFileName = this->sourceFileName;

    STR_COPY_WITH_NUL(&this->workingDirectory, &workingDirectory);
    if (!this->workingDirectory.s) {
        log_fatal("could not allocate memory for workingDirectory");
        parser_destroy(&this);
        return NULL;
    }

    this->extraParser.symtable = symboltable_new();
    if (!this->extraParser.symtable) {
        log_fatal("could not create symboltable");
        parser_destroy(&this);
        return NULL;
    }

    this->extraLexer.importStack = parser_yy_state_list_new();
    if (!this->extraLexer.importStack) {
        log_fatal("could not allocate memory for importStack");
        parser_destroy(&this);
        return NULL;
    }

    this->extraLexer.extraParser = &this->extraParser;
    this->extraLexer.lastToken   = -1;

    if (yylex_init_extra(&this->extraLexer, &this->extraParser.scanner) != 0) {
        log_fatal("could not initialize the lexer with extra data");
        parser_destroy(&this);
        return NULL;
    }

    if (parser_source_stdin.len == sourceFileName.len &&
        memcmp(parser_source_stdin.s, sourceFileName.s,
               parser_source_stdin.len) == 0) {
        this->sourceFile = stdin;
    } else {
        this->sourceFile = fopen(sourceFileName.s, "r");
        if (!this->sourceFile) {
            log_error("could not open filename: '%s'", this->sourceFileName.s);
            parser_destroy(&this);
            return NULL;
        }
    }

    yyset_in(this->sourceFile, this->extraParser.scanner);
    if (this->debug & PARSER_DEBUG_LEXER) {
        yyset_debug(1, this->extraParser.scanner);
    }

    log_trace("new parser successful created");

    return this;
}

void parser_destroy(parser **this) {
    log_trace("destroying parser");

    if (!this || !(*this)) { return; }

    parser_yy_state_list_destroy(&(*this)->extraLexer.importStack);
    symboltable_destroy(&(*this)->extraParser.symtable);
    if ((*this)->extraParser.scanner) {
        yylex_destroy((*this)->extraParser.scanner);
    }
    if ((*this)->sourceFile && (*this)->sourceFile != stdin) {
        fclose((*this)->sourceFile);
    }
    STR_FREE(&(*this)->sourceFileName);
    STR_FREE(&(*this)->workingDirectory);

    free(*this);
    *this = NULL;

    log_trace("parser successful destroyed");
}

int parser_parse(parser *this) {
    if (!this) { return 0; }

    if (this->debug & PARSER_DEBUG_PARSER) { yydebug = 1; }

    if (yyparse(&this->extraParser) != 0) { return 0; }

    return 1;
}

ast *parser_get_ast(parser *this) {
    log_trace("getting ast from parser");

    if (!this) { return 0; }

    return this->extraParser.resultAst;
}
