#include "blink/parser.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Hello, World!\n");

    int length = snprintf(NULL, 0, "%s\n", "stdin");
    filename = calloc(length + 1, sizeof(char));
    if (!filename) {
        yyerror("out of memory");
        exit(1);
    }
    snprintf(filename, length + 1, "stdin");

    yydebug = 1;

    if (!yyparse()) {
        printf("blink parse worked\n");
    } else {
        printf("blink parse failed\n");
    }

    return 0;
}
