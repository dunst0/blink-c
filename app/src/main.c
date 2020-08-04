#include "blink/version.h"

#include <blink/parser.h>
#include <blink/str.h>
#include <blink/symboltable.h>

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Local defines
// -----------------------------------------------------------------------------

#define BLINK_SUCCESS 0
#define BLINK_FAILURE 1
#define BLINK_OTHER_ERROR 2


// -----------------------------------------------------------------------------
//  Local variables
// -----------------------------------------------------------------------------

static str sourceFileName   = STR_STATIC_INIT("stdin");
static str currentDirectory = STR_NULL_INIT;
static str graphFileName    = STR_NULL_INIT;
static int parserDebug      = PARSER_DEBUG_NONE;


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
//  Main function
// -----------------------------------------------------------------------------

int main(int argc, char **argv) {
    int result = BLINK_SUCCESS;

    parser *blinkParser = NULL;
    ast *blinkAst       = NULL;
    FILE *graphFile     = NULL;

    if (argc == 2) {
        sourceFileName.len = strlen(argv[1]);
        sourceFileName.s   = argv[1];
    }

    parserDebug = PARSER_DEBUG_LEXER | PARSER_DEBUG_PARSER;

    blinkParser = parser_new(sourceFileName, currentDirectory, parserDebug);
    if (!blinkParser) {
        result = BLINK_OTHER_ERROR;
        goto done;
    }

    if (!parser_parse(blinkParser)) {
        fprintf(stderr, "Error: parsing failed\n");
        result = BLINK_FAILURE;
        goto done;
    } else {
        fprintf(stderr, "Debug: parsing worked\n");
    }

    blinkAst = parser_get_ast(blinkParser);
    if (!blinkAst) {
        result = BLINK_OTHER_ERROR;
        goto done;
    }

    graphFileName.len = sourceFileName.len + sizeof(".dot");
    graphFileName.s   = calloc(graphFileName.len, sizeof(*graphFileName.s));
    if (!graphFileName.s) {
        result = BLINK_OTHER_ERROR;
        goto done;
    }
    memcpy(graphFileName.s, sourceFileName.s, sourceFileName.len);
    snprintf(graphFileName.s + sourceFileName.len, graphFileName.len, "%s",
             ".dot");

    graphFile = fopen(graphFileName.s, "w");
    if (!graphFile) {
        result = BLINK_OTHER_ERROR;
        goto done;
    }
    ast_generate_graph(blinkAst, graphFile);

done:
    if (graphFile) { fclose(graphFile); }
    if (graphFileName.s) { free(graphFileName.s); }
    ast_destroy(&blinkAst);
    parser_destroy(&blinkParser);

    return result;
}
