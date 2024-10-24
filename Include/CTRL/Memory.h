#ifndef _CTRL_MEMORY_H
#define _CTRL_MEMORY_H

#include "CTRL/Types.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

Result ctrlFlushCache(u32 addr, size_t size);
Result ctrlFlushEntireCache(void);
Result ctrlQueryMemory(u32 addr, MemInfo* meminfo, PageInfo* pageinfo);
Result ctrlQueryRegion(u32 addr, MemInfo* memInfo);
Result ctrlChangePermission(u32 addr, size_t size, MemPerm perm);
Result ctrlMirror(u32 addr, u32 source, size_t size);
Result ctrlUnmirror(u32 addr, u32 source, size_t size);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* _CTRL_MEMORY_H */