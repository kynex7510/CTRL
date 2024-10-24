#ifndef _CTRL_ENV_H
#define _CTRL_ENV_H

#include "CTRL/Types.h"

#define CTRL_PAGE_SIZE 0x1000

typedef enum {
    Env_Unknown,
    Env_Luma,
    Env_Citra,
} CTRLEnv;

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

CTRLEnv ctrlDetectEnv(void);

CTRL_INLINE bool ctrlIsEmu(CTRLEnv env) {
    switch (env) {
        case Env_Citra:
            return true;
        default:
            break;
    }

    return false;
}

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* _CTRL_ENV_H */