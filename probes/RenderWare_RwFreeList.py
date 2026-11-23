# Probe table for src/RenderWare/RwFreeList.c (tools/pprobe.py -u RenderWare/<Unit> -p <table>): the block release loop.
SYMBOLS = {"fn_8047FC24": "void fn_8047FC24(R5FreeList *freeList) {"}
TAIL = """    if (!(freeList->flags & 1)) {
        if (lbl_805F156C == freeList || lbl_805F156C == 0) {
            RWSRCGLOBAL(rwfree)(freeList);
        } else {
            RWSRCGLOBAL(memoryFree)(lbl_805F156C, freeList);
        }
    }
"""
UNLINK = """        block->prev->next = block->next;
        block->next->prev = block->prev;
        RWSRCGLOBAL(rwfree)(block);
"""
BODIES = {"fn_8047FC24": {
 "while_a": "\n    R5Link *block = freeList->blockList.next;\n\n    while (block != &freeList->blockList) {\n" + UNLINK + "        block = freeList->blockList.next;\n    }\n" + TAIL,
 "while_b": "\n    while (freeList->blockList.next != &freeList->blockList) {\n        R5Link *block = freeList->blockList.next;\n\n" + UNLINK + "    }\n" + TAIL,
 "while_c": "\n    R5Link *block;\n\n    while ((block = freeList->blockList.next) != &freeList->blockList) {\n" + UNLINK + "    }\n" + TAIL,
 "while_d": "\n    R5Link *block;\n\n    block = freeList->blockList.next;\n    while (block != &freeList->blockList) {\n        R5Link *cur = block;\n\n        cur->prev->next = cur->next;\n        cur->next->prev = cur->prev;\n        RWSRCGLOBAL(rwfree)(cur);\n        block = freeList->blockList.next;\n    }\n" + TAIL,
 "cast_e": "\n    R5Link *block;\n\n    for (block = freeList->blockList.next; block != (R5Link *)((u8 *)freeList + 0x10); block = freeList->blockList.next) {\n" + UNLINK + "    }\n" + TAIL,
 "list_f": "\n    R5LinkList *list = (R5LinkList *)&freeList->blockList;\n    R5Link *block;\n\n    for (block = list->link.next; block != &list->link; block = list->link.next) {\n" + UNLINK + "    }\n" + TAIL,
}}
