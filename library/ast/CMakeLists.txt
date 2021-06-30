cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include("project-meta-info.in")

project(waitui-ast
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

add_library(ast OBJECT)

target_compile_features(ast PRIVATE c_std_11)

target_sources(ast
        PRIVATE
        "src/ast.c"
        "src/ast_node.c"
        PUBLIC
        "include/waitui/ast.h"
        "include/waitui/ast_node.h"
        )

target_include_directories(ast PUBLIC "include")

target_link_libraries(ast PUBLIC list symboltable utils log)
