/**
 * @file Hook.h
 * @brief Hooking utilities.
 */
#ifndef _CTRL_HOOK_H
#define _CTRL_HOOK_H

#include <CTRL/Defs.h>

#define CTRL_HOOK_SIZE 8    ///< Size for hook instructions.

/// @brief Hook definition.
typedef struct {
    u32 addr;                   ///< Target address.
    u32 callback;               ///< Hook callback.
    u8 ogBytes[CTRL_HOOK_SIZE]; ///< Original bytes (internal use only).
} CTRLHook;

/**
 * @brief Place hook.
 * @param[in, out] hook Hook to place.
 * @return Result code.
 */
CTRL_EXTERN Result ctrlPlaceHook(CTRLHook* hook);

/**
 * @brief Remove hook.
 * @param[in, out] hook Hook to remove.
 * @return Result code.
 */
CTRL_EXTERN Result ctrlRemoveHook(CTRLHook* hook);

#endif /* _CTRL_HOOK_H */