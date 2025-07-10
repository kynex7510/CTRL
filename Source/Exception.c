#include <CTRL/Exception.h>
#include <CTRL/App.h>

#define TLS_EX_HANDLER_SLOT 16
#define TLS_EX_STACK_SLOT 17
#define TLS_EX_CTX_SLOT 18
#define TLS_EX_CHAIN_SLOT 19

// Defined in ExHandler.s.
extern void ctrl_handleException(void);

bool ctrlExceptionHandlingIsSupported(void) {
    // Luma has debugging capabilities always enabled.
    if (ctrlEnv() == Env_Luma)
        return true;

    // Check for debug unit.
    if (OS_KernelConfig->unit_info != 0)
        return true;

    return false;
}

void ctrlEnableExceptionHandling(void) {
    if (ctrlExceptionHandlingIsSupported()) {
        u32* tls = (u32*)getThreadLocalStorage();

        tls[TLS_EX_HANDLER_SLOT] = (u32)ctrl_handleException;
        tls[TLS_EX_STACK_SLOT] = 1; // Use faulty thread stack.
        tls[TLS_EX_CTX_SLOT] = 1; // Write context data to faulty thread stack.

        __dsb();
	    __isb();
    }
}

void ctrlDisableExceptionHandling(void) {
    if (ctrlExceptionHandlingIsSupported()) {
        u32* tls = (u32*)getThreadLocalStorage();

        tls[TLS_EX_HANDLER_SLOT] = 0;
        tls[TLS_EX_STACK_SLOT] = 0;
        tls[TLS_EX_CTX_SLOT] = 0;

        __dsb();
	    __isb();
    }
}

bool ctrlSetExceptionHandler(CTRLExHandlerFn fn, size_t index) {
    if (!ctrlExceptionHandlingIsSupported() || index >= CTRL_MAX_EX_HANDLERS)
        return false;

    u32* tls = (u32*)getThreadLocalStorage();
    tls[TLS_EX_CHAIN_SLOT + index] = (u32)fn;
    return true;
}

CTRLExHandlerFn ctrlGetExceptionHandler(size_t index) {
    if (!ctrlExceptionHandlingIsSupported() || index >= CTRL_MAX_EX_HANDLERS)
        return NULL;

    const u32* tls = (u32*)getThreadLocalStorage();
    return (CTRLExHandlerFn)tls[TLS_EX_CHAIN_SLOT + index];
}

bool ctrlClearExceptionHandler(size_t index) {
    if (!ctrlExceptionHandlingIsSupported() || index >= CTRL_MAX_EX_HANDLERS)
        return false;

    u32* tls = (u32*)getThreadLocalStorage();
    tls[TLS_EX_CHAIN_SLOT + index] = 0;
    return true;
}

bool ctrlExceptionHandlerIsSet(size_t index) {
    if (!ctrlExceptionHandlingIsSupported() || index >= CTRL_MAX_EX_HANDLERS)
        return false;

    const u32* tls = (u32*)getThreadLocalStorage();
    return tls[TLS_EX_CHAIN_SLOT + index] != 0;
}