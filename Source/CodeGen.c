#include "CTRL/CodeGen.h"
#include "CTRL/App.h"
#include "CTRL/Memory.h"

#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define CODE_REGION_START 0x100000
#define CODE_REGION_SIZE 0x3F00000

#define ERR_NO_MEM MAKERESULT(RL_PERMANENT, RS_OUTOFRESOURCE, RM_APPLICATION, RD_OUT_OF_MEMORY);

typedef struct {
    u32 size;
} BlockHeader;

typedef struct {
    u32 sourceAddr;
    u32 regionSize;
} RegionHeader;

static void* ctrl_alignedRealloc(void *p, size_t newSize, size_t alignment) {
    void* q = aligned_alloc(alignment, newSize);
    if (q) {
        const size_t ogSize = malloc_usable_size(p);
        memcpy(q, p, ogSize < newSize ? ogSize : newSize);
        free(p);
    }

    return q;
}

static Result ctrl_findAddressForCodeMirror(size_t size, u32* out) {
    size_t queriedSize = 0;
    while (queriedSize < CODE_REGION_SIZE) {
        MemInfo info;
        Result ret = ctrlQueryRegion(CODE_REGION_START + queriedSize, &info);
        if (R_FAILED(ret))
            return ret;

        if (info.state == MEMSTATE_FREE && info.size >= size)
            break;

        queriedSize += info.size;
    }

    if (queriedSize >= CODE_REGION_SIZE)
        return ERR_NO_MEM;

    *out = CODE_REGION_START + queriedSize;
    return 0;
}

u8* ctrlAllocCodeBlock(CTRLCodeRegion* region, size_t size) {
    // Align code block size to word.
    size = ctrlAlignSize(size, sizeof(u32));

    // Init region if needed.
    if (!*region) {
        // A dummy block of size zero tells when the chain ends.
        const size_t newSize = sizeof(RegionHeader) + sizeof(BlockHeader) * 2 + size;
        RegionHeader* r = aligned_alloc(CTRL_PAGE_SIZE, newSize);
        if (!r)
            return 0;

        r->sourceAddr = (u32)r;
        r->regionSize = newSize;
        ((BlockHeader*)((u32)r + sizeof(RegionHeader)))->size = size;
        ((BlockHeader*)((u32)r + sizeof(RegionHeader) + size))->size = 0;
        *region = (CTRLCodeRegion)r;
        return (u8*)((u32)r + sizeof(RegionHeader) + sizeof(BlockHeader));
    }

    // Add space for code block.
    RegionHeader* r = (RegionHeader*)*region;
    const size_t newSize = r->regionSize + sizeof(BlockHeader) + size;
    r = ctrl_alignedRealloc(r, newSize, CTRL_PAGE_SIZE);
    if (!r)
        return 0;

    // Write block info: last block gets allocated, and new block becomes the zero block.
    const size_t offsetToLastBlock = r->regionSize - sizeof(BlockHeader);
    const size_t offsetToNewBlock = newSize - sizeof(BlockHeader);
    r->sourceAddr = (u32)r;
    r->regionSize = newSize;
    ((BlockHeader*)((u32)r + offsetToLastBlock))->size = size;
    ((BlockHeader*)((u32)r + offsetToNewBlock))->size = 0;
    *region = (CTRLCodeRegion)r;
    return (u8*)((u32)r + offsetToLastBlock + sizeof(BlockHeader));
}

Result ctrlCommitCodeRegion(CTRLCodeRegion* region) {
    RegionHeader* r = (RegionHeader*)*region;

    // Compute the base address.
    u32 baseAddr = 0;
    Result ret = ctrl_findAddressForCodeMirror(r->regionSize, &baseAddr);
    if (R_FAILED(ret))
        return ret;

    // Map code.
    const u32 sourceAddr = r->sourceAddr;
    const size_t regionSize = ctrlAlignSize(r->regionSize, CTRL_PAGE_SIZE);
    ret = ctrlMirror(baseAddr, sourceAddr, regionSize);
    if (R_FAILED(ret))
        return ret;

    // Make it executable.
    ret = ctrlChangePerms(baseAddr, regionSize, MEMPERM_READEXECUTE);
    if (R_FAILED(ret)) {
        ctrlUnmirror(baseAddr, sourceAddr, regionSize);
        return ret;
    }

    // Flush cache.
    ret = ctrlFlushCache(CTRL_ICACHE | CTRL_DCACHE);
    if (R_FAILED(ret)) {
        ctrlUnmirror(baseAddr, sourceAddr, regionSize);
        return ret;
    }

    *region = (CTRLCodeRegion)baseAddr;
    return ret;
}

Result ctrlDestroyCodeRegion(CTRLCodeRegion* region) {
    const RegionHeader* r = (RegionHeader*)*region;
    const u32 baseAddr = (u32)*region;
    const u32 sourceAddr = r->sourceAddr;
    const u32 regionSize = ctrlAlignSize(r->regionSize, CTRL_PAGE_SIZE);

    Result ret = ctrlUnmirror(baseAddr, sourceAddr, regionSize);
    if (R_FAILED(ret))
        return ret;

    free((void*)sourceAddr);
    *region = NULL;
    return 0;
}

u32 ctrlFirstCodeBlock(CTRLCodeRegion region) {
    const u32 codeBlock = (u32)region + sizeof(RegionHeader) + sizeof(BlockHeader);
    const BlockHeader* b = (BlockHeader*)(codeBlock - sizeof(BlockHeader));
    return b->size ? codeBlock : 0;
}

u32 ctrlNextCodeBlock(u32 codeBlock) {
    const BlockHeader* thisB = (BlockHeader*)(codeBlock - sizeof(BlockHeader));
    const BlockHeader* nextB = (BlockHeader*)(codeBlock + thisB->size);
    return nextB->size ? codeBlock + thisB->size + sizeof(BlockHeader) : 0;
}

size_t ctrlNumCodeBlocks(CTRLCodeRegion region) {
    size_t num = 0;
    u32 codeBlock = ctrlFirstCodeBlock(region);

    while (codeBlock) {
        ++num;
        codeBlock = ctrlNextCodeBlock(codeBlock);
    }

    return num;
}

u32 ctrlGetCodeBlock(CTRLCodeRegion region, size_t index) {
    u32 codeBlock = ctrlFirstCodeBlock(region);

    for (size_t i = 0; codeBlock; ++i) {
        if (i >= index)
            break;

        codeBlock = ctrlNextCodeBlock(codeBlock);
    }

    return codeBlock;
}