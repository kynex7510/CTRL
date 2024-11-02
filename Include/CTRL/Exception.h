#ifndef _CTRL_EXCEPTION_H
#define _CTRL_EXCEPTION_H

#include "CTRL/Types.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

typedef void(*CTRLExHandlerFn)(ERRF_ExceptionData*);

void ctrlSetExceptionHandler(CTRLExHandlerFn fn);
void ctrlRemoveExceptionHandler(void);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* _CTRL_EXCEPTION_H */