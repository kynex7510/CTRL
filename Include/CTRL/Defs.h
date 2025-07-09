/**
 * @file Defs.h
 * @brief Common definitions.
 */
#ifndef _CTRL_DEFS_H
#define _CTRL_DEFS_H

#include <3ds.h>

#ifdef __cplusplus
#define CTRL_INLINE inline
#else
#define CTRL_INLINE static inline
#endif // __cplusplus

/**
 * @brief Align down.
 * @param[in] v Value.
 * @param[in] align Alignment.
 * @return Aligned down value.
 */
CTRL_INLINE size_t ctrlAlignDown(size_t v, size_t align) { return v & ~(align - 1); }

/**
 * @brief Align up.
 * @param[in] v Value.
 * @param[in] align Alignment.
 * @return Aligned up value.
 */
CTRL_INLINE size_t ctrlAlignUp(size_t v, size_t align) { return (v + (align - 1)) & ~(align - 1); }

#endif /* _CTRL_DEFS_H */ 