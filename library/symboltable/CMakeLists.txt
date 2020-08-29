cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

include("project-meta-info.in")

project(waitui-symboltable
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

add_library(symboltable OBJECT)

target_sources(symboltable
        PRIVATE
        "src/symboltable.c"
        "src/symbol.c"
        "src/symbol_reference.c"
        PUBLIC
        "include/waitui/symboltable.h"
        "include/waitui/symbol.h"
        "include/waitui/symbol_reference.h"
        )

target_include_directories(symboltable PUBLIC "include")

target_link_libraries(symboltable PUBLIC hashtable list utils log)
