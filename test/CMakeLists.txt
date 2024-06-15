cmake_minimum_required(VERSION 3.10)
project(tests)

include_directories(${CMAKE_SOURCE_DIR}/map_lib)
include_directories(${CMAKE_SOURCE_DIR}/map_lib/misc)

# Поиск библиотеки CUnit
find_path(CUNIT_INCLUDE_DIR NAMES CUnit/CUnit.h PATHS /usr/include /usr/local/include)
find_library(CUNIT_LIBRARY NAMES cunit PATHS /usr/lib /usr/local/lib)
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

# Добавляем подпапки для сборки
add_subdirectory(behavior)
add_subdirectory(messuring)