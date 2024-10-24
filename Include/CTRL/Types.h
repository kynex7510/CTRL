#ifndef _CTRL_TYPES_H
#define _CTRL_TYPES_H

#include <3ds.h>

#define CTRL_INLINE inline __attribute__((always_inline))

CTRL_INLINE size_t ctrlAlignAddr(u32 addr, size_t align) { return addr & ~(align - 1); }
CTRL_INLINE size_t ctrlAlignSize(size_t size, size_t align) { return (size + (align - 1)) & ~(align - 1); }

#endif /* _CTRL_TYPES_H */ 