/**
 * @file Arch.h
 * @brief Arch utilities.
 */
#ifndef _CTRL_ARCH_H
#define _CTRL_ARCH_H

#include <CTRL/Defs.h>

/**
 * @brief Check if an address references thumb code.
 * @param[in] addr Address.
 * @result True if the address references thumb code, false otherwise.
 */
CTRL_INLINE bool ctrlIsThumb(u32 addr) { return addr & 1; }

/**
 * @brief Set thumb mode bit in address.
 * @param[in] addr Address.
 * @result Address with thumb mode bit set.
 */
CTRL_INLINE u32 ctrlSetThumb(u32 addr) { return addr | 1; }

/**
 * @brief Clear thumb mode bit in address.
 * @param[in] addr Address.
 * @result Address with thumb mode bit clear.
 */
CTRL_INLINE u32 ctrlClearThumb(u32 addr) { return addr & ~(1u); }

#endif /* _CTRL_ARCH_H */