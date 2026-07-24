/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

#include <CTRL/Allocator.h>
#include <CTRL/Memory.h>
#include <CTRL/App.h>

#include "Syscalls.h"

#define ERR_NO_MEM MAKERESULT(RL_STATUS, RS_OUTOFRESOURCE, RM_OS, 0x0A)

Result ctrlReserveMappablePages(size_t numPages, size_t* outPageIndex) {
    *outPageIndex = ctrlAddrToPageIndex((u32)mappableAlloc(ctrlNumPagesToSize(numPages)));
    return *outPageIndex ? 0 : ERR_NO_MEM;
}

Result ctrlMappableAlloc(size_t pageIndex, size_t numPages) {
    u32 dummy;
    return svcControlMemory(&dummy, ctrlPageIndexToAddr(pageIndex), 0, ctrlNumPagesToSize(numPages), MEMOP_ALLOC, MEMPERM_READWRITE);
}

Result ctrlMappableFree(size_t pageIndex, size_t numPages) {
    // svcControlMemory doesn't accept MAP area addresses when free'ing, I suppose this is a kernel bug.
    u32 dummy;
    return svcControlMemoryUnsafe(&dummy, ctrlPageIndexToAddr(pageIndex), ctrlNumPagesToSize(numPages), MEMOP_FREE, 0);
}

static Result findFreeCodeRange(size_t numPages, size_t curIndex, size_t* outPageIndex) {
    u32 curAddr = ctrlPageIndexToAddr(curIndex);

    while (true) {
        MemInfo memInfo;
        Result ret = ctrlQueryMemoryRegion(CUR_PROCESS_HANDLE, curAddr, &memInfo);
        if (R_FAILED(ret))
            return ret;

        if (memInfo.state == MEMSTATE_FREE && ctrlSizeToNumPages(memInfo.size) >= numPages) {
            *outPageIndex = ctrlAddrToPageIndex(memInfo.base_addr);
            return 0;
        }

        curAddr = memInfo.base_addr + memInfo.size;
    }
}

Result ctrlReserveExecutablePages(size_t numPages, size_t* outPageIndex) {
    static size_t offset = 0;
    const size_t pageBase = ctrlAddrToPageIndex(ctrlAppSectionInfo()->textAddr);

    Result ret = findFreeCodeRange(numPages, pageBase + offset, outPageIndex);

    if (R_FAILED(ret))
        ret = findFreeCodeRange(numPages, pageBase, outPageIndex);

    if (R_SUCCEEDED(ret))
        offset = (*outPageIndex + numPages) - pageBase;

    return ret;
}