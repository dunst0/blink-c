cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

include("project-meta-info.in")

project(blink-ast
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

add_library(ast OBJECT)

target_sources(ast
        PRIVATE
        "src/ast.c"
        "src/ast_node.c"
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/blink/ast.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/blink/ast_node.h"
        )

target_include_directories(ast PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(ast PUBLIC list utils)
