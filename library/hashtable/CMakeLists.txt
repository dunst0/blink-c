cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include("project-meta-info.in")

project(waitui-hashtable
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

add_library(hashtable OBJECT)

target_sources(hashtable
        PRIVATE
        "src/hashtable.c"
        PUBLIC
        "include/waitui/hashtable.h"
        )

target_include_directories(hashtable PUBLIC "include")

target_link_libraries(hashtable PUBLIC utils)
