#include <CTRL/CodeAllocator.h>
#include <CTRL/Memory.h>
#include <CTRL/App.h>

#define HEAP_SPLIT_SIZE_CAP (24 << 20)
#define LINEAR_HEAP_SIZE_CAP (32 << 20)

#define ERR_NO_MEM MAKERESULT(RL_STATUS, RS_OUTOFRESOURCE, RM_OS, 0x0A)

// Defined in libctru.
extern char* fake_heap_start;
extern char* fake_heap_end;

extern u32 __ctru_heap;
extern u32 __ctru_linear_heap;
extern u32 __ctru_heap_size;
extern u32 __ctru_linear_heap_size;

__attribute__((weak)) size_t __ctrl_code_allocator_pages = 0;

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

    //
    const bool stealHeapMemForCode = !__ctru_heap_size;
    //

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

    // Addition: steal memory for code region from application heap if not specified.
    if (stealHeapMemForCode) {
        const size_t codeAllocatorSize = ctrlNumPagesToSize(__ctrl_code_allocator_pages);
        if (__ctru_heap_size <= codeAllocatorSize)
            svcBreak(USERBREAK_PANIC);

        __ctru_heap_size -= codeAllocatorSize;
    }
    //

    rc = svcControlMemory(&__ctru_heap, OS_HEAP_AREA_BEGIN, 0x0, __ctru_heap_size, MEMOP_ALLOC, MEMPERM_READ | MEMPERM_WRITE);
    if (R_FAILED(rc))
        svcBreak(USERBREAK_PANIC);

    rc = svcControlMemory(&__ctru_linear_heap, 0x0, 0x0, __ctru_linear_heap_size, MEMOP_ALLOC_LINEAR, MEMPERM_READ | MEMPERM_WRITE);
    if (R_FAILED(rc))
        svcBreak(USERBREAK_PANIC);

    mappableInit(OS_MAP_AREA_BEGIN, OS_MAP_AREA_END);

    fake_heap_start = (char*)__ctru_heap;
    fake_heap_end = fake_heap_start + __ctru_heap_size;
}

static u32 detectCodeRegionStart(void) {
    const CTRLAppSectionInfo* secInfo = ctrlAppSectionInfo();
    return secInfo->textAddr + secInfo->textSize + secInfo->rodataSize + secInfo->dataSize;
}

Result ctrlAllocCodePages(size_t numPages, u32* outAddr) {
    const u32 codeAllocBegin = __ctru_heap + __ctru_heap_size;
    const u32 codeAllocEnd = codeAllocBegin + ctrlNumPagesToSize(__ctrl_code_allocator_pages);
    const size_t size = ctrlNumPagesToSize(numPages);

    if (!codeAllocBegin || codeAllocBegin == codeAllocEnd)
        return ERR_NO_MEM;

    // Find free space in the region.
    MemInfo info;
    u32 base = codeAllocBegin;

    while (base < codeAllocEnd) {
        Result ret = ctrlQueryRegion(base, &info);
        if (R_FAILED(ret))
            return ret;

        if (info.state == MEMSTATE_FREE && info.size >= size) {
            u32 dummy;
            Result ret = svcControlMemory(&dummy, base, 0, size, MEMOP_ALLOC, MEMPERM_READWRITE);

            if (R_FAILED(ret))
                return ret;

            *outAddr = base;
            return 0;
        }

        base = info.base_addr + info.size;
    }

    return ERR_NO_MEM;
}

Result ctrlFreeCodePages(u32 allocAddr, size_t numPages) {
    u32 dummy;
    return svcControlMemory(&dummy, allocAddr, 0, ctrlNumPagesToSize(numPages), MEMOP_FREE, 0);
}

Result ctrlCommitCodePages(u32 allocAddr, size_t numPages, u32* outCommitAddr) {
    const u32 codeAllocBase = __ctru_heap + __ctru_heap_size;
    const u32 commitAddr = allocAddr - codeAllocBase + detectCodeRegionStart();
    const size_t size = ctrlNumPagesToSize(numPages);

    // Map code.
    Result ret = ctrlMirror(commitAddr, allocAddr, size);
    if (R_FAILED(ret))
        return ret;

    // Make it executable.
    ret = ctrlChangePerms(commitAddr, size, MEMPERM_READEXECUTE);
    if (R_FAILED(ret)) {
        ctrlUnmirror(commitAddr, allocAddr, size);
        return ret;
    }

    // Handle cache.
    ctrlFlushDataCache();
    ctrlInvalidateInstructionCache();

    *outCommitAddr = commitAddr;
    return 0;
}

Result ctrlReleaseCodePages(u32 allocAddr, u32 commitAddr, size_t numPages) {
    return ctrlUnmirror(commitAddr, allocAddr, ctrlNumPagesToSize(numPages));
}