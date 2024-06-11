cmake_minimum_required(VERSION 3.10)
project(tests)

file(GLOB TEST_SOURCES "*.c")

include_directories(${CMAKE_SOURCE_DIR}/map_lib)


add_executable(run_tests ${TEST_SOURCES})

find_path(CUNIT_INCLUDE_DIR
    NAMES CUnit/CUnit.h
    PATHS /usr/include /usr/local/include
)

find_library(CUNIT_LIBRARY
    NAMES cunit
    # PATHS /usr/lib /usr/local/lib
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

target_link_libraries(run_tests ${CUNIT_LIBRARIES} map_static pthread)


set_target_properties(run_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)