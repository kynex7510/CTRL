cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(CTRL-test)

file(GLOB CTRL_TEST_SOURCES *.c)
add_executable(CTRL-test ${CTRL_TEST_SOURCES})
target_link_libraries(CTRL-test PUBLIC CTRL)
ctr_create_3dsx(CTRL-test)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CTRL-test.3dsx TYPE BIN)