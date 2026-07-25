cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(CTRL LANGUAGES C ASM)

option(CTRL_BUILD_TESTS "Enable tests compilation" OFF)

add_subdirectory(Source)

if (CTRL_BUILD_TESTS)
    add_subdirectory(Tests)
endif()