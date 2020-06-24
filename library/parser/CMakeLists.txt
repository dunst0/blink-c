cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

include("project-meta-info.in")

project(blink-parser
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C
        )

find_package(BISON 3.6.2)
find_package(FLEX 2.6.4)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src/")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/blink/")

BISON_TARGET(
        blink-parser src/blink.y
        "${CMAKE_CURRENT_BINARY_DIR}/src/parser.c"
        DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/blink/parser.h"
        )
FLEX_TARGET(
        blink-lexer src/blink.l
        "${CMAKE_CURRENT_BINARY_DIR}/src/lexer.c"
        )
ADD_FLEX_BISON_DEPENDENCY(blink-lexer blink-parser)

add_library(parser OBJECT)

target_sources(parser
        PRIVATE
        ${FLEX_blink-lexer_OUTPUTS}
        ${BISON_blink-parser_OUTPUT_SOURCE}
        PUBLIC
        ${BISON_blink-parser_OUTPUT_HEADER}
        )

target_include_directories(parser PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include")

target_link_libraries(parser PUBLIC utils)
