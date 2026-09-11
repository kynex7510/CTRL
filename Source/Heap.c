/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

#include <CTRL/Heap.h>
#include <CTRL/RingAllocator.h>
#include <CTRL/Memory.h>
#include <CTRL/App.h>

#include "Syscalls.h"

#define ERR_NO_MEM MAKERESULT(RL_STATUS, RS_OUTOFRESOURCE, RM_OS, 0x0A)

static CTRLRingAllocator g_HeapAllocator;

static Result setupHeapAllocator(void) {
    extern u32 __ctru_heap;
    extern u32 __ctru_heap_size;

    // Find first free page after application heap.
    u32 curAddr = __ctru_heap + __ctru_heap_size;

#if defined(CTRL_CFG_HEAP_OFFSET)
    u32 minAddr = OS_HEAP_AREA_BEGIN + CTRL_CFG_HEAP_OFFSET;

    if (minAddr < OS_HEAP_AREA_BEGIN)
        minAddr = OS_HEAP_AREA_BEGIN;

    if (curAddr < minAddr)
        curAddr = minAddr;
#endif // CTRL_CFG_HEAP_OFFSET

    u32 heapBase = 0;

    while (curAddr < OS_HEAP_AREA_END) {
        MemInfo memInfo;
        PageInfo pageInfo;
        const Result ret = svcQueryMemory(&memInfo, &pageInfo, curAddr);
        if (R_FAILED(ret))
            return ret;

        if (memInfo.state == MEMSTATE_FREE) {
            // Check if we have a free range within the heap area.
            if (memInfo.base_addr >= OS_HEAP_AREA_BEGIN) {
                heapBase = memInfo.base_addr;
                break;
            }

            // In some cases the heap area might not be initialized, thus we get a bigger range than expected.
            if (memInfo.base_addr <= OS_HEAP_AREA_BEGIN && (memInfo.base_addr + memInfo.size) > OS_HEAP_AREA_BEGIN) {
                heapBase = OS_HEAP_AREA_BEGIN;
                break;
            }
        }

        curAddr = memInfo.base_addr + memInfo.size;
    }

    if (!heapBase || heapBase > OS_HEAP_AREA_END)
        return ERR_NO_MEM;

    // Find consecutive pages.
    curAddr = heapBase;
    size_t heapSize = 0;
    const size_t maxHeapSize = OS_HEAP_AREA_END - heapBase;

    while (heapSize < maxHeapSize) {
        MemInfo memInfo;
        PageInfo pageInfo;
        const Result ret = svcQueryMemory(&memInfo, &pageInfo, curAddr);
        if (R_FAILED(ret))
            return ret;

        if (memInfo.state != MEMSTATE_FREE)
            break;

        heapSize += memInfo.size;
        if (heapSize > maxHeapSize)
            heapSize = maxHeapSize;

        curAddr = heapBase + heapSize;
    }

    if (!heapSize)
        return ERR_NO_MEM;

#if defined(CTRL_CFG_HEAP_SIZE)
    size_t wantedSize = ctrlAlignUp(CTRL_CFG_HEAP_SIZE, CTRL_PAGE_SIZE);

    if (wantedSize > (OS_HEAP_AREA_END - OS_HEAP_AREA_BEGIN))
        wantedSize = OS_HEAP_AREA_END - OS_HEAP_AREA_BEGIN;
    
    if (heapSize < wantedSize)
        return ERR_NO_MEM;

    if (heapSize > wantedSize)
        heapSize = wantedSize;
#endif // CTRL_CFG_HEAP_SIZE

    g_HeapAllocator.proc = CUR_PROCESS_HANDLE;
    g_HeapAllocator.base = ctrlAddrToPageIndex(heapBase);
    g_HeapAllocator.max = ctrlAddrToPageIndex(heapBase + heapSize);
    g_HeapAllocator.offset = 0;
    return 0;
}

__attribute((constructor)) void ctrlInitHeapAllocator(void) {
    if (R_FAILED(setupHeapAllocator()))
        svcBreak(USERBREAK_PANIC);
}

Result ctrlReserveHeapPages(size_t numPages, size_t* outPageIndex) {
    return ctrlRingAllocatorReservePages(&g_HeapAllocator, numPages, outPageIndex);
}

Result ctrlHeapAlloc(size_t pageIndex, size_t numPages) {
    u32 dummy;
    return svcControlMemory(&dummy, ctrlPageIndexToAddr(pageIndex), 0, ctrlNumPagesToSize(numPages), MEMOP_ALLOC, MEMPERM_READWRITE);
}

Result ctrlHeapFree(size_t pageIndex, size_t numPages) {
    u32 dummy;
    return svcControlMemory(&dummy, ctrlPageIndexToAddr(pageIndex), 0, ctrlNumPagesToSize(numPages), MEMOP_FREE, 0);
}