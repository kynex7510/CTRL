/**
 * @file Hook.h
 * @brief Hooking utilities.
 */
#ifndef _CTRL_HOOK_H
#define _CTRL_HOOK_H

#include <CTRL/Defs.h>

#define CTRL_HOOK_SIZE 8    ///< Size for hook instructions

/// @brief Hook definition.
typedef struct {
    u32 addr;                   ///< Target address.
    u32 callback;               ///< Hook callback.
    u8 ogBytes[CTRL_HOOK_SIZE]; ///< Original bytes, (internal use only).
} CTRLHook;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Place hook.
 * @param[in, out] hook Hook to place.
 * @return Result code.
 */
Result ctrlPlaceHook(CTRLHook* hook);

/**
 * @brief Remove hook.
 * @param[in, out] hook Hook to remove.
 * @return Result code.
 */
Result ctrlRemoveHook(CTRLHook* hook);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _CTRL_HOOK_H */