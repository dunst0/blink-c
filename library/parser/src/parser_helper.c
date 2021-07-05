/**
 * @file parser.c
 * @author rick
 * @date 01.04.21
 * @brief File for the Parser Helper implementation
 */

#include "waitui/parser_helper.h"

parser_yy_state *parser_yy_state_new(str filename, int first_line,
                                     int last_line, int first_column,
                                     int last_column, void *state) {
    return NULL;
}

void parser_yy_state_destroy(parser_yy_state **this) {}

CREATE_LIST_TYPE(IMPLEMENTATION, parser_yy_state)
