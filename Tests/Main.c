#include "CTRL/Memory.h"
#include "CTRL/Hook.h"
#include "CTRL/Exception.h"
#include "CTRL/App.h"
#include "CTRL/CodeGen.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define RAND_EXPECTED_RET 1337
#define RAND_ACTUAL_RET 0

/* APP INFO TEST */

static bool appInfoTest(void) {
    printf("=== APP INFO TEST ===\n");

    const CTRLAppSectionInfo* info = ctrlAppSectionInfo();

    printf("Total size: 0x%08x\n", info->textSize + info->rodataSize + info->dataSize);
    printf("- .text: 0x%08x-0x%08x\n", info->textAddr, info->textAddr + info->textSize);
    printf("- .rodata: 0x%08x-0x%08x\n", info->rodataAddr, info->rodataAddr + info->rodataSize);
    printf("- .data: 0x%08x-0x%08x\n", info->dataAddr, info->dataAddr + info->dataSize);

    printf("SUCCESS\n");
    return true;
}

/* EX TEST */

static bool exHandler0(ERRF_ExceptionData* info) {
    printf("exHandler0(): This is called first (not handling)\n");
    return false;
}

static bool exHandler1(ERRF_ExceptionData* info) {
    printf("exHandler1(): This is called last\n");
    printf("Skipping instruction...\n");
    info->regs.pc += 4;
    return true;
}

static bool exHandler2(ERRF_ExceptionData* info) {
    printf("exHandler2(): This should not be called\n");
    svcBreak(USERBREAK_PANIC);
    return false;
}

static bool exTest(void) {
    printf("=== EXCEPTION HANDLER TEST ===\n");

    if (ctrlExceptionHandlingIsSupported()) {
        if (!ctrlSetExceptionHandler(exHandler2, CTRL_MAX_EX_HANDLERS - 1)) {
            printf("FAILED: could not set exception handler\n");
            return false;
        }

        if (!ctrlSetExceptionHandler(exHandler1, 1)) {
            printf("FAILED: could not set exception handler\n");
            return false;
        }

        if (!ctrlSetExceptionHandler(exHandler0, 0)) {
            printf("FAILED: could not set exception handler\n");
            return false;
        }
        
        asm("mov r0, #0");
        asm("ldr r0, [r0]");

        ctrlDisableExceptionHandling();
        printf("SUCCESS\n");
    } else {
        printf("SKIPPED (not supported)\n");
    }

    return true;
}

/* CODEGEN TEST */

static bool setupCodeBlock(CTRLCodeRegion* codeRegion, const u8* bytes, size_t size) {
    u8* codeBlockData = ctrlAllocCodeBlock(codeRegion, size);
    if (codeBlockData) {
        memcpy(codeBlockData, bytes, size);
        return true;
    }

    return false;
}

static bool codegenTest(void) {
    #define FUNC_PARAM_1 5
    #define FUNC_PARAM_2 3

    const u8 codeAdd[] = {
        0x08, 0x44, // ADD R0, R1
        0x70, 0x47, // BX LR
    };

    const u8 codeSub[] = {
        0x01, 0x00, 0x40, 0xE0, // SUB R0, R1
        0x1E, 0xFF, 0x2F, 0xE1, // BX LR
    };

    printf("=== CODEGEN TEST ===\n");

    CTRLCodeRegion codeRegion = NULL;

    // Allocate code blocks.
    if (!setupCodeBlock(&codeRegion, codeAdd, sizeof(codeAdd))) {
        printf("ADD ALLOC FAILED\n");
        return false;
    }

    if (!setupCodeBlock(&codeRegion, codeSub, sizeof(codeSub))) {
        printf("SUB ALLOC FAILED\n");
        return false;
    }

    // Commit code region.
    Result ret = ctrlCommitCodeRegion(&codeRegion);
    if (R_FAILED(ret)) {
        printf("MAP FAILED: 0x%08lx\n", ret);
        return false;
    }

    // Get address for the code blocks.
    const u32 addCodeAddr = ctrlGetCodeBlock(codeRegion, 0);
    if (!addCodeAddr) {
        printf("NO ADD CODE ADDR\n");
        return false;
    }

    const u32 subCodeAddr = ctrlGetCodeBlock(codeRegion, 1);
    if (!subCodeAddr) {
        printf("NO SUB CODE ADDR\n");
        return false;
    }

    // Now we're ready to call the functions.
    typedef int(*Fn_t)(int, int);
    Fn_t f = (Fn_t)(addCodeAddr | 1); // Set bit 0 for thumb code.
    printf("Doing addition\n");
    int val = f(FUNC_PARAM_1, FUNC_PARAM_2);

    printf("- Expected: %i\n", FUNC_PARAM_1 + FUNC_PARAM_2);
    printf("- Got: %i\n", val);

    if (val != (FUNC_PARAM_1 + FUNC_PARAM_2)) {
        printf("ADD FAILED\n");
        return false;
    }

    f = (Fn_t)subCodeAddr;
    printf("Doing subtraction\n");
    val = f(FUNC_PARAM_1, FUNC_PARAM_2);

    printf("- Expected: %i\n", FUNC_PARAM_1 - FUNC_PARAM_2);
    printf("- Got: %i\n", val);

    if (val != (FUNC_PARAM_1 - FUNC_PARAM_2)) {
        printf("SUB FAILED\n");
        return false;
    }

    // Destroy code region.
    ret = ctrlDestroyCodeRegion(&codeRegion);
    if (R_FAILED(ret)) {
        printf("DESTROY FAILED: 0x%08lx\n", ret);
        return false;
    }

    printf("SUCCESS\n");
    return true;
}

/* HOOK TEST */

static int myRand(void) { return RAND_EXPECTED_RET; }

static bool hookTest(void) {
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

/* Main */

int main(int argc, char* argv[]) {
    typedef bool(*Test_t)(void);
    Test_t tests[] = { appInfoTest, exTest, codegenTest, hookTest };

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
