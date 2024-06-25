include_directories(${CMAKE_SOURCE_DIR}/map_lib)
include_directories(${CMAKE_SOURCE_DIR}/map_lib/misc)

file(GLOB TEST00_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/00_simple_map.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/common.c"
)

add_executable(run_test_00 ${TEST00_SOURCES})
target_link_libraries(run_test_00 ${CUNIT_LIBRARIES} map_static)
set_target_properties(run_test_00 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB TEST01_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/01_double_map.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/common.c"
)

add_executable(run_test_01 ${TEST01_SOURCES})
target_link_libraries(run_test_01 ${CUNIT_LIBRARIES} map_static)
set_target_properties(run_test_01 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB TEST02_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/02_std_map.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/common.c"
)

add_executable(run_test_02 ${TEST02_SOURCES})
target_link_libraries(run_test_02 ${CUNIT_LIBRARIES} map_static)
set_target_properties(run_test_02 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
