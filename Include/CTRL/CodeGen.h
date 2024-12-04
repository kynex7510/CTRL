#ifndef _CTRL_CODEGEN_H
#define _CTRL_CODEGEN_H

#include "CTRL/Types.h"

typedef void* CTRLCodeRegion;

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

u8* ctrlAllocCodeBlock(CTRLCodeRegion* region, size_t size);
Result ctrlCommitCodeRegion(CTRLCodeRegion* region);
Result ctrlDestroyCodeRegion(CTRLCodeRegion* region);
u32 ctrlFirstCodeBlock(const CTRLCodeRegion* region);
u32 ctrlNextCodeBlock(u32 codeBlock);
u32 ctrlGetCodeBlock(const CTRLCodeRegion* region, size_t index);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* _CTRL_CODEGEN_H */