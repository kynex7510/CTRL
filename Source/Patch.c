/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

#include <CTRL/Patch.h>
#include <CTRL/Memory.h>

Result ctrlApplyPatch(Handle proc, const CTRLPatch* patch) {
    bool clearCodeCache = false;
    size_t handledSize = 0;

    while (handledSize < patch->size) {
        MemInfo info;
        const u32 curAddr = patch->addr + handledSize;
        const size_t dataLeft = patch->size - handledSize;

        Result ret = ctrlQueryMemoryRegion(proc, curAddr, &info);
        if (R_FAILED(ret))
            return ret;

        // Calculate how much data to handle in this step.
        const size_t dataAvailable = (info.base_addr + info.size) - curAddr;
        const size_t dataToProcess = dataLeft < dataAvailable ? dataLeft : dataAvailable;
        
        // Copy the data.
        ctrlWriteMemory(proc, curAddr, dataToProcess, patch->data + handledSize);

        if (info.perm & MEMPERM_EXECUTE)
            clearCodeCache = true;

        handledSize += dataToProcess;
    }

    // Handle caches.
    ctrlFlushDataCache();

    if (clearCodeCache)
        ctrlInvalidateInstructionCache();

    return 0;
}