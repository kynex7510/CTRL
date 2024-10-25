#include "CTRL/Patch.h"
#include "CTRL/Memory.h"

#include <string.h>

Result ctrlPatch(const CTRLPatch* patch) {
    size_t handledSize = 0;

    while (handledSize < patch->size) {
        MemInfo info;
        const u32 curAddr = patch->addr + handledSize;
        const size_t dataLeft = patch->size - handledSize;

        Result ret = ctrlQueryRegion(curAddr, &info);
        if (R_FAILED(ret))
            return ret;

        // Calculate how much data to handle in this step.
        const size_t dataAvailable = (info.base_addr + info.size) - curAddr;
        const size_t dataToProcess = dataLeft < dataAvailable ? dataLeft : dataAvailable;

        ret = ctrlChangePermission(curAddr, dataToProcess, (info.perm | MEMPERM_WRITE));
        if (R_FAILED(ret))
            return ret;
        
        memcpy((void*)curAddr, (void*)(patch->data + handledSize), dataToProcess);

        ret = ctrlChangePermission(curAddr, dataToProcess, info.perm);
        if (R_FAILED(ret))
            return ret;

        size_t type = CTRL_DCACHE;
        if (info.perm & MEMPERM_EXECUTE)
            type |= CTRL_ICACHE;

        ret = ctrlFlushCache(curAddr, dataToProcess, type);
        if (R_FAILED(ret))
            return ret;

        handledSize += dataToProcess;
    }

    return 0;
}

Result ctrlPatchMulti(const CTRLPatch* patches, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        Result ret = ctrlPatch(&patches[i]);
        if (R_FAILED(ret))
            return ret;
    }

    return 0;
}