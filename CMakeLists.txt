set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules" ${CMAKE_PREFIX_PATH})

cmake_minimum_required(VERSION 3.10)

project(phxargs C)
set(CMAKE_C_STANDARD 99)

set(SRC_DIR src)
set(BUILD_DIR build)
set(TEST_DIR tests)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Wextra -O0")

file(GLOB SOURCES "${SRC_DIR}/*.c")
include_directories(${SRC_DIR})

add_executable(phxargs ${SOURCES})
add_library(libphxargs STATIC ${SOURCES})

find_package(Check REQUIRED)
enable_testing()

add_custom_target(run_tests
  COMMAND ctest --output-on-failure
  DEPENDS phxargs libphxargs
)

add_custom_target(run_all
  DEPENDS run_tests
)

file(GLOB SHELL_TESTS "${TEST_DIR}/test-*.sh")
foreach(script ${SHELL_TESTS})
  get_filename_component(test_name ${script} NAME_WE)
  add_test(NAME ${test_name}
    COMMAND /bin/bash "${script}"
  )
endforeach()

file(GLOB C_TEST_SOURCES "${TEST_DIR}/*.c")
foreach(test_source ${C_TEST_SOURCES})
  get_filename_component(test_name ${test_source} NAME_WE)
  message(STATUS "Adding test executable: ${test_name} from ${test_source}")
  add_executable(${test_name} ${test_source})
  set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  target_include_directories(${test_name} PRIVATE ${Check_INCLUDE_DIRS})
  target_link_libraries(${test_name} libphxargs ${Check_LIBRARIES})
  add_test(NAME ${test_name} COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${test_name})
  add_dependencies(run_tests ${test_name})
endforeach()

