#include <CTRL/CodeGen.h>
#include <CTRL/CodeAllocator.h>
#include <CTRL/Memory.h>

#include <stdlib.h> // malloc, free, realloc
#include <string.h> // memcpy

typedef struct {
    u32 size;
} BlockHeader;

typedef struct {
    u32 allocAddr;
    u32 regionSize;
} RegionHeader;

u8* ctrlAllocCodeBlock(CTRLCodeRegion* region, size_t size) {
    // Align code block size to word.
    size = ctrlAlignSize(size, sizeof(u32));

    // Init region if needed.
    if (!*region) {
        // A dummy block of size zero tells when the chain ends.
        const size_t newSize = sizeof(RegionHeader) + sizeof(BlockHeader) * 2 + size;
        RegionHeader* r = malloc(newSize);
        if (!r)
            return NULL;

        r->allocAddr = 0;
        r->regionSize = newSize;
        ((BlockHeader*)((u32)r + sizeof(RegionHeader)))->size = size;
        ((BlockHeader*)((u32)r + sizeof(RegionHeader) + size))->size = 0;
        *region = (CTRLCodeRegion)r;
        return (u8*)((u32)r + sizeof(RegionHeader) + sizeof(BlockHeader));
    }

    // Add space for code block.
    RegionHeader* r = (RegionHeader*)*region;
    const size_t newSize = r->regionSize + sizeof(BlockHeader) + size;
    r = realloc(r, newSize);
    if (!r)
        return NULL;

    // Write block info: last block gets allocated, and new block becomes the zero block.
    const size_t offsetToLastBlock = r->regionSize - sizeof(BlockHeader);
    const size_t offsetToNewBlock = newSize - sizeof(BlockHeader);

    r->allocAddr = 0;
    r->regionSize = newSize;
    ((BlockHeader*)((u32)r + offsetToLastBlock))->size = size;
    ((BlockHeader*)((u32)r + offsetToNewBlock))->size = 0;
    *region = (CTRLCodeRegion)r;
    return (u8*)((u32)r + offsetToLastBlock + sizeof(BlockHeader));
}

Result ctrlCommitCodeRegion(CTRLCodeRegion* region) {
    RegionHeader* r = (RegionHeader*)*region;
    const size_t numPages = ctrlSizeToNumPages(r->regionSize);

    // Allocate code pages.
    Result ret = ctrlAllocCodePages(numPages, &r->allocAddr);
    if (R_FAILED(ret))
        return ret;

    // Copy data.
    memcpy((void*)r->allocAddr, r, r->regionSize);

    // Commit code pages.
    u32 commitAddr = 0;
    ret = ctrlCommitCodePages(r->allocAddr, numPages, &commitAddr);
    if (R_FAILED(ret)) {
        ctrlFreeCodePages(r->allocAddr, numPages);
        r->allocAddr = 0;
        return ret;
    }

    free(r);
    *region = (CTRLCodeRegion)commitAddr;
    return ret;
}

Result ctrlDestroyCodeRegion(CTRLCodeRegion* region) {
    const RegionHeader* r = (RegionHeader*)*region;
    const u32 allocAddr = r->allocAddr;
    const u32 commitAddr = (u32)r;
    const size_t numPages = ctrlSizeToNumPages(r->regionSize);

    Result ret = ctrlReleaseCodePages(allocAddr, commitAddr, numPages);
    if (R_FAILED(ret))
        return ret;

    ret = ctrlFreeCodePages(allocAddr, numPages);
    if (R_FAILED(ret))
        return ret;

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