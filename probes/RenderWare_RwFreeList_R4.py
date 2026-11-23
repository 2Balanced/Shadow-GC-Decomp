# Round 4 for src/RenderWare/RwFreeList.c: the free list setup's size expressions.
import itertools
exec(open('probes/RenderWare_RwFreeList_R3.py').read())
EXPRS = {"e1": "(%s + alignment - 1) & ~(alignment - 1)", "e2": "(alignment + %s - 1) & ~(alignment - 1)",
         "e3": "(%s + (alignment - 1)) & ~(alignment - 1)", "e5": "((alignment - 1) + %s) & ~(alignment - 1)"}
BLOCKS = {"b1": "heapSize + entriesPerBlock * %s + alignment + sizeof(R5Link) - 1",
          "b2": "sizeof(R5Link) + heapSize + entriesPerBlock * %s + alignment - 1",
          "b3": "heapSize + entriesPerBlock * %s + (alignment - 1) + sizeof(R5Link)",
          "b4": "heapSize + %s * entriesPerBlock + alignment + sizeof(R5Link) - 1"}
BODIES = {"fn_8047FA30": {}}
for (en, e), (bn, b) in itertools.product(EXPRS.items(), BLOCKS.items()):
    # new local for the aligned size
    s = ("    {\n        u32 size = " + (e % "entrySize") + ";\n\n"
         "        heapSize = (entriesPerBlock + 7) >> 3;\n        freeList->entrySize = size;\n"
         "        freeList->entriesPerBlock = entriesPerBlock;\n        freeList->heapSize = heapSize;\n"
         "        freeList->alignment = alignment;\n        freeList->blockList.next = &freeList->blockList;\n"
         "        freeList->blockList.prev = &freeList->blockList;\n        blockSize = " + (b % "size") + ";\n    }\n")
    BODIES["fn_8047FA30"]["loc_%s_%s" % (en, bn)] = HEAD + s + LOOP
    s2 = ("    entrySize = " + (e % "entrySize") + ";\n"
          "    heapSize = (entriesPerBlock + 7) >> 3;\n    freeList->entrySize = entrySize;\n"
          "    freeList->entriesPerBlock = entriesPerBlock;\n    freeList->heapSize = heapSize;\n"
          "    freeList->alignment = alignment;\n    freeList->blockList.next = &freeList->blockList;\n"
          "    freeList->blockList.prev = &freeList->blockList;\n    blockSize = " + (b % "entrySize") + ";\n")
    BODIES["fn_8047FA30"]["par_%s_%s" % (en, bn)] = HEAD + s2 + LOOP
