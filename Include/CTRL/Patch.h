#ifndef _CTRL_PATCH_H
#define _CTRL_PATCH_H

#include "CTRL/Types.h"

typedef struct {
    u32 addr;
    const u8* data;
    size_t size;
} CTRLPatch;

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

Result ctrlPatch(const CTRLPatch* patch);
Result ctrlPatchMulti(const CTRLPatch* patches, size_t size);

CTRL_INLINE Result ctrlPatchMemory(u32 addr, const u8* data, size_t size) {
    CTRLPatch patch;
    patch.addr = addr;
    patch.data = data;
    patch.size = size;
    return ctrlPatch(&patch);
}

CTRL_INLINE Result ctrlPatch8(u32 addr, s8 val) { return ctrlPatchMemory(addr, (u8*)&val, sizeof(s8)); }
CTRL_INLINE Result ctrlPatch16(u32 addr, s16 val) { return ctrlPatchMemory(addr, (u8*)&val, sizeof(s16)); }
CTRL_INLINE Result ctrlPatch32(u32 addr, s32 val) { return ctrlPatchMemory(addr, (u8*)&val, sizeof(s32)); }
CTRL_INLINE Result ctrlPatch64(u32 addr, s64 val) { return ctrlPatchMemory(addr, (u8*)&val, sizeof(s64)); }
CTRL_INLINE Result ctrlPatchFloat(u32 addr, float val) { return ctrlPatchMemory(addr, (u8*)&val, sizeof(float)); }
CTRL_INLINE Result ctrlPatchDouble(u32 addr, double val) { return ctrlPatchMemory(addr, (u8*)&val, sizeof(double)); }

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* _CTRL_PATCH_H */