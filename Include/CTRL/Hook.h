#ifndef _CTRL_HOOK_H
#define _CTRL_HOOK_H

#include "CTRL/Types.h"

#define CTRL_HOOK_SIZE 8

typedef struct {
    u32 addr;
    u32 callback;
    u8 ogBytes[CTRL_HOOK_SIZE];
} CTRLHook;

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

Result ctrlHook(CTRLHook* hook);
Result ctrlUnhook(const CTRLHook* hook);
Result ctrlHookMulti(CTRLHook* hooks, size_t size);
Result ctrlUnhookMulti(const CTRLHook* hooks, size_t size);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* _CTRL_HOOK_H */