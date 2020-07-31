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
    } else  {
        fprintf(stderr, "Debug: parsing worked\n");
    }

done:
    parser_destroy(&blinkParser);

    return result;
}
