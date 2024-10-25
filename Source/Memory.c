/**
 * NOTES:
 * - svcControlMemory doesn't support the executable perm.
 * - svcControlProcessMemory doesn't support the pseudo handle.
 * - svcControlProcessMemory is more relaxed on checking addresses compared to svcControlMemory.
 * -- Citra only supports the latter, though it also doesn't enforce addresses checks.
 * -- It enforces permission checks, so MEMOP_PROT is a no-go; but it's "fine" as permissions arent really emulated.
 * - svcQueryMemory will never return whether the page is executable; svcQueryProcessMemory does.
 * -- Once again, citra doesn't have this limitation.
 * - HOS doesn't expose anything for cleaning the instruction cache.
 */

#include "CTRL/Memory.h"
#include "CTRL/Env.h"

#include <string.h>

#define DCACHE_THRESHOLD 0x700000

static CTRL_INLINE void ctrl_flushInsnCache(void) {
    switch (ctrlDetectEnv()) {
        case Env_Luma:
        case Env_Citra:
            asm("svc 0x94");
            return;
    }
}

Result ctrlFlushCache(u32 addr, size_t size, size_t type) {
    if (type & CTRL_ICACHE)
        ctrl_flushInsnCache();

    if (type & CTRL_DCACHE) {
        Result ret = svcFlushProcessDataCache(CUR_PROCESS_HANDLE, addr, size);
        if (R_FAILED(ret))
            return ret;
    }

    return 0;
}

Result ctrlFlushEntireCache(size_t type) { return ctrlFlushCache(1, DCACHE_THRESHOLD, type); }

Result ctrlQueryMemory(u32 addr, MemInfo* memInfo, PageInfo* pageInfo) {
    MemInfo silly;
    PageInfo dummy;

    Result ret = svcQueryProcessMemory(&silly, &dummy, CUR_PROCESS_HANDLE, addr);
    
    if (R_SUCCEEDED(ret)) {
        if (memInfo)
            memcpy(memInfo, &silly, sizeof(MemInfo));
            
        if (pageInfo)
            memcpy(pageInfo, &dummy, sizeof(PageInfo));
    }
    
    return ret;
}

Result ctrlQueryRegion(u32 addr, MemInfo* memInfo) {
    Result ret = ctrlQueryMemory(addr, memInfo, NULL);
    if (R_FAILED(ret))
        return ret;

    while (true) {
        MemInfo tmp;
        ret = ctrlQueryMemory(memInfo->base_addr + memInfo->size, &tmp, NULL);
        if (R_FAILED(ret))
            return ret;

        if ((tmp.perm != memInfo->perm) || (tmp.state != memInfo->state))
            break;

        memInfo->size += tmp.size;
    }

    return 0;
}

Result ctrlChangePermission(u32 addr, size_t size, MemPerm perm) {
    if (ctrlDetectEnv() == Env_Citra)
        return 0;

    Handle proc;
    Result ret = svcDuplicateHandle(&proc, CUR_PROCESS_HANDLE);
    if (R_FAILED(ret))
        return ret;

    const u32 alignedAddr = ctrlAlignAddr(addr, CTRL_PAGE_SIZE);
    const size_t alignedSize = ctrlAlignSize(size, CTRL_PAGE_SIZE);
    ret = svcControlProcessMemory(proc, alignedAddr, 0, alignedSize, MEMOP_PROT, perm);
    svcCloseHandle(proc);
    return ret;
}

static Result ctrl_mirrorAction(u32 addr, u32 source, size_t size, bool unmap) {
    Result ret = 0;
    const u32 alignedAddr = ctrlAlignAddr(addr, CTRL_PAGE_SIZE);
    const u32 alignedSrc = ctrlAlignAddr(source, CTRL_PAGE_SIZE);
    const size_t alignedSize = ctrlAlignSize(size, CTRL_PAGE_SIZE);

    if (ctrlDetectEnv() == Env_Citra) {
        u32 out;
        ret = svcControlMemory(&out, alignedAddr, alignedSrc, alignedSize, unmap ? MEMOP_UNMAP : MEMOP_MAP, MEMPERM_READWRITE);
    } else {
        Handle proc;
        ret = svcDuplicateHandle(&proc, CUR_PROCESS_HANDLE);
        if (R_FAILED(ret))
            return ret;

        ret = svcControlProcessMemory(proc, alignedAddr, alignedSrc, alignedSize, unmap ? MEMOP_UNMAP : MEMOP_MAP, MEMPERM_READWRITE);
        svcCloseHandle(proc);
    }

    return ret;
}

Result ctrlMirror(u32 addr, u32 source, size_t size) { return ctrl_mirrorAction(addr, source, size, false); }
Result ctrlUnmirror(u32 addr, u32 source, size_t size) { return ctrl_mirrorAction(addr, source, size, true); }