cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include("${CMAKE_CURRENT_LIST_DIR}/project-meta-info.in")

project(waitui
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C
        )

add_subdirectory(app)
add_subdirectory(library/ast)
add_subdirectory(library/ast_printer)
add_subdirectory(library/hashtable)
add_subdirectory(library/list)
add_subdirectory(library/log)
add_subdirectory(library/parser)
add_subdirectory(library/symboltable)
add_subdirectory(library/utils)
