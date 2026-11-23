# Probe table for src/RenderWare/RwResources.c (tools/pprobe.py -u RenderWare/<Unit> -p <table>).
SYMBOLS = {"fn_804807CC": "void fn_804807CC(void) {"}
MOVE = """            newer->link.next = older->link.next;
            newer->link.next->prev = &newer->link;
            newer->link.prev = older->link.prev;
            newer->link.prev->next = &newer->link;
            older->link.next = &older->link;
            older->link.prev = &older->link;
"""
APPEND = """            R5Link *last = newer->link.prev;
            R5Link *first = older->link.next;

            last->next = first;
            first->prev = last;
            first = older->link.prev;
            first->next = &newer->link;
            newer->link.prev = first;
            older->link.next = &older->link;
            older->link.prev = &older->link;
"""
TAIL = """    RWRESOURCESGLOBAL(usedEntries) = older;
    RWRESOURCESGLOBAL(freeEntries) = newer;
    RWRESOURCESGLOBAL(reusageSize) = 0;
"""
def body(decl):
    return ("\n" + decl + "\n    if (older->link.next != &older->link) {\n        if (newer->link.next == &newer->link) {\n" + MOVE +
            "        } else {\n" + APPEND + "        }\n    }\n" + TAIL)
BODIES = {"fn_804807CC": {
    "dup_on": body("    R5LinkList *older = RWRESOURCESGLOBAL(freeEntries);\n    R5LinkList *newer = RWRESOURCESGLOBAL(usedEntries);\n"),
    "dup_no": body("    R5LinkList *newer = RWRESOURCESGLOBAL(usedEntries);\n    R5LinkList *older = RWRESOURCESGLOBAL(freeEntries);\n"),
    "dup_split": body("    R5LinkList *older;\n    R5LinkList *newer;\n\n    newer = RWRESOURCESGLOBAL(usedEntries);\n    older = RWRESOURCESGLOBAL(freeEntries);\n"),
    "dup_split2": body("    R5LinkList *newer;\n    R5LinkList *older;\n\n    older = RWRESOURCESGLOBAL(freeEntries);\n    newer = RWRESOURCESGLOBAL(usedEntries);\n"),
}}
