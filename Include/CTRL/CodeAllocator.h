/**
 * @file CodeAllocator.h
 * @brief Code allocator utilities.
 */
#ifndef _CTRL_CODE_ALLOCATOR_H
#define _CTRL_CODE_ALLOCATOR_H

#include <CTRL/Defs.h>

/**
 * @brief Allocate pages to be used for code.
 * @param[in] numPages Number of pages to allocate.
 * @param[out] outAddr Allocation address.
 * @return Result code.
 */
CTRL_EXTERN Result ctrlAllocCodePages(size_t numPages, u32* outAddr);

/**
 * @brief Free allocated code pages.
 * @param[in] allocAddr Allocation address.
 * @param[in] numPages Number of pages to allocate.
 * @return Result code.
 */
CTRL_EXTERN Result ctrlFreeCodePages(u32 allocAddr, size_t numPages);

/**
 * @brief Commit allocated code pages.
 * @param[in] allocAddr Allocation address.
 * @param[in] numPages Number of pages to commit.
 * @param[out] outCommitAddr Commit address.
 * @return Result code.
 */
CTRL_EXTERN Result ctrlCommitCodePages(u32 allocAddr, size_t numPages, u32* outCommitAddr);

/**
 * @brief Release committed code pages.
 * @param[in] allocAddr Allocation address.
 * @param[in] commitAddr Commit address.
 * @param[in] numPages Number of pages to release.
 * @return Result code.
 */
CTRL_EXTERN Result ctrlReleaseCodePages(u32 allocAddr, u32 commitAddr, size_t numPages);

#endif /* _CTRL_CODE_ALLOCATOR_H */