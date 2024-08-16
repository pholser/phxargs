cmake_minimum_required(VERSION 3.10)

project(phxargs)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(phxargs ${SOURCES})

# Link libraries if any (e.g., pthread)
# target_link_libraries(phxargs pthread)
