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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/blink/)

bison_target(
        blink-parser src/blink.y
        ${CMAKE_CURRENT_BINARY_DIR}/src/parser_impl.c
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/blink/parser_impl.h
)
flex_target(
        blink-lexer src/blink.l
        ${CMAKE_CURRENT_BINARY_DIR}/src/lexer_impl.c
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/blink/lexer_impl.h
)
add_flex_bison_dependency(blink-lexer blink-parser)

add_library(parser OBJECT)

target_sources(parser
        PRIVATE
        src/parser.c
        ${FLEX_blink-lexer_OUTPUTS}
        ${FLEX_blink-lexer_OUTPUT_HEADER}
        ${BISON_blink-parser_OUTPUT_SOURCE}
        ${BISON_blink-parser_OUTPUT_HEADER}
        PUBLIC
        include/blink/parser.h
        )

target_include_directories(parser PUBLIC include PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)

target_link_libraries(parser PUBLIC ast utils)
