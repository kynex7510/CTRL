/**
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Copyright (c) 2024-2026 Kynex7510
 * See the LICENSE file for more info.
 */

#include <CTRL/CodeGen.h>
#include <CTRL/Allocator.h>
#include <CTRL/Memory.h>

#include <stdlib.h> // malloc, free, realloc
#include <string.h> // memcpy

typedef struct {
    u32 size;
} BlockHeader;

typedef struct {
    size_t allocPageIndex;
    size_t aliasPageIndex;
    u32 regionSize;
} RegionHeader;

u8* ctrlAllocCodeBlock(CTRLCodeRegion* region, size_t size) {
    // Align code block size to word.
    size = ctrlAlignUp(size, sizeof(u32));

    // Init region if needed.
    if (!*region) {
        // A dummy block of size zero tells when the chain ends.
        const size_t newSize = sizeof(RegionHeader) + sizeof(BlockHeader) * 2 + size;
        RegionHeader* r = malloc(newSize);
        if (!r)
            return NULL;

        r->allocPageIndex = 0;
        r->aliasPageIndex = 0;
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

    r->allocPageIndex = 0;
    r->aliasPageIndex = 0;
    r->regionSize = newSize;
    ((BlockHeader*)((u32)r + offsetToLastBlock))->size = size;
    ((BlockHeader*)((u32)r + offsetToNewBlock))->size = 0;
    *region = (CTRLCodeRegion)r;
    return (u8*)((u32)r + offsetToLastBlock + sizeof(BlockHeader));
}

Result ctrlReserveCodeRegionMemory(CTRLCodeRegion region, u32* allocAddr, u32* aliasAddr) {
    RegionHeader* r = (RegionHeader*)region;

    if (!r->allocPageIndex) {
        const Result ret = ctrlReserveMappablePages(ctrlSizeToNumPages(r->regionSize), &r->allocPageIndex);
        if (R_FAILED(ret))
            return ret;
    }

    if (!r->aliasPageIndex) {
        const Result ret = ctrlReserveExecutablePages(ctrlSizeToNumPages(r->regionSize), &r->aliasPageIndex);
        if (R_FAILED(ret))
            return ret;
    }

    if (allocAddr)
        *allocAddr = ctrlPageIndexToAddr(r->allocPageIndex);

    if (aliasAddr)
        *aliasAddr = ctrlPageIndexToAddr(r->aliasPageIndex);

    return 0;
}

Result ctrlCommitCodeRegion(CTRLCodeRegion* region) {
    RegionHeader* r = (RegionHeader*)*region;

    // Ensure we have reserved memory.
    Result ret = ctrlReserveCodeRegionMemory(*region, NULL, NULL);
    if (R_FAILED(ret))
        return ret;

    // Allocate mappable memory.
    const size_t numPages = ctrlSizeToNumPages(r->regionSize);
    ret = ctrlMappableAlloc(r->allocPageIndex, numPages);
    if (R_FAILED(ret))
        return ret;

    // Copy data.
    memcpy((void*)ctrlPageIndexToAddr(r->allocPageIndex), r, r->regionSize);

    // Map executable memory.
    ret = ctrlMapExecutablePages(r->allocPageIndex, r->aliasPageIndex, numPages);
    if (R_FAILED(ret)) {
        ctrlMappableFree(r->allocPageIndex, numPages);
        return ret;
    }

    *region = (CTRLCodeRegion)ctrlPageIndexToAddr(r->aliasPageIndex);
    free(r);
    return ret;
}

Result ctrlDestroyCodeRegion(CTRLCodeRegion* region) {
    RegionHeader* r = (RegionHeader*)*region;
    const u32 allocAddr = ctrlPageIndexToAddr(r->allocPageIndex);
    const size_t numPages = ctrlSizeToNumPages(r->regionSize);

    Result ret = ctrlUnmapExecutablePages(r->allocPageIndex, r->aliasPageIndex, numPages);
    if (R_FAILED(ret))
        return ret;

    *region = (CTRLCodeRegion)allocAddr;
    r = (RegionHeader*)allocAddr;
    r->aliasPageIndex = 0;

    ret = ctrlMappableFree(r->allocPageIndex, numPages);
    if (R_SUCCEEDED(ret))
        *region = NULL;

    return ret;
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