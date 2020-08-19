cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

include("project-meta-info.in")

project(blink-symboltable
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

add_library(symboltable OBJECT)

target_sources(symboltable
        PRIVATE
        "src/symboltable.c"
        "src/symbol.c"
        PUBLIC
        "include/blink/symboltable.h"
        "include/blink/symbol.h"
        )

target_include_directories(symboltable PUBLIC "include")

target_link_libraries(symboltable PUBLIC hashtable list utils)
