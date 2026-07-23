/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

.global svcFlushEntireDataCache
.global svcInvalidateEntireInstructionCache
.global svcMapProcessMemoryEx
.global svcUnmapProcessMemoryEx

.section .text

.type svcFlushEntireDataCache, %function
svcFlushEntireDataCache:
    svc 0x92
    bx lr

.type svcInvalidateEntireInstructionCache, %function
svcInvalidateEntireInstructionCache:
    svc 0x94
    bx lr

.type svcMapProcessMemoryEx, %function
svcMapProcessMemoryEx:
    push {r4-r6, lr}
    ldr r4, [sp, #0x10]
    ldr r5, [sp, #0x14]
    mov r6, r0
    mov r0, #0xFFFFFFF2
    svc 0xA0
    pop {r4-r6, pc}

.type svcUnmapProcessMemoryEx, %function
svcUnmapProcessMemoryEx:
    svc 0xA1
    bx lr
