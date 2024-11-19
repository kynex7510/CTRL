/**
 * NOTES:
 * - svcControlMemory doesn't support the executable perm.
 * - svcControlProcessMemory doesn't support the pseudo handle.
 * - svcControlProcessMemory is more relaxed on checking addresses compared to svcControlMemory.
 * -- Citra only supports the latter, though it also doesn't enforce addresses checks.
 * -- It enforces permission checks, so MEMOP_PROT is a no-go; but it's "fine" as permissions arent really emulated.
 * - svcQueryMemory will never return whether the page is executable; svcQueryProcessMemory does.
 * -- Once again, citra doesn't have this limitation.
 * - Both luma and citra support cleaning the instruction cache.
 * - When mapping/unmapping svcControlProcessMemory expects a region of pages.
 * -- When unmapping, it's possible that the mirrored memory is made of multiple regions.
 */

#include "CTRL/Memory.h"
#include "CTRL/App.h"

#include <string.h>

#define DCACHE_THRESHOLD 0x700000

static CTRL_INLINE void ctrl_flushInsnCache(void) {
    switch (ctrlEnv()) {
        case Env_Luma:
        case Env_Citra:
            asm("svc 0x94");
            return;
    }
}

Result ctrlFlushCache(size_t type) {
    if (type & CTRL_ICACHE)
        ctrl_flushInsnCache();

    if (type & CTRL_DCACHE) {
        Result ret = svcFlushProcessDataCache(CUR_PROCESS_HANDLE, 1, DCACHE_THRESHOLD);
        if (R_FAILED(ret))
            return ret;
    }

    return 0;
}

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

Result ctrlChangePerms(u32 addr, size_t size, MemPerm perms) {
    if (ctrlEnv() == Env_Citra)
        return 0;

    Handle proc;
    Result ret = svcDuplicateHandle(&proc, CUR_PROCESS_HANDLE);
    if (R_FAILED(ret))
        return ret;

    const u32 alignedAddr = ctrlAlignAddr(addr, CTRL_PAGE_SIZE);
    const size_t alignedSize = ctrlAlignSize(size, CTRL_PAGE_SIZE);
    ret = svcControlProcessMemory(proc, alignedAddr, 0, alignedSize, MEMOP_PROT, perms);
    svcCloseHandle(proc);
    return ret;
}

Result ctrlMirror(u32 addr, u32 source, size_t size) {
    Result ret = 0;

    if (ctrlEnv() == Env_Citra) {
        u32 out;
        ret = svcControlMemory(&out, addr, source, size, MEMOP_MAP, MEMPERM_READWRITE);
    } else {
        Handle proc;
        ret = svcDuplicateHandle(&proc, CUR_PROCESS_HANDLE);
        if (R_FAILED(ret))
            return ret;

        ret = svcControlProcessMemory(proc, addr, source, size, MEMOP_MAP, MEMPERM_READWRITE);
        svcCloseHandle(proc);
    }

    return ret;
}

Result ctrlUnmirror(u32 addr, u32 source, size_t size) {
    Result ret = 0;
    bool isCitra = (ctrlEnv() == Env_Citra);
    Handle proc = CUR_PROCESS_HANDLE;

    if (!isCitra) {
        ret = svcDuplicateHandle(&proc, CUR_PROCESS_HANDLE);
        if (R_FAILED(ret))
            return ret;
    }

    size_t processedData = 0;
    while (processedData < size) {
        MemInfo memInfo;
        ret = ctrlQueryRegion(addr + processedData, &memInfo);
        if (R_FAILED(ret))
            break;

        const size_t dataLeft = size - processedData;
        const size_t dataToProcess = memInfo.size < dataLeft ? memInfo.size : dataLeft;
        if (isCitra) {
            u32 dummy;
            ret = svcControlMemory(&dummy, addr + processedData, source + processedData, dataToProcess, MEMOP_UNMAP, MEMPERM_READWRITE);
        } else {
            ret = svcControlProcessMemory(proc, addr + processedData, source + processedData, dataToProcess, MEMOP_UNMAP, MEMPERM_READWRITE);
        }

        if (R_FAILED(ret))
            break;

        processedData += dataToProcess;
    }

    if (!isCitra)
        svcCloseHandle(proc);
    
    return ret;
}