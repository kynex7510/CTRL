#include "CTRL/App.h"
#include "CTRL/Memory.h"

#include <string.h>

#define LUMA_TYPE 0x10000
#define CITRA_TYPE 0x20000

#define TEXT_ADDR_TYPE 0x10005
#define TEXT_SIZE_TYPE 0x10002
#define RODATA_ADDR_TYPE 0x10006
#define RODATA_SIZE_TYPE 0x10003
#define DATA_ADDR_TYPE 0x10007
#define DATA_SIZE_TYPE 0x10004

#define LUMA_SYSTEM_VERSION(major, minor, revision) (((major) << 24) | ((minor) << 16) | ((revision) << 8))

static CTRLEnv g_Env;
static CTRLAppSectionInfo g_AppSectionInfo;

static CTRL_INLINE bool ctrl_detectEnv(CTRLEnv* env) {
    s64 check = 0;

    // Detected luma 8.0.0+.
    if (R_SUCCEEDED(svcGetSystemInfo(&check, LUMA_TYPE, 0)) && (check >= LUMA_SYSTEM_VERSION(8, 0, 0))) {
        *env = Env_Luma;
        return true;
    }
    
    // Detected citra.
    if (R_SUCCEEDED(svcGetSystemInfo(&check, CITRA_TYPE, 0)) && (check == 1)) {
        *env = Env_Citra;
        return true;
    }

    return false;
}

static CTRL_INLINE Result ctrl_getAppSectionInfo(CTRLAppSectionInfo* out) {
    s64 tmp;
    Result ret;

#define READ_INFO(type, outMember) \
    ret = svcGetProcessInfo(&tmp, CUR_PROCESS_HANDLE, ((type))); \
    if (R_FAILED(ret)) \
        return ret; \
    \
    outMember = tmp;

    READ_INFO(TEXT_ADDR_TYPE, out->textAddr);
    READ_INFO(TEXT_SIZE_TYPE, out->textSize);
    READ_INFO(RODATA_ADDR_TYPE, out->rodataAddr);
    READ_INFO(RODATA_SIZE_TYPE, out->rodataSize);
    READ_INFO(DATA_ADDR_TYPE, out->dataAddr);
    READ_INFO(DATA_SIZE_TYPE, out->dataSize);

#undef READ_INFO
    return 0;
}

static __attribute((constructor)) void ctrl_initEnv(void) {
    if (!ctrl_detectEnv(&g_Env))
        svcBreak(USERBREAK_PANIC);
}

static __attribute((constructor)) void ctrl_initAppSectionInfo(void) {
    if (R_FAILED(ctrl_getAppSectionInfo(&g_AppSectionInfo)))
        svcBreak(USERBREAK_PANIC);
}

CTRLEnv ctrlEnv(void) { return g_Env; }
const CTRLAppSectionInfo* ctrlAppSectionInfo(void) { return &g_AppSectionInfo; }