/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 * 
 * @file Allocator.h
 * @brief Allocator utilities.
 */
#ifndef GUARD_CTRL_ALLOCATOR_H
#define GUARD_CTRL_ALLOCATOR_H

#include <CTRL/Defs.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Reserve mappable pages.
 * @param[in] numPages Number of pages.
 * @param[out] outPageIndex Allocation page index.
 * @return Result code.
 */
Result ctrlReserveMappablePages(size_t numPages, size_t* outPageIndex);

/**
 * @brief Allocate pages in "mappable" memory as RW.
 * @param[in] pageIndex Page index.
 * @param[in] numPages Number of pages.
 * @return Result code.
 */
Result ctrlMappableAlloc(size_t pageIndex, size_t numPages);

/**
 * @brief Free allocated pages.
 * @param[in] pageIndex Page index.
 * @param[in] numPages Number of pages.
 * @return Result code.
 */
Result ctrlMappableFree(size_t pageIndex, size_t numPages);

/**
 * @brief Reserve executable pages.
 * @param[in] numPages Number of pages.
 * @param[out] outPageIndex Allocation page index.
 * @return Result code.
 */
Result ctrlReserveExecutablePages(size_t numPages, size_t* outPageIndex);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* GUARD_CTRL_ALLOCATOR_H */