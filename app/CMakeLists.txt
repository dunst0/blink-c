cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

include("project-meta-info.in")

project(blink-app
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C
        )

add_executable(blink)

target_sources(blink
        PRIVATE
        "src/main.c"
        "${CMAKE_CURRENT_BINARY_DIR}/include/blink/version.h"
        )

target_include_directories(blink PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")

target_link_libraries(blink PRIVATE ast list parser symboltable hashtable)

configure_file(
        "include/blink/version.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/include/blink/version.h"
)
