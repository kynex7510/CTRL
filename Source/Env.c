#include "CTRL/Env.h"

#define LUMA_TYPE 0x10000
#define CITRA_TYPE 0x20000

#define NOT_INIT (CTRLEnv)-1

#define LUMA_SYSTEM_VERSION(major, minor, revision) (((major) << 24) | ((minor) << 16) | ((revision) << 8))

static s32 g_Env = NOT_INIT;

CTRLEnv ctrlDetectEnv(void) {
    CTRLEnv env = (CTRLEnv)g_Env;

    if (env == NOT_INIT) {
        s64 check = 0;

        if (R_SUCCEEDED(svcGetSystemInfo(&check, LUMA_TYPE, 0)) && (check >= LUMA_SYSTEM_VERSION(8, 0, 0))) {
            // Detected luma 8.0.0+.
            env = Env_Luma;
        } else  if (R_SUCCEEDED(svcGetSystemInfo(&check, CITRA_TYPE, 0)) && (check == 1)) {
            // Detected citra.
            env = Env_Citra;
        } else {
            // Detected nothing.
            env = Env_Unknown;
        }

        do {
            __ldrex(&g_Env);
        } while (__strex(&g_Env, (s32)env));
    }

    return env;
}