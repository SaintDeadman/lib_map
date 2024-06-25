include_directories(${CMAKE_SOURCE_DIR}/map_lib)
include_directories(${CMAKE_SOURCE_DIR}/map_lib/misc)

file(GLOB TEST_measuring_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/simple_measuring.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/map_lib/misc/*.c"
)
add_executable(run_measuring_simple ${TEST_measuring_SOURCES})
target_link_libraries(run_measuring_simple ${CUNIT_LIBRARIES} map_static pthread)
set_target_properties(run_measuring_simple PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB TEST_DOUBLE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/double_measuring.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/map_lib/misc/*.c"
)
add_executable(run_measuring_double ${TEST_DOUBLE_SOURCES})
target_link_libraries(run_measuring_double ${CUNIT_LIBRARIES} map_static pthread)
set_target_properties(run_measuring_double PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB TEST_HASHES_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/hash_measuring.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/map_lib/misc/*.c"
)
add_executable(run_measuring_hash ${TEST_HASHES_SOURCES})
target_link_libraries(run_measuring_hash ${CUNIT_LIBRARIES} map_static m)
set_target_properties(run_measuring_hash PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB TEST_STD_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/std_measuring.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/map_lib/misc/*.c"
)
add_executable(run_measuring_std ${TEST_STD_SOURCES})
target_link_libraries(run_measuring_std ${CUNIT_LIBRARIES} map_static pthread)
set_target_properties(run_measuring_std PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)