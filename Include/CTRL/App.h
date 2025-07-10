/**
 * @file App.h
 * @brief Application utilities.
 */
#ifndef _CTRL_APP_H
#define _CTRL_APP_H

#include <CTRL/Defs.h>

/// @brief Environment values.
typedef enum {
    Env_Luma,    ///< Luma 8.0.0+.
    Env_Citra,   ///< Citra (and derived forks).
    Env_Generic, ///< Generic environment.
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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Detect the environment in use.
 * @return Environment in use.
 */
CTRLEnv ctrlEnv(void);

/**
 * @brief Detect informations about app sections.
 * @return Pointer to app sections info.
 */
const CTRLAppSectionInfo* ctrlAppSectionInfo(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _CTRL_APP_H