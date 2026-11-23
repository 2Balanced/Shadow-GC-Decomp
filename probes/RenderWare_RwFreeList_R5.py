# Round 5 for src/RenderWare/RwFreeList.c: the free list setup's store order.
import itertools
exec(open('probes/RenderWare_RwFreeList_R3.py').read())
stores = ["freeList->entrySize = size;", "freeList->entriesPerBlock = entriesPerBlock;", "freeList->heapSize = heapSize;",
          "freeList->alignment = alignment;"]
BODIES = {"fn_8047FA30": {}}
for i, perm in enumerate(itertools.permutations(stores)):
    for j, hs in enumerate(["first", "before_block"]):
        s = "    {\n        u32 size = (entrySize + (alignment - 1)) & ~(alignment - 1);\n\n"
        if hs == "first":
            s += "        heapSize = (entriesPerBlock + 7) >> 3;\n"
        s += "".join("        " + x + "\n" for x in perm)
        s += "        freeList->blockList.next = &freeList->blockList;\n        freeList->blockList.prev = &freeList->blockList;\n"
        if hs != "first":
            s = s.replace("        freeList->heapSize = heapSize;\n", "        freeList->heapSize = heapSize = (entriesPerBlock + 7) >> 3;\n")
        s += "        blockSize = sizeof(R5Link) + heapSize + entriesPerBlock * size + alignment - 1;\n    }\n"
        BODIES["fn_8047FA30"]["st%02d_%s" % (i, hs)] = HEAD + s + LOOP
