cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

include("project-meta-info.in")

project(blink-hashtable
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

add_library(hashtable OBJECT)

target_sources(hashtable
        PRIVATE
        "src/hashtable.c"
        PUBLIC
        "include/blink/hashtable.h"
        )

target_include_directories(hashtable PUBLIC "include")

target_link_libraries(hashtable PUBLIC utils)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
