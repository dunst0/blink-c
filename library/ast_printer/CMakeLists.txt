cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include("project-meta-info.in")

project(waitui-ast_printer
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

add_library(ast_printer OBJECT)

target_sources(ast_printer
        PRIVATE
        "src/ast_printer.c"
        PUBLIC
        "include/waitui/ast_printer.h"
        )

target_include_directories(ast_printer PUBLIC "include")

target_link_libraries(ast_printer PUBLIC ast log)
