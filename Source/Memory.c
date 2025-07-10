/**
 * NOTES:
 * - svcControlMemory doesn't support the executable perm.
 * - svcControlProcessMemory doesn't support the pseudo handle.
 * - svcControlProcessMemory is more relaxed on checking addresses compared to svcControlMemory.
 * -- Citra only supports the latter, though it also doesn't enforce addresses checks.
 * -- It enforces permission checks, so MEMOP_PROT is a no-go; but it's "fine" as permissions arent really emulated.
 * - svcQueryMemory will never return whether the page is executable; svcQueryProcessMemory does.
 * -- Once again, citra doesn't have this limitation.
 * - Both luma and citra support invalidating the instruction cache.
 * - When mapping/unmapping svcControlProcessMemory expects a region of pages.
 * -- When unmapping, it's possible that aliased memory is made of multiple regions.
 */

#include <CTRL/Memory.h>
#include <CTRL/App.h>

#include <string.h> // memcpy

void ctrlFlushDataCache(void) {
    const CTRLEnv env = ctrlEnv();

    // Check for specialized syscall.
    if (env == Env_Luma || env == Env_Citra) {
        asm("svc 0x92");
        return;
    }

    // Use GSP to flush the entire data cache.
    Result ret = GSPGPU_FlushDataCache((void*)0x30000000, 0x10000000);
    if (R_FAILED(ret))
        svcBreak(USERBREAK_PANIC);
}
void ctrlInvalidateInstructionCache(void) {
    const CTRLEnv env = ctrlEnv();

    // Check for specialized syscall.
    if (env == Env_Luma || env == Env_Citra) {
        asm("svc 0x94");
        return;
    }

    // Use svcUnmapProcessMemory, which unconditionally invalidates the entire instruction cache.
    svcUnmapProcessMemory(0, 0, 0);
}

Result ctrlQueryMemory(u32 addr, MemInfo* memInfo, PageInfo* pageInfo) {
    MemInfo silly;
    PageInfo dummy;

    const Result ret = svcQueryProcessMemory(&silly, &dummy, CUR_PROCESS_HANDLE, addr);
    
    if (R_SUCCEEDED(ret)) {
        if (memInfo)
            memcpy(memInfo, &silly, sizeof(MemInfo));
            
        if (pageInfo)
            memcpy(pageInfo, &dummy, sizeof(PageInfo));
    }
    
    return ret;
}

Result ctrlQueryMemoryRegion(u32 addr, MemInfo* memInfo) {
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

Result ctrlChangeMemoryPerms(u32 addr, size_t size, MemPerm perms) {
    if (ctrlEnv() == Env_Citra)
        return 0;

    Handle proc;
    Result ret = svcDuplicateHandle(&proc, CUR_PROCESS_HANDLE);
    if (R_FAILED(ret))
        return ret;

    const u32 alignedAddr = ctrlAlignDown(addr, CTRL_PAGE_SIZE);
    const size_t alignedSize = ctrlAlignUp(size, CTRL_PAGE_SIZE);
    ret = svcControlProcessMemory(proc, alignedAddr, 0, alignedSize, MEMOP_PROT, perms);
    svcCloseHandle(proc);
    return ret;
}

Result ctrlMapAliasMemory(u32 addr, u32 alias, size_t size) {
    Result ret = 0;

    if (ctrlEnv() == Env_Citra) {
        u32 out;
        ret = svcControlMemory(&out, alias, addr, size, MEMOP_MAP, MEMPERM_READWRITE);
    } else {
        Handle proc;
        ret = svcDuplicateHandle(&proc, CUR_PROCESS_HANDLE);
        if (R_FAILED(ret))
            return ret;

        ret = svcControlProcessMemory(proc, alias, addr, size, MEMOP_MAP, MEMPERM_READWRITE);
        svcCloseHandle(proc);
    }

    return ret;
}

Result ctrlUnmapAliasMemory(u32 addr, u32 alias, size_t size) {
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
        ret = ctrlQueryMemoryRegion(alias + processedData, &memInfo);
        if (R_FAILED(ret))
            break;

        const size_t dataLeft = size - processedData;
        const size_t dataToProcess = memInfo.size < dataLeft ? memInfo.size : dataLeft;
        if (isCitra) {
            u32 dummy;
            ret = svcControlMemory(&dummy, alias + processedData, addr + processedData, dataToProcess, MEMOP_UNMAP, MEMPERM_READWRITE);
        } else {
            ret = svcControlProcessMemory(proc, alias + processedData, addr + processedData, dataToProcess, MEMOP_UNMAP, MEMPERM_READWRITE);
        }

        if (R_FAILED(ret))
            break;

        processedData += dataToProcess;
    }

    if (!isCitra)
        svcCloseHandle(proc);
    
    return ret;
}