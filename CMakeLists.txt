cmake_minimum_required(VERSION 3.15...4.0.0)

project(LoxInterpreter)
set(CMAKE_CXX_STANDARD 17)
set(exec "lox.out")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

# not good practice, but whatever for the time being
file(
    GLOB all_srcs
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${exec} ${all_srcs})
