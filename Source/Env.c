#include "CTRL/Env.h"

#define LUMA_TYPE 0x10000
#define CITRA_TYPE 0x20000

#define LUMA_SYSTEM_VERSION(major, minor, revision) (((major)<<24)|((minor)<<16)|((revision)<<8))

CTRLEnv ctrlDetectEnv(void) {
    s64 check = 0;

    // Detect luma.
    if (R_SUCCEEDED(svcGetSystemInfo(&check, LUMA_TYPE, 0)) && (check >= LUMA_SYSTEM_VERSION(8, 0, 0)))
        return Env_Luma;

    // Detect citra.
    if (R_SUCCEEDED(svcGetSystemInfo(&check, CITRA_TYPE, 0)) && (check == 1))
        return Env_Citra;

    return Env_Unknown;
}