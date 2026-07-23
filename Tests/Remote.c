/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

#include <3ds.h>

#include <CTRL/Memory.h>

#include <stdio.h>
#include <stdlib.h>

#define TARGET_PID 20 // GSP
#define TARGET_ADDR 0x100000 // Code region

// Reserve 1 page (4kb) of heap memory for remote transfers.
size_t __ctrl_code_allocator_pages = 1;

int main(int argc, char* argv[]) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    Handle gspProc;
    MemInfo memInfo;
    void* buffer = NULL;
    FILE* f = NULL;

    Result ret = svcOpenProcess(&gspProc, 20);
    if (R_FAILED(ret)) {
        printf("svcOpenProcess failed: 0x%08lX\n", ret);
        goto main_loop;
    }

    ret = ctrlQueryMemoryRegion(gspProc, TARGET_ADDR, &memInfo);
    if (R_FAILED(ret)) {
        printf("ctrlQueryMemoryRegion failed: 0x%08lX\n", ret);
        goto main_loop;
    }

    buffer = malloc(memInfo.size);
    if (!buffer) {
        printf("malloc failed\n");
        goto main_loop;
    }

    ret = ctrlReadMemory(gspProc, memInfo.base_addr, memInfo.size, buffer);
    if (R_FAILED(ret)) {
        printf("ctrlReadMemory failed: 0x%08X\n", ret);
        goto main_loop;
    }

    f = fopen("sdmc:/gsp_code.bin", "wb");
    if (!f) {
        printf("fopen failed\n");
        goto main_loop;
    }

    fwrite(buffer, memInfo.size, 1, f);
    printf("Memory dumped successfully!\n");

main_loop:
    fclose(f);
    free(buffer);
    svcCloseHandle(gspProc);

    printf("Press START to exit\n");

    while (aptMainLoop()) {
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break;

        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}