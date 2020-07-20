#include "blink/parser.h"
#include "blink/str.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    extern FILE *yyin;

    FILE *graphFile    = NULL;
    char *sourceFile   = "stdin";
    str sourceFileName = STR_NULL;
    int loadFromFile   = 0;
    ast *astResult     = NULL;

    if (argc == 2) {
        sourceFile   = argv[1];
        loadFromFile = 1;
    }

    sourceFileName.len = snprintf(NULL, 0, "%s", sourceFile) + 1;
    sourceFileName.s   = calloc(sourceFileName.len, sizeof(*sourceFileName.s));
    if (!sourceFileName.s) {
        fprintf(stderr, "Error: could not allocate memory for filename\n");
        return 2;
    }
    snprintf(sourceFileName.s, sourceFileName.len, "%s", sourceFile);

    if (loadFromFile) {
        yyin = fopen(sourceFileName.s, "r");
    } else {
        yyin = stdin;
    }

    printf("%s: start parsing from '%.*s'\n", argv[0], STR_FMT(&sourceFileName));

    yydebug         = 0;
    yylloc.filename = sourceFileName;

    if (yyparse() != 0) {
        printf("blink parse failed\n");
        return 1;
    }
    printf("blink parse worked\n");

    astResult = parser_get_ast();
    if (!astResult) {
        fprintf(stderr, "Error: could not allocate memory for ast\n");
        return 1;
    }

    graphFile = fopen("graph.dot", "w");
    ast_generate_graph(astResult, graphFile);

    ast_destroy(&astResult);

    return 0;
}
