/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 * 
 * @file RingAllocator.h
 * @brief Ring allocator for processes.
 */
#ifndef GUARD_CTRL_RING_ALLOCATOR_H
#define GUARD_CTRL_RING_ALLOCATOR_H

#include <CTRL/Defs.h>

/// @brief Ring allocator object.
typedef struct {
    Handle proc;   ///< Target process.
    size_t base;   ///< Base page.
    size_t max;    ///< Max page (excluded).
    size_t offset; ///< Lookup offset.
} CTRLRingAllocator;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Initialize ring allocator.
 * @param[out] a Ring allocator object.
 * @param[in] proc Target process.
 * @param[in] base Starting base address.
 * @param[in] size Region size.
 */
void ctrlRingAllocatorInit(CTRLRingAllocator* a, Handle proc, u32 base, size_t size);

/**
 * @brief Reserve consecutive pages.
 * @param[in, out] a Ring allocator object.
 * @param[in] numPages Number of pages.
 * @param[out] outPageIndex Allocation page index.
 * @return Result code.
 */
Result ctrlRingAllocatorReservePages(CTRLRingAllocator* a, size_t numPages, size_t* outPageIndex);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* GUARD_CTRL_RING_ALLOCATOR_H */