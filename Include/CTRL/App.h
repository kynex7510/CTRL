/**
 * @file App.h
 * @brief Application utilities.
 */
#ifndef _CTRL_APP_H
#define _CTRL_APP_H

#include <CTRL/Defs.h>

/// @brief Environment values.
typedef enum {
    Env_Luma,   ///< Luma 8.0.0+
    Env_Citra,  ///< Citra (and derived forks)
} CTRLEnv;

/// @brief Application sections info.
typedef struct {
    u32 textAddr;      ///< Application .text address
    size_t textSize;   ///< Application .text size
    u32 rodataAddr;    ///< Application .rodata address
    size_t rodataSize; ///< Application .rodata size
    u32 dataAddr;      ///< Application .data address
    size_t dataSize;   ///< Application .data size
} CTRLAppSectionInfo;

/**
 * @brief Detect the environment in use.
 * @return Environment in use.
 */
CTRL_EXTERN CTRLEnv ctrlEnv(void);

/**
 * @brief Detect informations about app sections.
 * @return Pointer to app sections info.
 */
CTRL_EXTERN const CTRLAppSectionInfo* ctrlAppSectionInfo(void);

#endif // _CTRL_APP_H