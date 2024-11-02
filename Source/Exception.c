#include "CTRL/Exception.h"

extern void ctrl_handleException();

void ctrlSetExceptionHandler(CTRLExHandlerFn fn) {
    u32* tls = (u32*)getThreadLocalStorage();
    tls[16] = (u32)ctrl_handleException;
    tls[17] = 1; // Use default stack.
    tls[18] = 1; // Write context data to stack.
    tls[19] = (u32)fn; // Used by custom handler.
}

void ctrlRemoveExceptionHandler(void) {
    u32* tls = (u32*)getThreadLocalStorage();
    tls[16] = 0;
    tls[17] = 0;
    tls[18] = 0;
    tls[19] = 0;
}