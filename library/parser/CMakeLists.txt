cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include("project-meta-info.in")

project(waitui-parser
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C
        )

find_package(BISON 3.6.2)
find_package(FLEX 2.6.4)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/waitui/)

bison_target(
        waitui-parser src/waitui.y
        ${CMAKE_CURRENT_BINARY_DIR}/src/parser_impl.c
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/waitui/parser_impl.h
)
flex_target(
        waitui-lexer src/waitui.l
        ${CMAKE_CURRENT_BINARY_DIR}/src/lexer_impl.c
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/waitui/lexer_impl.h
)
add_flex_bison_dependency(waitui-lexer waitui-parser)

add_library(parser OBJECT)

target_sources(parser
        PRIVATE
        src/parser.c
        src/parser_helper.c
        include/waitui/parser_helper.h
        ${FLEX_waitui-lexer_OUTPUTS}
        ${FLEX_waitui-lexer_OUTPUT_HEADER}
        ${BISON_waitui-parser_OUTPUT_SOURCE}
        ${BISON_waitui-parser_OUTPUT_HEADER}
        PUBLIC
        include/waitui/parser.h
        )

target_include_directories(parser PUBLIC include PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include/waitui)

target_link_libraries(parser PUBLIC ast utils log)
