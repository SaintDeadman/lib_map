cmake_minimum_required(VERSION 3.10)
project(tests)


include_directories(${CMAKE_SOURCE_DIR}/map_lib)


file(GLOB TEST00_SOURCES 
    "00_simple_map.c"
    "common.c"
    )
add_executable(run_test_00 ${TEST00_SOURCES})

file(GLOB TEST01_SOURCES 
    "01_double_map.c" 
    "common.c"
    )
add_executable(run_test_01 ${TEST01_SOURCES})


find_path(CUNIT_INCLUDE_DIR
    NAMES CUnit/CUnit.h
    PATHS /usr/include /usr/local/include
)

find_library(CUNIT_LIBRARY
    NAMES cunit
    PATHS /usr/lib /usr/local/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CUnit DEFAULT_MSG CUNIT_INCLUDE_DIR CUNIT_LIBRARY)

if(CUNIT_FOUND)
    set(CUNIT_LIBRARIES ${CUNIT_LIBRARY})
    set(CUNIT_INCLUDE_DIRS ${CUNIT_INCLUDE_DIR})
else()
    set(CUNIT_LIBRARIES)
    set(CUNIT_INCLUDE_DIRS)
endif()

include_directories(${CUNIT_INCLUDE_DIR})

target_link_libraries(run_test_00 ${CUNIT_LIBRARIES} map_static pthread)
set_target_properties(run_test_00 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


target_link_libraries(run_test_01 ${CUNIT_LIBRARIES} map_static pthread)
set_target_properties(run_test_01 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)