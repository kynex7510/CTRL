cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(CTRL-test C ASM)

add_executable(CTRL-test Main.c Crash.s)
target_link_libraries(CTRL-test PRIVATE CTRL)
ctr_create_3dsx(CTRL-test)

add_executable(CTRL-remote Remote.c)
target_link_libraries(CTRL-remote PRIVATE CTRL)
ctr_create_3dsx(CTRL-remote)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CTRL-test.3dsx TYPE BIN)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CTRL-remote.3dsx TYPE BIN)