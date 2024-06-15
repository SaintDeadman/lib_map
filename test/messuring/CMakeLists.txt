include_directories(${CMAKE_SOURCE_DIR}/map_lib)
include_directories(${CMAKE_SOURCE_DIR}/map_lib/misc)

file(GLOB TEST_MESSURING_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/simple_messuring.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/map_lib/misc/*.c"
)
add_executable(run_messuring_simple ${TEST_MESSURING_SOURCES})
target_link_libraries(run_messuring_simple ${CUNIT_LIBRARIES} map_static pthread)
set_target_properties(run_messuring_simple PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB TEST_DOUBLE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/double_messuring.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/map_lib/misc/*.c"
)
add_executable(run_messuring_double ${TEST_DOUBLE_SOURCES})
target_link_libraries(run_messuring_double ${CUNIT_LIBRARIES} map_static pthread)
set_target_properties(run_messuring_double PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB TEST_HASHES_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/hash_messuring.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/map_lib/misc/*.c"
)
add_executable(run_messuring_hash ${TEST_HASHES_SOURCES})
target_link_libraries(run_messuring_hash ${CUNIT_LIBRARIES} map_static m)
set_target_properties(run_messuring_hash PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)