#include <CTRL/CodeAllocator.h>
#include <CTRL/Memory.h>
#include <CTRL/App.h>

#define ERR_NO_MEM MAKERESULT(RL_FATAL, RS_OUTOFRESOURCE, RM_APPLICATION, RD_OUT_OF_MEMORY);

// Defined in AllocateHeaps.c.
extern u32 __ctrl_code_allocator_base;
extern u32 __ctrl_code_allocator_pages;

static u32 detectCodeRegionStart(void) {
    const CTRLAppSectionInfo* secInfo = ctrlAppSectionInfo();
    return secInfo->textAddr + secInfo->textSize + secInfo->rodataSize + secInfo->dataSize;
}

Result ctrlAllocCodePages(size_t numPages, u32* outAddr) {
    const size_t codeAllocatorSize = __ctrl_code_allocator_pages << 12;

    if (!__ctrl_code_allocator_base || !codeAllocatorSize)
        return ERR_NO_MEM;

    // Find free space in the region.
    MemInfo info;
    u32 base = __ctrl_code_allocator_base;

    while (base < (__ctrl_code_allocator_base +  codeAllocatorSize)) {
        Result ret = ctrlQueryRegion(base, &info);
        if (R_FAILED(ret))
            return ret;

        if (info.state == MEMSTATE_FREE && info.size >= (numPages << 12)) {
            u32 dummy;
            Result ret = svcControlMemory(&dummy, base, 0, numPages << 12, MEMOP_ALLOC, MEMPERM_READWRITE);

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
    return svcControlMemory(&dummy, allocAddr, 0, numPages << 12, MEMOP_FREE, 0);
}

Result ctrlCommitCodePages(u32 allocAddr, size_t numPages, u32* outCommitAddr) {
    const u32 commitAddr = allocAddr - __ctrl_code_allocator_base + detectCodeRegionStart();
    const size_t size = numPages << 12;

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
    return ctrlUnmirror(commitAddr, allocAddr, numPages << 12);
}