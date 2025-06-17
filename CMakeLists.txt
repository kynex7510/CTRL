cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(CTRL LANGUAGES C ASM)

option(CTRL_BUILD_TESTS "Enable tests compilation" OFF)

set(CTRL_SOURCES
    Source/AllocateHeaps.c
    Source/App.c
    Source/CodeAllocator.c
    Source/CodeGen.c
    Source/Exception.c
    Source/ExHandler.s
    Source/Hook.c
    Source/Memory.c
    Source/Patch.c
)

add_library(CTRL STATIC ${CTRL_SOURCES})
target_include_directories(CTRL PUBLIC Include)
target_compile_options(CTRL PRIVATE -O2 -Wall -Wno-switch)

install(TARGETS CTRL)
install(DIRECTORY Include/CTRL DESTINATION include)

if (CTRL_BUILD_TESTS)
    add_subdirectory(Tests)
endif()