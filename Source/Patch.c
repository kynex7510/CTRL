#include <CTRL/Patch.h>
#include <CTRL/Memory.h>

#include <string.h> // memcpy

Result ctrlApplyPatch(const CTRLPatch* patch) {
    size_t handledSize = 0;

    while (handledSize < patch->size) {
        MemInfo info;
        const u32 curAddr = patch->addr + handledSize;
        const size_t dataLeft = patch->size - handledSize;

        Result ret = ctrlQueryMemoryRegion(curAddr, &info);
        if (R_FAILED(ret))
            return ret;

        // Calculate how much data to handle in this step.
        const size_t dataAvailable = (info.base_addr + info.size) - curAddr;
        const size_t dataToProcess = dataLeft < dataAvailable ? dataLeft : dataAvailable;

        // Make sure the target address is writable.
        if (!(info.perm & MEMPERM_WRITE)) {
            ret = ctrlChangeMemoryPerms(curAddr, dataToProcess, (info.perm | MEMPERM_WRITE));
            if (R_FAILED(ret))
                return ret;
        }
        
        // Copy the data.
        memcpy((void*)curAddr, (void*)(patch->data + handledSize), dataToProcess);

        // Restore protection flags if necessary.
        if (!(info.perm & MEMPERM_WRITE)) {
            ret = ctrlChangeMemoryPerms(curAddr, dataToProcess, info.perm);
            if (R_FAILED(ret))
                return ret;
        }

        // Handle caches.
        ctrlFlushDataCache();

        if (info.perm & MEMPERM_EXECUTE)
            ctrlInvalidateInstructionCache();

        handledSize += dataToProcess;
    }

    return 0;
}