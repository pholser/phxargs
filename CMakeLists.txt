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

set(CMAKE_C_FLAGS_DEBUG -g -Wall -Wextra -O0)

function(get_system_include_paths out_var)
  message(STATUS "Using: ${CMAKE_C_COMPILER}")
  execute_process(
    COMMAND /bin/sh -c
            "${CMAKE_C_COMPILER} -E -x c -v /dev/null 2>&1 | awk '
              BEGIN { inside = 0; }
              /#include <...> search starts here:/ { inside = 1; next; }
              /End of search list./ { inside = 0; }
              inside { print $1; }'
            "
    OUTPUT_VARIABLE raw_paths
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if(raw_paths)
    message(STATUS "Raw system include paths:\n${raw_paths}")

    # Replace newlines with semicolons to create a CMake list
    string(REPLACE "\n" ";" raw_paths "${raw_paths}")
    message(STATUS "Split raw paths: ${raw_paths}")
    
    # Convert the raw paths into a list of -isystem arguments
    set(include_paths)
    foreach(path ${raw_paths})
      string(STRIP "${path}" path)  # Remove any leading/trailing whitespace
      if(NOT path STREQUAL "")
        message(STATUS "Adding include path: ${path}")
        list(APPEND include_paths -isystem "${path}")
      endif()
    endforeach()

    message(STATUS "Formatted system include paths: ${include_paths}")
    set(${out_var} "${include_paths}" PARENT_SCOPE)
  else()
    message(WARNING "Failed to extract system include paths from compiler output.")
    set(${out_var} "" PARENT_SCOPE)
  endif()
endfunction()

file(GLOB SOURCES "${SRC_DIR}/*.c")
include_directories(${SRC_DIR})

add_executable(phxargs ${SOURCES})
add_library(libphxargs STATIC ${SOURCES})

find_program(IWYU_PATH NAMES include-what-you-use iwyu)

if(IWYU_PATH)
  message(STATUS "IWYU found: ${IWYU_PATH}")
else()
  message(WARNING "IWYU not found. Install it to use this feature.")
endif()

if(IWYU_PATH)
  get_system_include_paths(SYSTEM_INCLUDE_PATHS)
  message(STATUS "System include paths passed to IWYU: ${SYSTEM_INCLUDE_PATHS}")

  add_custom_target(iwyu
    COMMAND ${CMAKE_COMMAND} -E echo "Running IWYU analysis..."
    COMMAND ${IWYU_PATH} -Xiwyu --verbose=3 --std=c99 ${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}
            ${SYSTEM_INCLUDE_PATHS} -I${SRC_DIR} ${SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running IWYU on source files..."
    VERBATIM
  )
  add_dependencies(iwyu phxargs libphxargs)
endif()


find_package(Check REQUIRED)
enable_testing()

add_custom_target(run_tests
  COMMAND ctest --output-on-failure
  DEPENDS phxargs libphxargs
)

add_custom_target(run_all
  DEPENDS iwyu run_tests
)

file(GLOB SHELL_TESTS "${TEST_DIR}/test-*.sh" "${TEST_DIR}/test-*.exp")
foreach(script ${SHELL_TESTS})
  get_filename_component(test_name ${script} NAME_WE)
  add_test(NAME ${test_name}
    COMMAND "${script}"
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
