/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 * 
 * @file CodeAllocator.h
 * @brief Code allocator utilities.
 */
#ifndef GUARD_CTRL_CODE_ALLOCATOR_H
#define GUARD_CTRL_CODE_ALLOCATOR_H

#include <CTRL/Defs.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Get the next address used for mapping code pages.
 * @note This value might be different across different calls.
 * @param[in] numPages Number of pages to allocate.
 * @param[out] outAddr Possible allocation address.
 * @return Result code. 
 */
Result ctrlNextCodeAllocAddress(size_t numPages, u32* outAddr);

/**
 * @brief Allocate pages to be used for code.
 * @param[in] numPages Number of pages to allocate.
 * @param[out] outAddr Allocation address.
 * @return Result code.
 */
Result ctrlAllocCodePages(size_t numPages, u32* outAddr);

/**
 * @brief Free allocated code pages.
 * @param[in] allocAddr Allocation address.
 * @param[in] numPages Number of pages to allocate.
 * @return Result code.
 */
Result ctrlFreeCodePages(u32 allocAddr, size_t numPages);

/**
 * @brief Commit allocated code pages.
 * @param[in] allocAddr Allocation address.
 * @param[in] numPages Number of pages to commit.
 * @param[out] outCommitAddr Commit address.
 * @return Result code.
 */
Result ctrlCommitCodePages(u32 allocAddr, size_t numPages, u32* outCommitAddr);

/**
 * @brief Release committed code pages.
 * @param[in] allocAddr Allocation address.
 * @param[in] commitAddr Commit address.
 * @param[in] numPages Number of pages to release.
 * @return Result code.
 */
Result ctrlReleaseCodePages(u32 allocAddr, u32 commitAddr, size_t numPages);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* GUARD_CTRL_CODE_ALLOCATOR_H */