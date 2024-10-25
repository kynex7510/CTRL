cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(CTRL-test)

file(GLOB CTRL_TEST_SOURCES *.c)
add_executable(${PROJECT_NAME} ${CTRL_TEST_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC CTRL)
ctr_create_3dsx(${PROJECT_NAME})