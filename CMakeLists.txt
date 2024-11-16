set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules" ${CMAKE_PREFIX_PATH})

cmake_minimum_required(VERSION 3.10)

project(phxargs C)
set(CMAKE_C_STANDARD 99)

set(SRC_DIR src)
set(BUILD_DIR build)
set(TEST_DIR tests)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BUILD_DIR})

set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Wextra -O0")

file(GLOB SOURCES "${SRC_DIR}/*.c")
include_directories(${SRC_DIR})

add_executable(phxargs ${SOURCES})
add_library(libphxargs STATIC ${SOURCES})

file(GLOB TEST_SOURCES "${TEST_DIR}/*.c")

find_package(Check REQUIRED)
