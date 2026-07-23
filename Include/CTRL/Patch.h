/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 * 
 * @file Patch.h
 * @brief Patching utilities.
 */
#ifndef _CTRL_PATCH_H
#define _CTRL_PATCH_H

#include <CTRL/Defs.h>

/// @brief Patch definition.
typedef struct {
    u32 addr;       ///< Patch address.
    const u8* data; ///< Patch data.
    size_t size;    ///< Patch size.
} CTRLPatch;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Apply a patch.
 * @param[in] patch Patch to apply.
 * @return Result code.
 */
Result ctrlApplyPatch(const CTRLPatch* patch);

/**
 * @brief Apply a memory patch.
 * @param[in] addr Address to patch.
 * @param[in] data Patch data.
 * @param[in] size Patch size.
 * @return Result code.
 */
CTRL_INLINE Result ctrlPatchMemory(u32 addr, const u8* data, size_t size) {
    CTRLPatch patch;
    patch.addr = addr;
    patch.data = data;
    patch.size = size;
    return ctrlApplyPatch(&patch);
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _CTRL_PATCH_H */