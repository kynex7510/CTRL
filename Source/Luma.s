.global lumaFlushDataCacheRange
.global lumaFlushEntireDataCache
.global lumaInvalidateInstructionCacheRange
.global lumaInvalidateEntireInstructionCache

.section .text

.type lumaFlushDataCacheRange, %function
lumaFlushDataCacheRange:
	svc 0x91
	bx lr

.type lumaFlushEntireDataCache, %function
lumaFlushEntireDataCache:
	svc 0x92
	bx lr

.type lumaInvalidateInstructionCacheRange, %function
lumaInvalidateInstructionCacheRange:
	svc 0x93
	bx lr

.type lumaInvalidateEntireInstructionCache, %function
lumaInvalidateEntireInstructionCache:
	svc 0x94
	bx lr