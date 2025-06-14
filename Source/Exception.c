#include <CTRL/Exception.h>
#include <CTRL/App.h>

#define TLS_EX_HANDLER_SLOT 16
#define TLS_EX_STACK_SLOT 17
#define TLS_EX_CTX_SLOT 18
#define TLS_EX_CHAIN_SLOT 19

// Defined in ExHandler.s.
extern void ctrl_handleException(void);

static bool hasExHandlers(u32* tls) {
    for (size_t i = 0; i < CTRL_MAX_EX_HANDLERS; ++i) {
        if (tls[TLS_EX_CHAIN_SLOT + i])
            return true;
    }

    return false;
}

static void enableExHandlingImpl(u32* tls) {
    tls[TLS_EX_HANDLER_SLOT] = (u32)ctrl_handleException;
    tls[TLS_EX_STACK_SLOT] = 1; // Use faulty thread stack.
    tls[TLS_EX_CTX_SLOT] = 1; // Write context data to faulty thread stack.
}

static void disableExHandlingImpl(u32* tls) {
    tls[TLS_EX_HANDLER_SLOT] = 0;
    tls[TLS_EX_STACK_SLOT] = 0;
    tls[TLS_EX_CTX_SLOT] = 0;
}

bool ctrlExceptionHandlingIsSupported(void) { return ctrlEnv() == Env_Luma; }

bool ctrlEnableExceptionHandling(void) {
    if (ctrlExceptionHandlingIsSupported()) {
        u32* tls = (u32*)getThreadLocalStorage();
        if (hasExHandlers(tls)) {
            enableExHandlingImpl(tls);
            return true;
        }
    }

    return false;
}

void ctrlDisableExceptionHandling(void) { disableExHandlingImpl((u32*)getThreadLocalStorage()); }

bool ctrlSetExceptionHandler(CTRLExHandlerFn fn, size_t index) {
    if (!ctrlExceptionHandlingIsSupported() || (index >= CTRL_MAX_EX_HANDLERS))
        return false;

    u32* tls = (u32*)getThreadLocalStorage();
    tls[TLS_EX_CHAIN_SLOT + index] = (u32)fn;
    if (!tls[TLS_EX_HANDLER_SLOT])
        enableExHandlingImpl(tls);

    return true;
}

bool ctrlClearExceptionHandler(size_t index) {
    if (!ctrlExceptionHandlingIsSupported() || (index >= CTRL_MAX_EX_HANDLERS))
        return false;

    u32* tls = (u32*)getThreadLocalStorage();
    tls[TLS_EX_CHAIN_SLOT + index] = 0;
    if (!hasExHandlers(tls))
        disableExHandlingImpl(tls);

    return true;
}

bool ctrlExceptionHandlerIsSet(size_t index) {
    if (index < CTRL_MAX_EX_HANDLERS)
        return ((u32*)getThreadLocalStorage())[TLS_EX_CHAIN_SLOT + index] != 0;

    return false;
}