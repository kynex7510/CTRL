/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

#include <CTRL/Hook.h>
#include <CTRL/Patch.h>
#include <CTRL/Memory.h>
#include <CTRL/Arch.h>

#include <string.h> // memcpy

static const u8 ARM_OP[4] = { 0x04, 0xF0, 0x1F, 0xE5 };   // LDR PC, [PC, #-4]
static const u8 THUMB_OP[4] = { 0x00, 0x4F, 0x38, 0x47 }; // LDR R7, [PC, #0]; BX R7

Result ctrlPlaceHook(Handle proc, CTRLHook* hook) {
    const u32 realAddr = ctrlClearThumb(hook->addr);
    const Result ret = ctrlReadMemory(proc, realAddr, CTRL_HOOK_SIZE, hook->ogBytes);
    if (R_FAILED(ret))
        return ret;

    u8 buffer[CTRL_HOOK_SIZE];
    memcpy(buffer, ctrlIsThumb(hook->addr) ? THUMB_OP : ARM_OP, 4);
    memcpy(&buffer[4], &hook->callback, sizeof(u32));
    
    return ctrlPatchMemory(proc, realAddr, buffer, CTRL_HOOK_SIZE);
}

Result ctrlRemoveHook(Handle proc, CTRLHook* hook) {
    return ctrlPatchMemory(proc, ctrlClearThumb(hook->addr), hook->ogBytes, CTRL_HOOK_SIZE);
}