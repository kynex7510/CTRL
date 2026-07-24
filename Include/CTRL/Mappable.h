/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 * 
 * @file Mappable.h
 * @brief Mappable memory utilities.
 */
#ifndef GUARD_CTRL_MAPPABLE_H
#define GUARD_CTRL_MAPPABLE_H

#include <CTRL/Defs.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Reserve address for mappable memory.
 * @param[in] size Size.
 * @param[out] outAddr Allocation address.
 * @return Result code.
 */
Result ctrlReserveMappableMemory(size_t size, u32* outAddr);

/**
 * @brief Allocate pages in "mappable" memory.
 * @param[in] addr Allocation address.
 * @param[in] numPages Number of pages to allocate.
 * @param[out] outAddr Allocation address.
 * @return Result code.
 */
Result ctrlMappableAlloc(u32 addr, size_t size);

/**
 * @brief Free allocated pages.
 * @param[in] addr Allocation address.
 * @param[in] numPages Number of pages to allocate.
 * @return Result code.
 */
Result ctrlMappableFree(u32 addr, size_t size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* GUARD_CTRL_MAPPABLE_H */