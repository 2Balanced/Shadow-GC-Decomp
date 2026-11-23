# Round 2 for src/RenderWare/RwFreeList.c: the block release loop.
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
XTRA = {"fn_8047FC24": """static R5Link *ListFirst(R5Link *list) {
    return list->next;
}

static R5Link *ListEnd(R5Link *list) {
    return list;
}

static RwBool ListEmpty(R5Link *list) {
    return list->next == list;
}

"""}
BODIES = {"fn_8047FC24": {
 "goto_a": "\n    R5Link *block;\n\nagain:\n    block = freeList->blockList.next;\n    if (block != &freeList->blockList) {\n" + UNLINK + "        goto again;\n    }\n" + TAIL,
 "forever": "\n    for (;;) {\n        R5Link *block = freeList->blockList.next;\n\n        if (block == &freeList->blockList) {\n            break;\n        }\n" + UNLINK + "    }\n" + TAIL,
 "inl_end": "\n    R5Link *block;\n\n    for (block = ListFirst(&freeList->blockList); block != ListEnd(&freeList->blockList); block = ListFirst(&freeList->blockList)) {\n" + UNLINK + "    }\n" + TAIL,
 "inl_empty": "\n    while (!ListEmpty(&freeList->blockList)) {\n        R5Link *block = freeList->blockList.next;\n\n" + UNLINK + "    }\n" + TAIL,
 "dowhile": "\n    R5Link *block = freeList->blockList.next;\n\n    if (block != &freeList->blockList) {\n        do {\n" + UNLINK + "            block = freeList->blockList.next;\n        } while (block != &freeList->blockList);\n    }\n" + TAIL,
}}
