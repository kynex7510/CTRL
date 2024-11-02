cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(CTRL LANGUAGES C ASM)

file(GLOB CTRL_SOURCES Source/*.c Source/*.s)
add_library(${PROJECT_NAME} STATIC ${CTRL_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC Include)
target_compile_options(${PROJECT_NAME} PRIVATE -O3 -Wall -Wno-switch)

add_subdirectory(Tests)