/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

#include <CTRL/Mappable.h>
#include <CTRL/Memory.h>
#include <CTRL/App.h>

#define ERR_NO_MEM MAKERESULT(RL_STATUS, RS_OUTOFRESOURCE, RM_OS, 0x0A)

Result ctrlReserveMappableMemory(size_t size, u32* outAddr) {
    *outAddr = (u32)mappableAlloc(size);
    return *outAddr ? 0 : ERR_NO_MEM;
}

Result ctrlMappableAlloc(u32 addr, size_t numPages) {
    u32 dummy;
    return svcControlMemory(&dummy, addr, 0, ctrlNumPagesToSize(numPages), MEMOP_ALLOC, MEMPERM_READWRITE);
}

Result ctrlMappableFree(u32 addr, size_t numPages) {
    u32 dummy;
    return svcControlMemory(&dummy, addr, 0, ctrlNumPagesToSize(numPages), MEMOP_FREE, 0);
}