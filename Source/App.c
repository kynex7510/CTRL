#include <CTRL/App.h>
#include <CTRL/Memory.h>

#include <string.h>

#define TYPE_LUMA_VERSION 0x10000
#define TYPE_CITRA_EMULATOR_ID 0x20000

#define EMULATOR_ID_CITRA 1
#define EMULATOR_ID_AZAHAR 2

#define TEXT_ADDR_TYPE 0x10005
#define TEXT_SIZE_TYPE 0x10002
#define RODATA_ADDR_TYPE 0x10006
#define RODATA_SIZE_TYPE 0x10003
#define DATA_ADDR_TYPE 0x10007
#define DATA_SIZE_TYPE 0x10004

#define LUMA_SYSTEM_VERSION(major, minor, revision) (((major) << 24) | ((minor) << 16) | ((revision) << 8))

static CTRLEnv g_Env;
static CTRLAppSectionInfo g_AppSectionInfo;

static inline bool isCitraKnownEmulator(u32 id) { return id == EMULATOR_ID_CITRA || id == EMULATOR_ID_AZAHAR; }

static inline bool detectEnv(CTRLEnv* env) {
    s64 check = 0;

    // Detected luma 8.0.0+.
    if (R_SUCCEEDED(svcGetSystemInfo(&check, TYPE_LUMA_VERSION, 0)) && (check >= LUMA_SYSTEM_VERSION(8, 0, 0))) {
        *env = Env_Luma;
        return true;
    }
    
    // Detected citra.
    if (R_SUCCEEDED(svcGetSystemInfo(&check, TYPE_CITRA_EMULATOR_ID, 0)) && isCitraKnownEmulator(check)) {
        *env = Env_Citra;
        return true;
    }

    return false;
}

static inline Result getAppSectionInfo(CTRLAppSectionInfo* out) {
    s64 tmp;
    Result ret;

#define READ_INFO(type, outMember)                               \
    ret = svcGetProcessInfo(&tmp, CUR_PROCESS_HANDLE, ((type))); \
    if (R_FAILED(ret))                                           \
        return ret;                                              \
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

static __attribute((constructor)) void initEnv(void) {
    if (!detectEnv(&g_Env))
        svcBreak(USERBREAK_PANIC);
}

static __attribute((constructor)) void initAppSectionInfo(void) {
    if (R_FAILED(getAppSectionInfo(&g_AppSectionInfo)))
        svcBreak(USERBREAK_PANIC);
}

CTRLEnv ctrlEnv(void) { return g_Env; }
const CTRLAppSectionInfo* ctrlAppSectionInfo(void) { return &g_AppSectionInfo; }