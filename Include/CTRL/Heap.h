/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 * 
 * @file Heap.h
 * @brief Heap utilities.
 */
#ifndef GUARD_CTRL_HEAP_H
#define GUARD_CTRL_HEAP_H

#include <CTRL/Defs.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Reserve heap pages.
 * @param[in] numPages Number of pages.
 * @param[out] outPageIndex Allocation page index.
 * @return Result code.
 */
Result ctrlReserveHeapPages(size_t numPages, size_t* outPageIndex);

/**
 * @brief Allocate pages in heap memory as RW.
 * @param[in] pageIndex Page index.
 * @param[in] numPages Number of pages.
 * @return Result code.
 */
Result ctrlHeapAlloc(size_t pageIndex, size_t numPages);

/**
 * @brief Free heap pages.
 * @param[in] pageIndex Page index.
 * @param[in] numPages Number of pages.
 * @return Result code.
 */
Result ctrlHeapFree(size_t pageIndex, size_t numPages);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* GUARD_CTRL_HEAP_H */