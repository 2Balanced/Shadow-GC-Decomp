# Round 3 for src/RenderWare/RwFreeList.c: the free list setup with its sizes in locals.
SYMBOLS = {"fn_8047FA30": "R5FreeList *fn_8047FA30(u32 entrySize, u32 entriesPerBlock, u32 alignment, u32 preallocBlocks,\n                        R5FreeList *inPlace, u32 hint) {"}
HEAD = """
    R5FreeList *freeList;
    u32 heapSize;
    u32 blockSize;

    if (!lbl_805EDEA0) {
        preallocBlocks = 0;
    }
    if (alignment == 0) {
        alignment = 32;
    }
    if (inPlace == 0) {
        if (lbl_805F156C != 0) {
            inPlace = (R5FreeList *)RWSRCGLOBAL(memoryAlloc)(lbl_805F156C, hint & 0xFF0000);
        } else {
            inPlace = (R5FreeList *)RWSRCGLOBAL(rwmalloc)(sizeof(R5FreeList), hint & 0xFF0000);
        }
        if (inPlace == 0) {
            return 0;
        }
        inPlace->flags = 2;
    } else {
        inPlace->flags = 3;
    }
    freeList = inPlace;
"""
LOOP = """    while (preallocBlocks != 0) {
        R5Link *block = (R5Link *)RWSRCGLOBAL(rwmalloc)(blockSize, hint);

        if (block == 0) {
            fn_8047FC24(freeList);
            return 0;
        }
        block->prev = 0;
        block->next = 0;
        block->next = freeList->blockList.next;
        block->prev = &freeList->blockList;
        freeList->blockList.next->prev = block;
        freeList->blockList.next = block;
        memset(block + 1, 0, heapSize);
        preallocBlocks--;
    }
    freeList->link.next = lbl_805F1570.next;
    freeList->link.prev = &lbl_805F1570;
    lbl_805F1570.next->prev = &freeList->link;
    lbl_805F1570.next = &freeList->link;
    return freeList;
"""
SIZES = {
 "s1": """    entrySize = (entrySize + alignment - 1) & ~(alignment - 1);
    heapSize = (entriesPerBlock + 7) >> 3;
    freeList->entrySize = entrySize;
    freeList->entriesPerBlock = entriesPerBlock;
    freeList->heapSize = heapSize;
    freeList->alignment = alignment;
    freeList->blockList.next = &freeList->blockList;
    freeList->blockList.prev = &freeList->blockList;
    blockSize = heapSize + entriesPerBlock * entrySize + alignment + sizeof(R5Link) - 1;
""",
 "s2": """    entrySize = (alignment + entrySize - 1) & ~(alignment - 1);
    heapSize = (entriesPerBlock + 7) >> 3;
    freeList->entrySize = entrySize;
    freeList->entriesPerBlock = entriesPerBlock;
    freeList->heapSize = heapSize;
    freeList->alignment = alignment;
    freeList->blockList.next = &freeList->blockList;
    freeList->blockList.prev = &freeList->blockList;
    blockSize = heapSize + entriesPerBlock * entrySize + alignment + sizeof(R5Link) - 1;
""",
 "s3": """    freeList->entrySize = (entrySize + alignment - 1) & ~(alignment - 1);
    freeList->entriesPerBlock = entriesPerBlock;
    heapSize = (entriesPerBlock + 7) >> 3;
    freeList->heapSize = heapSize;
    freeList->alignment = alignment;
    freeList->blockList.next = &freeList->blockList;
    freeList->blockList.prev = &freeList->blockList;
    blockSize = heapSize + entriesPerBlock * freeList->entrySize + alignment + sizeof(R5Link) - 1;
""",
 "s4": """    entrySize = ((entrySize + alignment) - 1) & ~(alignment - 1);
    heapSize = (entriesPerBlock + 7) >> 3;
    freeList->entrySize = entrySize;
    freeList->entriesPerBlock = entriesPerBlock;
    freeList->heapSize = heapSize;
    freeList->alignment = alignment;
    freeList->blockList.next = &freeList->blockList;
    freeList->blockList.prev = &freeList->blockList;
    blockSize = heapSize + entriesPerBlock * entrySize + alignment + 7;
""",
 "s5": """    entrySize = (entrySize + alignment - 1) & ~(alignment - 1);
    heapSize = (entriesPerBlock + 7) >> 3;
    blockSize = heapSize + entriesPerBlock * entrySize + alignment + sizeof(R5Link) - 1;
    freeList->entrySize = entrySize;
    freeList->entriesPerBlock = entriesPerBlock;
    freeList->heapSize = heapSize;
    freeList->alignment = alignment;
    freeList->blockList.next = &freeList->blockList;
    freeList->blockList.prev = &freeList->blockList;
""",
}
BODIES = {"fn_8047FA30": {k: HEAD + v + LOOP for k, v in SIZES.items()}}
