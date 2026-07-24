/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

#ifndef GUARD_CTRL_INTERNAL_SYSCALLS_H
#define GUARD_CTRL_INTERNAL_SYSCALLS_H

#include <3ds.h>

typedef enum MapExFlags {
    MAPEXFLAGS_PRIVATE = BIT(0),
} MapExFlags;

void svcFlushEntireDataCache(void);
void svcInvalidateEntireInstructionCache(void);
Result svcMapProcessMemoryEx(Handle dstProc, u32 vaDst, Handle srcProc, u32 vaSrc, u32 size, MapExFlags flags);
Result svcUnmapProcessMemoryEx(Handle proc, u32 addr, u32 size);
Result svcControlMemoryUnsafe(u32 *out, u32 addr0, u32 size, MemOp op, MemPerm perm);

#endif /* GUARD_CTRL_INTERNAL_SYSCALLS_H */