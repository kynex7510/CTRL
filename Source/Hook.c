#include "CTRL/Hook.h"
#include "CTRL/Patch.h"
#include "CTRL/Memory.h"

#include <string.h>

#define IS_THUMB(addr) ((addr) & 1)
#define GET_REAL_ADDR(addr) ((addr) & ~(1u))

const u8 ARM_OP[4] = { 0x04, 0xF0, 0x1F, 0xE5 };   // LDR PC, [PC, #-4]
const u8 THUMB_OP[4] = { 0x00, 0x4F, 0x38, 0x47 }; // LDR R7, [PC, #0]; BX R7

Result ctrlPlaceHook(CTRLHook* hook) {
    u8 buffer[CTRL_HOOK_SIZE];
    memcpy(buffer, IS_THUMB(hook->addr) ? THUMB_OP : ARM_OP, 4);
    memcpy(&buffer[4], &hook->callback, sizeof(u32));
    memcpy(hook->ogBytes, (void*)GET_REAL_ADDR(hook->addr), CTRL_HOOK_SIZE);
    return ctrlPatchMemory(GET_REAL_ADDR(hook->addr), buffer, CTRL_HOOK_SIZE);
}

Result ctrlRemoveHook(CTRLHook* hook) {
    return ctrlPatchMemory(GET_REAL_ADDR(hook->addr), hook->ogBytes, CTRL_HOOK_SIZE);
}