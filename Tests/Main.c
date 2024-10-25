#include "CTRL/Memory.h"
#include "CTRL/Hook.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define RAND_EXPECTED_RET 1337
#define RAND_ACTUAL_RET 0

#define CODE_REGION_START 0x100000
#define CODE_REGION_SIZE 0x3F00000

static int myRand(void) { return RAND_EXPECTED_RET; }

static bool test1() {
    printf("=== HOOK TEST ===\n");

    CTRLHook hook;
    hook.addr = (u32)rand;
    hook.callback = (u32)myRand;

    // Hook code.
    Result ret = ctrlHook(&hook);
    if (R_FAILED(ret)) {
        printf("HOOK FAILED: 0x%08lx\n", ret);
        return false;
    }

    printf("Calling rand()...\n");
    int val = rand();
    printf("- Expected: %u\n", RAND_EXPECTED_RET);
    printf("- Got: %u\n", val);

    if (val != RAND_EXPECTED_RET) {
        printf("FAILED\n");
        return false;
    }

    // Unhook code.
    ret = ctrlUnhook(&hook);
    if (R_FAILED(ret)) {
        printf("UNHOOK FAILED: 0x%08lx\n", ret);
        return false;
    }

    srand(0);
    val = rand();
    printf("- Expected: %u\n", RAND_ACTUAL_RET);
    printf("- Got: %u\n", val);

    if (val != RAND_ACTUAL_RET) {
        printf("FAILED\n");
        return false;
    }

    printf("SUCCESS\n");
    return true;
}

static bool test2(void) {
    #define FUNC_PARAM_1 5
    #define FUNC_PARAM_2 3

    const u8 codeBytes[] = {
        0x08, 0x44, // ADD R0, R1
        0x70, 0x47, // BX LR
    };

    printf("=== CODEGEN TEST ===\n");

    // Allocate code buffer.
    void* mem = aligned_alloc(CTRL_PAGE_SIZE, sizeof(codeBytes));
    if (!mem) {
        printf("ALLOC FAILED\n");
        return false;
    }

    memcpy(mem, codeBytes, sizeof(codeBytes));

    // Find address in CODE region where to map the code.
    size_t queriedSize = 0;
    while (queriedSize < CODE_REGION_SIZE) {
        MemInfo info;
        Result ret = ctrlQueryRegion(CODE_REGION_START + queriedSize, &info);
        if (R_FAILED(ret)) {
            printf("QUERY FAILED: 0x%08lx\n", ret);
            return false;
        }

        if (info.state == MEMSTATE_FREE)
            break;

        queriedSize += info.size;
    }

    if (queriedSize >= CODE_REGION_SIZE) {
        printf("NO SPACE AVAILABLE\n");
        return false;
    }

    const u32 codeAddr = CODE_REGION_START + queriedSize;
    printf("Found address in CODE region: 0x%08lx\n", codeAddr);

    // Mirror code buffer to CODE region.
    Result ret = ctrlMirror(codeAddr, (u32)mem, sizeof(codeBytes));
    if (R_FAILED(ret)) {
        printf("MIRROR FAILED: 0x%08lx\n", ret);
        return false;
    }

    // Make it executable.
    ret = ctrlChangePermission(codeAddr, sizeof(codeBytes), MEMPERM_READEXECUTE);
    if (R_FAILED(ret)) {
        printf("PERM CHANGE FAILED: 0x%08lx\n", ret);
        return false;
    }

    // Flush instruction cache.
    ret = ctrlFlushCache(codeAddr, sizeof(codeBytes), CTRL_ICACHE);
    if (R_FAILED(ret)) {
        printf("CACHE FLUSH FAILED: 0x%08lx\n", ret);
        return false;
    }

    // Now we're ready to call the function.
    printf("Calling function...\n");
    typedef int(*Add_t)(int, int);
    Add_t f = (Add_t)(codeAddr | 1); // Set bit 0 for thumb.
    int val = f(5, 3);

    printf("- Expected: %u\n", FUNC_PARAM_1 + FUNC_PARAM_2);
    printf("- Got: %u\n", val);

    if (val != (FUNC_PARAM_1 + FUNC_PARAM_2)) {
        printf("FAILED\n");
        return false;
    }

    // Unmirror buffer from CODE region.
    ret = ctrlUnmirror(codeAddr, (u32)mem, sizeof(codeBytes));
    if (R_FAILED(ret)) {
        printf("UNMIRROR FAILED: 0x%08lx\n", ret);
        return false;
    }

    // Free code buffer.
    free(mem);

    printf("SUCCESS\n");
    return true;
}

int main(int argc, char* argv[]) {
    typedef bool(*Test_t)(void);
    Test_t tests[] = { test1, test2 };

    const size_t numTests = sizeof(tests) / sizeof(Test_t);

    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    bool error = false;
    for (size_t i = 0; i < numTests; ++i) {
        bool ret = tests[i]();

        printf("\n");

        if (!ret) {
            error = true;
            break;
        }

        gspWaitForVBlank();
        gfxSwapBuffers();
    }

    printf("===============\n");
    printf("Testing completed.\n");

    if (error) {
        printf("- Some tests have failed.\n");
    } else {
        printf("- All tests completed successfully.\n");
    }

    printf("Press START to exit.\n");

    while (aptMainLoop()) {
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break;

        gspWaitForVBlank();
        gfxSwapBuffers();
    }

    gfxExit();
    return 0;
}
