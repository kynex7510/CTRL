#ifndef _CTRL_MEMORY_H
#define _CTRL_MEMORY_H

#include "CTRL/Types.h"

#define CTRL_PAGE_SIZE 0x1000

#define CTRL_ICACHE 0x01
#define CTRL_DCACHE 0x02

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

Result ctrlFlushCache(size_t type);
Result ctrlQueryMemory(u32 addr, MemInfo* meminfo, PageInfo* pageinfo);
Result ctrlQueryRegion(u32 addr, MemInfo* memInfo);
Result ctrlChangePerms(u32 addr, size_t size, MemPerm perms);
Result ctrlMirror(u32 addr, u32 source, size_t size);
Result ctrlUnmirror(u32 addr, u32 source, size_t size);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* _CTRL_MEMORY_H */