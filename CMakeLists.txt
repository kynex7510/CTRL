cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(CTRL LANGUAGES C ASM)

file(GLOB CTRL_SOURCES Source/*.c Source/*.s)
add_library(CTRL STATIC ${CTRL_SOURCES})
target_include_directories(CTRL PUBLIC Include)
target_compile_options(CTRL PRIVATE -O2 -Wall -Wno-switch)

install(TARGETS CTRL)
install(DIRECTORY Include/CTRL DESTINATION include)

add_subdirectory(Tests)