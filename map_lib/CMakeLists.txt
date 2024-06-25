cmake_minimum_required(VERSION 3.10)
project(map)

file(GLOB SOURCES 
    "*.c"
    "*.cpp"
    "misc/*.c"
)
file(GLOB HEADERS "map.h")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(map_static STATIC ${SOURCES})
add_library(map_shared SHARED ${SOURCES})

set_target_properties(map_static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj)
set_target_properties(map_shared PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj)

add_custom_command(TARGET map_static POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${HEADERS} ${CMAKE_BINARY_DIR}/obj)