cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include("project-meta-info.in")

project(waitui-ast
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

add_library(ast OBJECT)

target_sources(ast
        PRIVATE
        "src/ast.c"
        "src/ast_node.c"
        "src/ast_printer.c"
        PUBLIC
        "include/waitui/ast.h"
        "include/waitui/ast_node.h"
        "include/waitui/ast_printer.h"
        )

target_include_directories(ast PUBLIC "include")

target_link_libraries(ast PUBLIC list symboltable utils log)
