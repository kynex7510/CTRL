/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 * 
 * @file Code.h
 * @brief Code utilities.
 */
#ifndef GUARD_CTRL_CODE_H
#define GUARD_CTRL_CODE_H

#include <CTRL/Defs.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Reserve code pages.
 * @param[in] numPages Number of pages.
 * @param[out] outPageIndex Allocation page index.
 * @return Result code.
 */
Result ctrlReserveCodePages(size_t numPages, size_t* outPageIndex);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* GUARD_CTRL_CODE_H */