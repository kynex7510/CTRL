#include "CTRL/Patch.h"
#include "CTRL/Memory.h"

#include <string.h>
#include <stdio.h>

Result ctrlPatch(const CTRLPatch* patch) {
    MemInfo info;
    Result ret = ctrlQueryMemory(patch->addr, &info, NULL);
    if (R_FAILED(ret))
        return ret;

    ret = ctrlChangePermission(patch->addr, patch->size, (info.perm | MEMPERM_WRITE));
    if (R_FAILED(ret))
        return ret;

    memcpy((void*)patch->addr, (void*)patch->data, patch->size);

    ret = ctrlChangePermission(patch->addr, patch->size, info.perm);
    if (R_SUCCEEDED(ret))
        ret = ctrlFlushCache(patch->addr, patch->size);

    return ret;
}

Result ctrlPatchMulti(const CTRLPatch* patches, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        Result ret = ctrlPatch(&patches[i]);
        if (R_FAILED(ret))
            return ret;
    }

    return 0;
}