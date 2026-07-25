set(CTRL_SOURCES
    App.c
    Code.c
    CodeGen.c
    Exception.c
    ExHandler.s
    Heap.c
    Hook.c
    Memory.c
    Patch.c
    RingAllocator.c
    Syscalls.s
)

add_library(CTRL STATIC ${CTRL_SOURCES})
target_include_directories(CTRL PUBLIC ${CMAKE_SOURCE_DIR}/Include)
target_compile_options(CTRL PRIVATE -O2 -Wall -Wno-switch)

if (CTRL_CFG_EXEC_BASE)
    target_compile_definitions(CTRL PUBLIC -DCTRL_CFG_EXEC_BASE=${CTRL_CFG_EXEC_BASE})
endif()

if (CTRL_CFG_EXEC_SIZE)
    target_compile_definitions(CTRL PUBLIC -DCTRL_CFG_EXEC_SIZE=${CTRL_CFG_EXEC_SIZE})
endif()

install(TARGETS CTRL)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Include/CTRL DESTINATION include)