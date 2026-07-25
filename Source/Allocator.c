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

static size_t g_HeapPageBase = 0;
static size_t g_HeapMaxPage = 0;

static Result setupHeapAllocator(void) {
    extern u32 __ctru_heap;
    extern u32 __ctru_heap_size;

    // Find first free page after application heap.
    u32 curAddr = __ctru_heap + __ctru_heap_size;
    u32 heapBase = 0;

    while (curAddr < OS_HEAP_AREA_END) {
        MemInfo memInfo;
        PageInfo pageInfo;
        const Result ret = svcQueryMemory(&memInfo, &pageInfo, curAddr);
        if (R_FAILED(ret))
            return ret;

        if (memInfo.base_addr >= OS_HEAP_AREA_BEGIN && memInfo.state == MEMSTATE_FREE) {
            heapBase = memInfo.base_addr;
            break;
        }

        curAddr = memInfo.base_addr + memInfo.size;
    }

    if (!heapBase)
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

    g_HeapPageBase = ctrlAddrToPageIndex(heapBase);
    g_HeapMaxPage = ctrlAddrToPageIndex(heapBase + heapSize);
    return 0;
}

__attribute((constructor)) void ctrlInitHeapAllocator(void) {
    if (R_FAILED(setupHeapAllocator()))
        svcBreak(USERBREAK_PANIC);
}

static Result findFreeRange(size_t numPages, size_t curIndex, size_t maxIndex, size_t* outPageIndex) {
    u32 curAddr = ctrlPageIndexToAddr(curIndex);

    while (curAddr < ctrlPageIndexToAddr(maxIndex)) {
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

    return ERR_NO_MEM;
}

Result ctrlReserveHeapPages(size_t numPages, size_t* outPageIndex) {
    static size_t offset = 0;

    Result ret = findFreeRange(numPages, g_HeapPageBase + offset, g_HeapMaxPage, outPageIndex);

    if (R_FAILED(ret))
        ret = findFreeRange(numPages, g_HeapPageBase, g_HeapPageBase + offset, outPageIndex);

    if (R_SUCCEEDED(ret))
        offset = (*outPageIndex + numPages) - g_HeapPageBase;

    return ret;
}

Result ctrlHeapAlloc(size_t pageIndex, size_t numPages) {
    u32 dummy;
    return svcControlMemory(&dummy, ctrlPageIndexToAddr(pageIndex), 0, ctrlNumPagesToSize(numPages), MEMOP_ALLOC, MEMPERM_READWRITE);
}

Result ctrlHeapFree(size_t pageIndex, size_t numPages) {
    u32 dummy;
    return svcControlMemory(&dummy, ctrlPageIndexToAddr(pageIndex), 0, ctrlNumPagesToSize(numPages), MEMOP_FREE, 0);
}

static size_t maxPageForCodeBase(size_t pageBase) {
    const u32 CODE_START = 0x100000;
    const u32 CODE_END = 0x4000000;
    const u32 SPECIAL_CODE_START = 0x14000000;
    const u32 SPECIAL_CODE_END = 0x1C000000;
    const u32 VA_END = 0x40000000;

    const u32 addr = ctrlPageIndexToAddr(pageBase);

#if defined(CTRL_CFG_EXEC_SIZE)
    u32 addrEnd = ctrlAlignUp(addr + CTRL_CFG_EXEC_SIZE, CTRL_PAGE_SIZE);
    if (addrEnd < addr)
        addrEnd = VA_END;

    // Check known ranges.
    if (addr >= CODE_START && addr <= CODE_END) {
        if (addrEnd > CODE_END)
            svcBreak(USERBREAK_PANIC);
    } else if (addr >= SPECIAL_CODE_START && addr <= SPECIAL_CODE_END) {
        if (addrEnd > SPECIAL_CODE_END)
            svcBreak(USERBREAK_PANIC);
    }

    return ctrlAddrToPageIndex(addrEnd);
#else
    if (addr >= CODE_START && addr <= CODE_END)
        return ctrlAddrToPageIndex(CODE_END);

    if (addr >= SPECIAL_CODE_START && addr <= SPECIAL_CODE_END)
        return ctrlAddrToPageIndex(SPECIAL_CODE_END);

    return ctrlAddrToPageIndex(VA_END);
#endif // CTRL_CFG_EXEC_SIZE
}

Result ctrlReserveExecutablePages(size_t numPages, size_t* outPageIndex) {
    static size_t offset = 0;

#if defined(CTRL_CFG_EXEC_BASE)
    const size_t pageBase = ctrlAddrToPageIndex(CTRL_CFG_EXEC_BASE);
#else
    const size_t pageBase = ctrlAddrToPageIndex(ctrlAppSectionInfo()->textAddr);
#endif // CTRL_CFG_EXEC_BASE

    Result ret = findFreeRange(numPages, pageBase + offset, maxPageForCodeBase(pageBase), outPageIndex);

    if (R_FAILED(ret))
        ret = findFreeRange(numPages, pageBase, pageBase + offset, outPageIndex);

    if (R_SUCCEEDED(ret))
        offset = (*outPageIndex + numPages) - pageBase;

    return ret;
}