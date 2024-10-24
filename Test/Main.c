#include "CTRL/Hook.h"

#include <stdlib.h>
#include <stdio.h>

#define RAND_EXPECTED_RET 1337

static int myRand(void) { return RAND_EXPECTED_RET; }

static bool test1() {
    printf("=== HOOK TEST ===\n");

    CTRLHook hook;
    hook.addr = (u32)rand;
    hook.callback = (u32)myRand;

    printf("Hooking rand()...\n");
    Result ret = ctrlHook(&hook);
    if (R_FAILED(ret)) {
        printf("FAILED: 0x%08lx\n", ret);
        return false;
    }

    int val = rand();
    printf("Expected: %u\n", RAND_EXPECTED_RET);
    printf("Got: %u\n", val);

    if (val != RAND_EXPECTED_RET) {
        printf("FAILED\n");
        return false;
    }

    printf("Removing hook...\n");
    ret = ctrlUnhook(&hook);
    if (R_FAILED(ret)) {
        printf("FAILED: 0x%08lx\n", ret);
        return false;
    }

    printf("Ret: %u\n", rand());
    printf("SUCCESS\n");
    return true;
}

int main(int argc, char* argv[]) {
    typedef bool(*Test_t)(void);
    Test_t tests[] = { test1 };

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
