/**
 * @file CodeGen.h
 * @brief Code generation utilities.
 */
#ifndef _CTRL_CODEGEN_H
#define _CTRL_CODEGEN_H

#include "CTRL/Types.h"

/// @brief Handle for a region of code pages.
typedef void* CTRLCodeRegion;

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/**
 * @brief Allocate space for a block of code inside a region.
 * @param[in, out] region Pointer to region handle.
 * @param[in] size Code block size.
 * @return Pointer to the allocated space, or NULL on failure.
 */
u8* ctrlAllocCodeBlock(CTRLCodeRegion* region, size_t size);

/**
 * @brief Commit a region of code.
 * @param[in, out] region Pointer to region handle.
 * @return Result code.
 */
Result ctrlCommitCodeRegion(CTRLCodeRegion* region);

/**
 * @brief Destroy a region of code and deallocate all related buffers.
 * @param[in, out] region Pointer to region handle.
 * @return Result code.
 */
Result ctrlDestroyCodeRegion(CTRLCodeRegion* region);

/**
 * @brief Get the first code block in a region.
 * @param[in] region Region handle.
 * @return Pointer to the allocated space, if not committed; address for the
 * code block, if committed; 0, if not available in either cases.
 */
u32 ctrlFirstCodeBlock(CTRLCodeRegion region);

/**
 * @brief Get the next block after the current one.
 * @param[in] codeBlock Current code block.
 * @return Pointer to the allocated space, if not committed; address for the
 * code block, if committed; 0, if not available in either cases.
 */
u32 ctrlNextCodeBlock(u32 codeBlock);

/**
 * @brief Get the number of code blocks in a region.
 * @param[in] region Region handle.
 * @return Number of code blocks.
 */
size_t ctrlNumCodeBlocks(CTRLCodeRegion region);

/**
 * @brief Get a code block from its index.
 * @param[in] region Region handle.
 * @param[in] index Code block index.
 * @return Pointer to the allocated space, if not committed; address for the
 * code block, if committed; 0, if not available in either cases.
 */
u32 ctrlGetCodeBlock(CTRLCodeRegion region, size_t index);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* _CTRL_CODEGEN_H */