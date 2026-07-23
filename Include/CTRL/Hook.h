/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 * 
 * @file Hook.h
 * @brief Hooking utilities.
 */
#ifndef GUARD_CTRL_HOOK_H
#define GUARD_CTRL_HOOK_H

#include <CTRL/Defs.h>

#define CTRL_HOOK_SIZE 8 ///< Size for hook instructions.

/// @brief Hook definition.
typedef struct {
    u32 addr;                   ///< Target address.
    u32 callback;               ///< Hook callback.
    u8 ogBytes[CTRL_HOOK_SIZE]; ///< Original bytes (internal use only).
} CTRLHook;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Place hook on the specified process.
 * @param[in] proc Target process.
 * @param[in, out] hook Hook to place.
 * @return Result code.
 */
Result ctrlPlaceHook(Handle proc, CTRLHook* hook);

/**
 * @brief Remove hook from the specified process.
 * @param[in] proc Target process.
 * @param[in, out] hook Hook to remove.
 * @return Result code.
 */
Result ctrlRemoveHook(Handle proc, CTRLHook* hook);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* GUARD_CTRL_HOOK_H */