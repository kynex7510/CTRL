#ifndef _CTRL_APP_H
#define _CTRL_APP_H

#include "CTRL/Types.h"

typedef enum {
    Env_Luma,
    Env_Citra,
} CTRLEnv;

typedef struct {
    u32 textAddr;
    size_t textSize;
    u32 rodataAddr;
    size_t rodataSize;
    u32 dataAddr;
    size_t dataSize;
} CTRLAppSectionInfo;

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

CTRLEnv ctrlEnv(void);
const CTRLAppSectionInfo* ctrlAppSectionInfo(void);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // _CTRL_APP_H