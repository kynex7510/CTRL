#include <3ds.h>

#define HEAP_SPLIT_SIZE_CAP (24 << 20)
#define LINEAR_HEAP_SIZE_CAP (32 << 20)

extern char* fake_heap_start;
extern char* fake_heap_end;

extern u32 __ctru_heap;
extern u32 __ctru_linear_heap;
extern u32 __ctru_heap_size;
extern u32 __ctru_linear_heap_size;

u32 __ctrl_code_allocator_base = 0;
__attribute__((weak)) u32 __ctrl_code_allocator_pages = 0;

// https://github.com/devkitPro/libctru/blob/master/libctru/source/system/allocateHeaps.c
void __system_allocateHeaps(void) {
    Result rc;

    Handle reslimit = 0;
    rc = svcGetResourceLimit(&reslimit, CUR_PROCESS_HANDLE);
    if (R_FAILED(rc))
        svcBreak(USERBREAK_PANIC);

    s64 maxCommit = 0;
    s64 currentCommit = 0;
    ResourceLimitType reslimitType = RESLIMIT_COMMIT;
    svcGetResourceLimitLimitValues(&maxCommit, reslimit, &reslimitType, 1);
    svcGetResourceLimitCurrentValues(&currentCommit, reslimit, &reslimitType, 1);
    svcCloseHandle(reslimit);

    const u32 remaining = (u32)(maxCommit - currentCommit) &~ 0xFFF;
    if (__ctru_heap_size + __ctru_linear_heap_size > remaining)
        svcBreak(USERBREAK_PANIC);

    if (__ctru_heap_size == 0 && __ctru_linear_heap_size == 0) {
        __ctru_linear_heap_size = (remaining / 2) & ~0xFFF;
        __ctru_heap_size = remaining - __ctru_linear_heap_size;

        if (__ctru_heap_size > HEAP_SPLIT_SIZE_CAP) {
            __ctru_heap_size = HEAP_SPLIT_SIZE_CAP;
            __ctru_linear_heap_size = remaining - __ctru_heap_size;

            if (__ctru_linear_heap_size > LINEAR_HEAP_SIZE_CAP) {
                __ctru_linear_heap_size = LINEAR_HEAP_SIZE_CAP;
                __ctru_heap_size = remaining - __ctru_linear_heap_size;
            }
        }
    } else if (__ctru_heap_size == 0) {
        __ctru_heap_size = remaining - __ctru_linear_heap_size;
    } else if (__ctru_linear_heap_size == 0) {
        __ctru_linear_heap_size = remaining - __ctru_heap_size;
    }

    // Addition: steal memory for code region from application heap.
    const size_t codeAllocatorSize = __ctrl_code_allocator_pages << 12;
    if (__ctru_heap_size <= codeAllocatorSize)
        svcBreak(USERBREAK_PANIC);

    __ctru_heap_size -= codeAllocatorSize;
    //

    rc = svcControlMemory(&__ctru_heap, OS_HEAP_AREA_BEGIN, 0x0, __ctru_heap_size, MEMOP_ALLOC, MEMPERM_READ | MEMPERM_WRITE);
    if (R_FAILED(rc))
        svcBreak(USERBREAK_PANIC);

    //
    __ctrl_code_allocator_base = __ctru_heap + __ctru_heap_size;
    //

    rc = svcControlMemory(&__ctru_linear_heap, 0x0, 0x0, __ctru_linear_heap_size, MEMOP_ALLOC_LINEAR, MEMPERM_READ | MEMPERM_WRITE);
    if (R_FAILED(rc))
        svcBreak(USERBREAK_PANIC);

    mappableInit(OS_MAP_AREA_BEGIN, OS_MAP_AREA_END);

    fake_heap_start = (char*)__ctru_heap;
    fake_heap_end = fake_heap_start + __ctru_heap_size;
}