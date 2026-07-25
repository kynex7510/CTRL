/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

#include <CTRL/Code.h>
#include <CTRL/RingAllocator.h>
#include <CTRL/Memory.h>
#include <CTRL/App.h>

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

    CTRLRingAllocator codeAllocator;
    codeAllocator.proc = CUR_PROCESS_HANDLE;

#if defined(CTRL_CFG_EXEC_BASE)
    codeAllocator.base = ctrlAddrToPageIndex(CTRL_CFG_EXEC_BASE);
#else
    codeAllocator.base = ctrlAddrToPageIndex(ctrlAppSectionInfo()->textAddr);
#endif

    codeAllocator.max = maxPageForCodeBase(codeAllocator.base);
    codeAllocator.offset = offset;

    const Result ret = ctrlRingAllocatorReservePages(&codeAllocator, numPages, outPageIndex);
    if (R_SUCCEEDED(ret))
        offset = codeAllocator.offset;

    return ret;
}