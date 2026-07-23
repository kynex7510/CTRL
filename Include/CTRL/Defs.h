/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 * 
 * @file Defs.h
 * @brief Common definitions.
 */
#ifndef GUARD_CTRL_DEFS_H
#define GUARD_CTRL_DEFS_H

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

#endif /* GUARD_CTRL_DEFS_H */ 