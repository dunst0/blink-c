cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include("project-meta-info.in")

project(waitui-log
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

add_library(log OBJECT)

target_sources(log
        PRIVATE
        "src/log.c"
        PUBLIC
        "include/waitui/log.h"
        )

target_include_directories(log PUBLIC "include")

target_compile_definitions(log PUBLIC "LOG_USE_COLOR")
