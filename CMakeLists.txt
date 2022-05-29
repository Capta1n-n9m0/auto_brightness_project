cmake_minimum_required(VERSION 3.22)
project(auto_brightness_project C)

set(CMAKE_C_STANDARD 23)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif ()

add_executable(auto_brightness_project main.c bmp_core.c bmp_core.h auto_brightness.c auto_brightness.h)
target_link_libraries(auto_brightness_project m)