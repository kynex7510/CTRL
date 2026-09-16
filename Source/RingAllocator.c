/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

#include <CTRL/RingAllocator.h>
#include <CTRL/Memory.h>

#define ERR_NO_MEM MAKERESULT(RL_STATUS, RS_OUTOFRESOURCE, RM_OS, 0x0A)

static Result findFreeRange(Handle proc, size_t numPages, size_t baseIndex, size_t maxIndex, size_t* outPageIndex) {
    const u32 maxAddr = ctrlPageIndexToAddr(maxIndex);
    u32 curAddr = ctrlPageIndexToAddr(baseIndex);

    while (curAddr < maxAddr) {
        MemInfo memInfo;
        Result ret = ctrlQueryMemoryRegion(proc, curAddr, &memInfo);
        if (R_FAILED(ret))
            return ret;

        // Handle the case where the queried region goes out of bound.
        if (memInfo.base_addr < curAddr) {
            memInfo.size -= (curAddr - memInfo.base_addr);
            memInfo.base_addr = curAddr;
        }

        if (maxAddr - curAddr < memInfo.size)
            memInfo.size = maxAddr - curAddr;

        if (memInfo.state == MEMSTATE_FREE && ctrlSizeToNumPages(memInfo.size) >= numPages) {
            *outPageIndex = ctrlAddrToPageIndex(memInfo.base_addr);
            return 0;
        }

        curAddr = memInfo.base_addr + memInfo.size;
    }

    return ERR_NO_MEM;
}

void ctrlRingAllocatorInit(CTRLRingAllocator* a, Handle proc, u32 base, size_t size) {
    a->proc = proc;
    a->base = ctrlAddrToPageIndex(base);
    a->max = ctrlAddrToPageIndex(ctrlAlignUp(base + size, CTRL_PAGE_SIZE));
    a->offset = 0;
}

Result ctrlRingAllocatorReservePages(CTRLRingAllocator* a, size_t numPages, size_t* outPageIndex) {
     Result ret = findFreeRange(a->proc, numPages, a->base + a->offset, a->max, outPageIndex);

    if (R_FAILED(ret))
        ret = findFreeRange(a->proc, numPages, a->base, a->base + a->offset, outPageIndex);

    if (R_SUCCEEDED(ret))
        a->offset = (*outPageIndex + numPages) - a->base;

    return ret;
}