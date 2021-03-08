cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include("project-meta-info.in")

project(waitui-utils
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

add_library(utils OBJECT)

target_sources(utils
        PRIVATE
        PUBLIC
        "include/waitui/str.h"
        )

target_include_directories(utils PUBLIC "include")
