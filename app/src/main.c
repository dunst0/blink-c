#include "waitui/version.h"

#include <waitui/log.h>
#include <waitui/ast_printer.h>
#include <waitui/parser.h>
#include <waitui/str.h>

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>


// -----------------------------------------------------------------------------
//  Local defines
// -----------------------------------------------------------------------------

#define WAITUI_SUCCESS 0
#define WAITUI_FAILURE 1
#define WAITUI_OTHER_ERROR 2


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
    int result = WAITUI_SUCCESS;

    parser *waituiParser = NULL;
    waitui_ast *waituiAst       = NULL;
    FILE *graphFile      = NULL;

    if (argc == 2) {
        sourceFileName.len = strlen(argv[1]);
        sourceFileName.s   = argv[1];
    }

    waitui_log_setLevel(WAITUI_LOG_DEBUG);
    waitui_log_setQuiet(false);

    parserDebug = PARSER_DEBUG_NONE | PARSER_DEBUG_LEXER | PARSER_DEBUG_PARSER;

    waitui_log_debug("waitui start execution");

    waituiParser = parser_new(sourceFileName, currentDirectory, parserDebug);
    if (!waituiParser) {
        result = WAITUI_OTHER_ERROR;
        goto done;
    }

    waitui_log_trace("start parsing input");
    if (!parser_parse(waituiParser)) {
        waitui_log_fatal("parsing input failed");
        result = WAITUI_FAILURE;
        goto done;
    }
    waitui_log_debug("input was parsed successful");

    waituiAst = parser_get_ast(waituiParser);
    if (!waituiAst) {
        result = WAITUI_OTHER_ERROR;
        goto done;
    }

    parser_destroy(&waituiParser);

    graphFileName.len = sourceFileName.len + sizeof(".dot");
    graphFileName.s   = calloc(graphFileName.len, sizeof(*graphFileName.s));
    if (!graphFileName.s) {
        result = WAITUI_OTHER_ERROR;
        goto done;
    }
    memcpy(graphFileName.s, sourceFileName.s, sourceFileName.len);
    snprintf(graphFileName.s + sourceFileName.len, graphFileName.len, "%s",
             ".dot");

    graphFile = fopen(graphFileName.s, "w");
    if (!graphFile) {
        result = WAITUI_OTHER_ERROR;
        goto done;
    }
    waitui_ast_printer_generateGraph(waituiAst, graphFile);

    waitui_log_debug("waitui execution done");

done:
    if (graphFile) { fclose(graphFile); }
    if (graphFileName.s) { free(graphFileName.s); }
    ast_destroy(&waituiAst);
    parser_destroy(&waituiParser);

    return result;
}
