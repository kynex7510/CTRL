#ifndef _CTRL_EXCEPTION_H
#define _CTRL_EXCEPTION_H

#include "CTRL/Types.h"

#define CTRL_MAX_EX_HANDLERS 13

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

typedef void(*CTRLExHandlerFn)(ERRF_ExceptionData*);

bool ctrlExceptionHandlingIsSupported(void);
bool ctrlEnableExceptionHandling(void);
void ctrlDisableExceptionHandling(void);

bool ctrlSetExceptionHandler(CTRLExHandlerFn fn, size_t index);
bool ctrlClearExceptionHandler(size_t index);
bool ctrlExceptionHandlerIsSet(size_t index);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* _CTRL_EXCEPTION_H */