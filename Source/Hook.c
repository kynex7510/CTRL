#include "CTRL/Hook.h"
#include "CTRL/Patch.h"
#include "CTRL/Memory.h"
#include "CTRL/Arch.h"

#include <string.h>

const u8 ARM_OP[4] = { 0x04, 0xF0, 0x1F, 0xE5 };   // LDR PC, [PC, #-4]
const u8 THUMB_OP[4] = { 0x00, 0x4F, 0x38, 0x47 }; // LDR R7, [PC, #0]; BX R7

Result ctrlPlaceHook(CTRLHook* hook) {
    u8 buffer[CTRL_HOOK_SIZE];
    const u32 realAddr = ctrlClearThumb(hook->addr);
    memcpy(buffer, ctrlIsThumb(hook->addr) ? THUMB_OP : ARM_OP, 4);
    memcpy(&buffer[4], &hook->callback, sizeof(u32));
    memcpy(hook->ogBytes, (void*)realAddr, CTRL_HOOK_SIZE);
    return ctrlPatchMemory(realAddr, buffer, CTRL_HOOK_SIZE);
}

Result ctrlRemoveHook(CTRLHook* hook) {
    return ctrlPatchMemory(ctrlClearThumb(hook->addr), hook->ogBytes, CTRL_HOOK_SIZE);
}