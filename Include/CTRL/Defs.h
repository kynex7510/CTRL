/**
 * @file Defs.h
 * @brief Common definitions.
 */
#ifndef _CTRL_DEFS_H
#define _CTRL_DEFS_H

#include <3ds.h>

#ifdef __cplusplus
#define CTRL_INLINE inline
#define CTRL_EXTERN extern "C"
#else
#define CTRL_INLINE static inline
#define CTRL_EXTERN
#endif // __cplusplus

/**
 * @brief Align an address (align down).
 * @param[in] addr Address.
 * @param[in] align Alignment.
 * @return Aligned address.
 */
CTRL_INLINE u32 ctrlAlignAddr(u32 addr, size_t align) { return addr & ~(align - 1); }

/**
 * @brief Align size (align up).
 * @param[in] size Size.
 * @param[in] align Alignment.
 * @return Aligned size.
 */
CTRL_INLINE size_t ctrlAlignSize(size_t size, size_t align) { return (size + (align - 1)) & ~(align - 1); }

#endif /* _CTRL_DEFS_H */ 