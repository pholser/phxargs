set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules" ${CMAKE_PREFIX_PATH})

cmake_minimum_required(VERSION 3.10)

project(phxargs C)
set(CMAKE_C_STANDARD 99)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(SRC_DIR src)
set(BUILD_DIR build)
set(TEST_DIR tests)

set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Wextra -O0")

file(GLOB SOURCES "${SRC_DIR}/*.c")

include_directories(${SRC_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BUILD_DIR})

add_executable(phxargs ${SOURCES})

add_library(libphxargs STATIC ${SOURCES})

file(GLOB TEST_SOURCES "${TEST_DIR}/*.c")

find_package(Check REQUIRED)

foreach(test_source ${TEST_SOURCES})
  get_filename_component(test_name ${test_source} NAME_WE)
  add_executable(${test_name} ${test_source})
  target_include_directories(${test_name} PRIVATE ${Check_INCLUDE_DIRS})
  target_link_libraries(${test_name} libphxargs ${Check_LIBRARIES})
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

add_custom_target(test
  COMMAND ${CMAKE_CTEST_COMMAND}
  DEPENDS ${test_name}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(tidy
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/${BUILD_DIR}
)

# Custom target to run shell-based and C tests
add_custom_target(run_tests ALL
  COMMAND echo "Running shell-based tests..."
)

file(GLOB SHELL_TESTS "${CMAKE_SOURCE_DIR}/tests/test-*.sh")

foreach(script ${SHELL_TESTS})
  add_custom_command(TARGET run_tests
    COMMAND "${script}"
    COMMENT "Running shell test: ${script}"
  )
endforeach()

add_custom_command(TARGET run_tests
  COMMAND echo "Running C-based tests..."
  COMMAND ${CMAKE_CTEST_COMMAND}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
