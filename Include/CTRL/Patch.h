/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 * 
 * @file Patch.h
 * @brief Patching utilities.
 */
#ifndef GUARD_CTRL_PATCH_H
#define GUARD_CTRL_PATCH_H

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
 * @brief Apply a patch to the specified process.
 * @param[in] proc Target process.
 * @param[in] patch Patch to apply.
 * @return Result code.
 * @note A different process should be targeted only when suspended.
 */
Result ctrlApplyPatch(Handle proc, const CTRLPatch* patch);

/**
 * @brief Apply a memory patch to the specified process.
 * @param[in] proc Target process.
 * @param[in] addr Address to patch.
 * @param[in] data Patch data.
 * @param[in] size Patch size.
 * @return Result code.
 * @note A different process should be targeted only when suspended.
 */
CTRL_INLINE Result ctrlPatchMemory(Handle proc, u32 addr, const u8* data, size_t size) {
    CTRLPatch patch;
    patch.addr = addr;
    patch.data = data;
    patch.size = size;
    return ctrlApplyPatch(proc, &patch);
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* GUARD_CTRL_PATCH_H */