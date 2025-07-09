/**
 * @file Memory.h
 * @brief Memory utilities.
 */
#ifndef _CTRL_MEMORY_H
#define _CTRL_MEMORY_H

#include <CTRL/Defs.h>

#define CTRL_PAGE_SIZE 0x1000 ///< Page size

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Convert size in bytes into number of pages.
 * @return Number of pages.
 */
CTRL_INLINE size_t ctrlSizeToNumPages(size_t size) { return ctrlAlignUp(size, CTRL_PAGE_SIZE) >> 12; }

/**
 * @brief Convert number of pages into size in bytes.
 * @return Size in bytes.
 */
CTRL_INLINE size_t ctrlNumPagesToSize(size_t numPages) { return numPages << 12; }

/**
 * @brief Flush entire processor data cache.
 */
void ctrlFlushDataCache(void);

/**
 * @brief Invalidate entire processor instruction cache.
 */
void ctrlInvalidateInstructionCache(void);

/**
 * @brief Get informations about a page.
 * @param[in] addr Address within a page.
 * @param[out] memInfo Memory info (can be NULL).
 * @param[out] pageInfo Page info (can be NULL).
 * @return Result code.
 */
Result ctrlQueryMemory(u32 addr, MemInfo* memInfo, PageInfo* pageInfo);

/**
 * @brief Get informations about a range of pages, with same permission and state values.
 * @param[in] addr Address within the range.
 * @param[out] memInfo Range info.
 * @return Result code.
 */
Result ctrlQueryMemoryRegion(u32 addr, MemInfo* memInfo);

/**
 * @brief Change permissions on a range of pages.
 * @param[in] addr Address.
 * @param[in] size Size.
 * @param[in] perms New permissions.
 * @return Result code.
 */
Result ctrlChangeMemoryPerms(u32 addr, size_t size, MemPerm perms);

/**
 * @brief Remap a range of pages as read-write.
 * @param[in] addr Source address, must be page aligned.
 * @param[in] alias Alias address, must be page aligned.
 * @param[in] size Size, must be page aligned.
 * @return Result code.
 */
Result ctrlMapAliasMemory(u32 addr, u32 alias, size_t size);

/**
 * @brief Restore aliased memory.
 * @param[in] addr Source address, must be page aligned.
 * @param[in] alias Alias address, must be page aligned.
 * @param[in] size Size, must be page aligned.
 * @return Result code.
 */
Result ctrlUnmapAliasMemory(u32 addr, u32 alias, size_t size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _CTRL_MEMORY_H */