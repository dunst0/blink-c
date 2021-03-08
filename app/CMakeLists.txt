cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include("project-meta-info.in")

project(waitui-app
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C
        )

add_executable(waitui)

target_sources(waitui
        PRIVATE
        "src/main.c"
        "${CMAKE_CURRENT_BINARY_DIR}/include/waitui/version.h"
        )

target_include_directories(waitui PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")

target_link_libraries(waitui PRIVATE ast list log parser symboltable hashtable)

configure_file(
        "include/waitui/version.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/include/waitui/version.h"
)
