/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

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
#include <CTRL/Mappable.h>

#include "Syscalls.h"

#include <string.h> // memcpy

void ctrlFlushDataCache(void) { svcFlushEntireDataCache(); }
void ctrlInvalidateInstructionCache(void) { svcInvalidateEntireInstructionCache(); }

Result ctrlQueryMemory(Handle proc, u32 addr, MemInfo* memInfo, PageInfo* pageInfo) {
    MemInfo silly;
    PageInfo dummy;

    const Result ret = svcQueryProcessMemory(&silly, &dummy, proc, addr);
    
    if (R_SUCCEEDED(ret)) {
        if (memInfo)
            memcpy(memInfo, &silly, sizeof(MemInfo));
            
        if (pageInfo)
            memcpy(pageInfo, &dummy, sizeof(PageInfo));
    }
    
    return ret;
}

Result ctrlQueryMemoryRegion(Handle proc, u32 addr, MemInfo* memInfo) {
    Result ret = ctrlQueryMemory(proc, addr, memInfo, NULL);
    if (R_FAILED(ret))
        return ret;

    while (true) {
        MemInfo tmp;
        const u32 curAddr = memInfo->base_addr + memInfo->size;
        ret = ctrlQueryMemory(proc, curAddr, &tmp, NULL);
        if (R_FAILED(ret))
            return ret;

        if ((tmp.base_addr != curAddr) || (tmp.perm != memInfo->perm) || (tmp.state != memInfo->state))
            break;

        memInfo->size += tmp.size;
    }

    return 0;
}

Result ctrlChangeMemoryPerms(Handle proc, u32 addr, size_t size, MemPerm perms) {
    if (ctrlEnv() == Env_Citra)
        return ctrlIsThisProcess(proc) ? 0 : MAKERESULT(RL_PERMANENT, RS_NOTSUPPORTED, RM_OS, RD_NOT_IMPLEMENTED);

    bool needsClose = false;
    if (proc == CUR_PROCESS_HANDLE) {
        const Result ret = svcDuplicateHandle(&proc, CUR_PROCESS_HANDLE);
        if (R_FAILED(ret))
            return ret;

        needsClose = true;
    }

    const u32 alignedAddr = ctrlAlignDown(addr, CTRL_PAGE_SIZE);
    const size_t alignedSize = ctrlAlignUp(size, CTRL_PAGE_SIZE);
    const Result ret = svcControlProcessMemory(proc, alignedAddr, 0, alignedSize, MEMOP_PROT, perms);

    if (needsClose)
        svcCloseHandle(proc);
    
    return ret;
}

Result ctrlShareMemory(Handle srcProc, Handle dstProc, u32 srcAddr, u32 dstAddr, size_t size, MemPerm dstPerms) {
    Result ret = svcMapProcessMemoryEx(dstProc, dstAddr, srcProc, srcAddr, size, MAPEXFLAGS_PRIVATE);
    if (R_FAILED(ret))
        return ret;

    ret = ctrlChangeMemoryPerms(dstProc, dstAddr, size, dstPerms);
    if (R_FAILED(ret))
        ctrlUnshareMemory(dstProc, dstAddr, size);

    return ret;
}

Result ctrlUnshareMemory(Handle dstProc, u32 dstAddr, size_t size) {
    const Result ret = svcUnmapProcessMemoryEx(dstProc, dstAddr, size);
    if (R_FAILED(ret))
        return ret;

    // TODO:
    // Workaround a bug which doesn't restore the true memstate field.
    // This workaround is currently only available for the current process.
    if (ctrlIsThisProcess(dstProc)) {
        u32 dummy;
        return svcControlMemoryUnsafe(&dummy, dstAddr, size, MEMOP_FREE, 0);
    }

    return 0;
}

static Result mapProcPage(Handle proc, u32 otherAddr, u32* thisAddr) {
    Result ret = ctrlReserveMappableMemory(CTRL_PAGE_SIZE, thisAddr);
    if (R_FAILED(ret))
        return ret;

    ret = ctrlShareMemory(proc, CUR_PROCESS_HANDLE, otherAddr, *thisAddr, CTRL_PAGE_SIZE, MEMPERM_READWRITE);
    if (R_FAILED(ret))
        *thisAddr = 0;

    return ret;
}

static Result unmapProcPage(u32 thisAddr) {
    return ctrlUnshareMemory(CUR_PROCESS_HANDLE, thisAddr, CTRL_PAGE_SIZE);
}

static Result safeMemCpy(void* dst, const void* src, size_t size) {
    // Make sure the target address is writable.
    MemInfo info;
    Result ret = ctrlQueryMemoryRegion(CUR_PROCESS_HANDLE, (u32)dst, &info);
    if (R_FAILED(ret))
        return ret;
    
    if (!(info.perm & MEMPERM_WRITE)) {
        ret = ctrlChangeMemoryPerms(CUR_PROCESS_HANDLE, (u32)dst, size, (info.perm | MEMPERM_WRITE));
        if (R_FAILED(ret))
            return ret;
    }

    memcpy(dst, src, size);

    // Restore protection flags if necessary.
    if (!(info.perm & MEMPERM_WRITE)) {
        ret = ctrlChangeMemoryPerms(CUR_PROCESS_HANDLE, (u32)dst, size, info.perm);
        if (R_FAILED(ret))
            return ret;
    }

    return 0;
}

Result ctrlReadMemory(Handle proc, u32 addr, size_t size, void* buffer) {
    if (ctrlIsThisProcess(proc))
        return safeMemCpy(buffer, (const void*)addr, size);

    u8* out = (u8*)buffer;

    while (size) {
        const u32 alignedAddr = ctrlAlignDown(addr, CTRL_PAGE_SIZE);
        const size_t offset = addr - alignedAddr;
        const u32 maxAddr = alignedAddr + CTRL_PAGE_SIZE;
        const size_t maxSize = maxAddr - addr;
        const size_t chunkSize = size < maxSize ? size : maxSize;

        u32 thisAddr;
        Result ret = mapProcPage(proc, alignedAddr, &thisAddr);
        if (R_FAILED(ret))
            return ret;

        memcpy(out, (const void*)(thisAddr + offset), chunkSize);

        ret = unmapProcPage(thisAddr);
        if (R_FAILED(ret))
            return ret;

        out += chunkSize;
        addr += chunkSize;
        size -= chunkSize;
    }

    return 0;
}

Result ctrlWriteMemory(Handle proc, u32 addr, size_t size, const void* buffer) {
    if (ctrlIsThisProcess(proc))
        return safeMemCpy((void*)addr, buffer, size);

    const u8* in = (const u8*)buffer;

    while (size) {
        const u32 alignedAddr = ctrlAlignDown(addr, CTRL_PAGE_SIZE);
        const size_t offset = addr - alignedAddr;
        const u32 maxAddr = alignedAddr + CTRL_PAGE_SIZE;
        const size_t maxSize = maxAddr - addr;
        const size_t chunkSize = size < maxSize ? size : maxSize;

        u32 thisAddr;
        Result ret = mapProcPage(proc, alignedAddr, &thisAddr);
        if (R_FAILED(ret))
            return ret;

        memcpy((void*)(thisAddr + offset), in, chunkSize);

        ret = unmapProcPage(thisAddr);
        if (R_FAILED(ret))
            return ret;

        in += chunkSize;
        addr += chunkSize;
        size -= chunkSize;
    }

    return 0;
}

Result ctrlAliasMemory(u32 addr, u32 alias, size_t size) {
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

Result ctrlUnaliasMemory(u32 addr, u32 alias, size_t size) {
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
        ret = ctrlQueryMemoryRegion(proc, alias + processedData, &memInfo);
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