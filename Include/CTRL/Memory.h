/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 * 
 * @file Memory.h
 * @brief Memory utilities.
 */
#ifndef GUARD_CTRL_MEMORY_H
#define GUARD_CTRL_MEMORY_H

#include <CTRL/Defs.h>

#define CTRL_PAGE_SIZE 0x1000 ///< Page size.

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Convert address into page index.
 * @return Page index.
 */
CTRL_INLINE size_t ctrlAddrToPageIndex(u32 addr) { return ctrlAlignDown(addr, CTRL_PAGE_SIZE) >> 12; }

/**
 * @brief Convert page index into address.
 * @return Address.
 */
CTRL_INLINE u32 ctrlPageIndexToAddr(size_t pageIndex) { return pageIndex << 12; }

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
 * @brief Get informations about a page for the specified process.
 * @param[in] proc Target process.
 * @param[in] addr Address within a page.
 * @param[out] memInfo Memory info (can be NULL).
 * @param[out] pageInfo Page info (can be NULL).
 * @return Result code.
 */
Result ctrlQueryMemory(Handle proc, u32 addr, MemInfo* memInfo, PageInfo* pageInfo);

/**
 * @brief Get informations about a range of pages for the specified process, with
 * the same permission and state values.
 * @param[in] addr Address within the range.
 * @param[out] memInfo Range info.
 * @return Result code.
 */
Result ctrlQueryMemoryRegion(Handle proc, u32 addr, MemInfo* memInfo);

/**
 * @brief Change permissions on a range of pages for the specified process.
 * @param[in] proc Target process.
 * @param[in] addr Address.
 * @param[in] size Size.
 * @param[in] perms New permissions.
 * @return Result code.
 */
Result ctrlChangeMemoryPerms(Handle proc, u32 addr, size_t size, MemPerm perms);

/**
 * @brief Share memory between processes.
 * @param[in] srcProc Source process.
 * @param[in] dstProc Destination process.
 * @param[in] srcAddr Source process address.
 * @param[in] dstAddr Destination process address.
 * @param[in] size Size.
 * @param[in] dstPerms Target process page permissions.
 * @return Result code.
 */
Result ctrlShareMemory(Handle srcProc, Handle dstProc, u32 srcAddr, u32 dstAddr, size_t size, MemPerm dstPerms);

/**
 * @brief Restore shared memory.
 * @param[in] dstProc Destination process.
 * @param[in] dstAddr Destination process address.
 * @param[in] size Size.
 * @return Result code.
 */
Result ctrlUnshareMemory(Handle dstProc, u32 dstAddr, size_t size);

/**
 * @brief Read process memory.
 * @param[in] proc Target process.
 * @param[in] addr Address.
 * @param[in] size Size.
 * @param[out] buffer Output buffer.
 * @return Result code.
 */
Result ctrlReadMemory(Handle proc, u32 addr, size_t size, void* buffer);

/**
 * @brief Write process memory.
 * @param[in] proc Target process.
 * @param[in] addr Address.
 * @param[in] size Size.
 * @param[in] buffer Input buffer.
 * @return Result code.
 */
Result ctrlWriteMemory(Handle proc, u32 addr, size_t size, const void* buffer);

/**
 * @brief Remap a range of pages as read-write.
 * @param[in] sourcePageIndex Source page index.
 * @param[in] aliasPageIndex Alias page index.
 * @param[in] numPages Number of pages.
 * @return Result code.
 */
Result ctrlAliasPages(size_t sourcePageIndex, size_t aliasPageIndex, size_t numPages);

/**
 * @brief Restore aliased pages.
 * @param[in] sourcePageIndex Source page index.
 * @param[in] aliasPageIndex Alias page index.
 * @param[in] numPages Number of pages.
 * @return Result code.
 */
Result ctrlUnaliasPages(size_t sourcePageIndex, size_t aliasPageIndex, size_t numPages);

/**
 * @brief Remap pages as executable.
 * @param[in] sourcePageIndex Source page index.
 * @param[in] aliasPageIndex Alias page index.
 * @param[in] numPages Number of pages.
 * @return Result code.
 */
Result ctrlMapExecutablePages(size_t sourcePageIndex, size_t aliasPageIndex, size_t numPages);

/**
 * @brief Restore pages mapped as executable.
 * @param[in] sourcePageIndex Source page index.
 * @param[in] aliasPageIndex Alias page index.
 * @param[in] numPages Number of pages.
 * @return Result code.
 */
Result ctrlUnmapExecutablePages(size_t sourcePageIndex, size_t aliasPageIndex, size_t numPages);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* GUARD_CTRL_MEMORY_H */