/**
 * @file Memory.h
 * @brief Memory utilities.
 */
#ifndef _CTRL_MEMORY_H
#define _CTRL_MEMORY_H

#include "CTRL/Types.h"

#define CTRL_PAGE_SIZE 0x1000   ///< Page size

#define CTRL_ICACHE 0x01    ///< Instruction cache flag
#define CTRL_DCACHE 0x02    ///< Data cache flag

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/**
 * @brief Flush processor cache.
 * @param[in] type Flush type (ICACHE, DCACHE or both).
 * @return Result code.
 */
Result ctrlFlushCache(size_t type);

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
Result ctrlQueryRegion(u32 addr, MemInfo* memInfo);

/**
 * @brief Change permissions on a range of pages.
 * @param[in] addr Address.
 * @param[in] size Size.
 * @param[in] perms New permissions.
 * @return Result code.
 */
Result ctrlChangePerms(u32 addr, size_t size, MemPerm perms);

/**
 * @brief Mirror a range of pages to a different address.
 * @param[in] addr Mirror address, must be page aligned.
 * @param[in] source Source address, must be page aligned.
 * @param[in] size Size, must be page aligned.
 * @return Result code.
 */
Result ctrlMirror(u32 addr, u32 source, size_t size);

/**
 * @brief Unmirror a range of pages.
 * @param[in] addr Mirror address, must be page aligned.
 * @param[in] source Source address, must be page aligned.
 * @param[in] size Size, must be page aligned.
 * @return Result code.
 */
Result ctrlUnmirror(u32 addr, u32 source, size_t size);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* _CTRL_MEMORY_H */