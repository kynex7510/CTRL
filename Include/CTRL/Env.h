#ifndef _CTRL_ENV_H
#define _CTRL_ENV_H

#include "CTRL/Types.h"

typedef enum {
    Env_Unknown,
    Env_Luma,
    Env_Citra,
} CTRLEnv;

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

CTRLEnv ctrlDetectEnv(void);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* _CTRL_ENV_H */