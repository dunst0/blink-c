cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

include("project-meta-info.in")

project(blink-list
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

add_library(list OBJECT)

target_sources(list
        PRIVATE
        "src/list.c"
        PUBLIC
        "include/blink/list.h"
        )

target_include_directories(list PUBLIC "include")
