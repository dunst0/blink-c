cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

include("${CMAKE_CURRENT_LIST_DIR}/project-meta-info.in")

project(blink
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C)

add_subdirectory(app)
