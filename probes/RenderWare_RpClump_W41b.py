"""Wave 41, RenderWare/RpClump: RpClumpAddAtomic (fn_80457608) as a linked-list macro."""
SCORE = ["fn_80457608"]
SRC = open("src/RenderWare/RpClump.c", newline="").read().replace("\r\n", "\n")


def _span(text, start, end, after=0):
    """The text from start up to and including end (searched from start); a marker when the
    anchor is gone because a variant of this table was kept in the source."""
    try:
        a = text.index(start)
        b = text.index(end, a) + len(end) + after
    except ValueError:
        return "<<anchor gone: a variant of this table is in the source>>"
    return text[a:b]

OLD = _span(SRC, "void fn_80457608(u32 *object, u32 *other) {", "\n}\n", 3 - len("\n}\n"))
LINK = """typedef struct ClumpLink {
    struct ClumpLink *next;
    struct ClumpLink *prev;
} ClumpLink;

"""
V1 = LINK + """u8 *fn_80457608(u8 *clump, u8 *atomic) {
    ClumpLink *list = (ClumpLink *)(clump + 8);
    ClumpLink *link = (ClumpLink *)(atomic + 0x40);

    link->next = list->next;
    link->prev = list;
    list->next->prev = link;
    list->next = link;
    *(u8 **)(atomic + 0x3C) = clump;
    return clump;
}
"""
V2 = LINK + """#define LINK_ADD(list, link)          \
    do {                              \
        (link)->next = (list)->next;  \
        (link)->prev = (list);        \
        (list)->next->prev = (link);  \
        (list)->next = (link);        \
    } while (0)

u8 *fn_80457608(u8 *clump, u8 *atomic) {
    LINK_ADD((ClumpLink *)(clump + 8), (ClumpLink *)(atomic + 0x40));
    *(u8 **)(atomic + 0x3C) = clump;
    return clump;
}
"""
V3 = V1.replace("u8 *fn_80457608(u8 *clump, u8 *atomic) {\n    ClumpLink *list = (ClumpLink *)(clump + 8);\n    ClumpLink *link = (ClumpLink *)(atomic + 0x40);\n", "u8 *fn_80457608(u8 *clump, u8 *atomic) {\n    ClumpLink *link = (ClumpLink *)(atomic + 0x40);\n    ClumpLink *list = (ClumpLink *)(clump + 8);\n")
REPL = {"base": [], "v1": [(OLD, V1)], "v2": [(OLD, V2)], "v3": [(OLD, V3)]}
DECL = ("void fn_80457608();\n", "u8 *fn_80457608();\n")
for k in ("v1", "v2", "v3"):
    REPL[k] = [DECL] + REPL[k]
for k in ("v1", "v2", "v3"):
    pairs = []
    for old, new in REPL[k]:
        new = new.replace("u8 *fn_80457608(u8 *clump, u8 *atomic) {", "void *fn_80457608(void *clump, void *atomic) {")
        new = new.replace("(clump + 8)", "((u8 *)clump + 8)").replace("(atomic + 0x40)", "((u8 *)atomic + 0x40)").replace("*(u8 **)(atomic + 0x3C) = clump;", "*(void **)((u8 *)atomic + 0x3C) = clump;")
        new = new.replace("u8 *fn_80457608();\n", "void *fn_80457608();\n")
        pairs.append((old, new))
    REPL[k] = pairs
V4 = LINK + """typedef struct ClumpHead {
    u8 object[8];
    ClumpLink atomicList;
} ClumpHead;

typedef struct AtomicHead {
    u8 pad[0x3C];
    void *clump;
    ClumpLink inClumpLink;
} AtomicHead;

#define LINK_ADD(list, link)          \
    do {                              \
        (link)->next = (list)->next;  \
        (link)->prev = (list);        \
        (list)->next->prev = (link);  \
        (list)->next = (link);        \
    } while (0)

void *fn_80457608(void *clump, void *atomic) {
    LINK_ADD(&((ClumpHead *)clump)->atomicList, &((AtomicHead *)atomic)->inClumpLink);
    ((AtomicHead *)atomic)->clump = clump;
    return clump;
}
"""
V5 = V4.replace("""void *fn_80457608(void *clump, void *atomic) {
    LINK_ADD(&((ClumpHead *)clump)->atomicList, &((AtomicHead *)atomic)->inClumpLink);
    ((AtomicHead *)atomic)->clump = clump;
    return clump;
}""", """ClumpHead *fn_80457608(ClumpHead *clump, AtomicHead *atomic) {
    LINK_ADD(&clump->atomicList, &atomic->inClumpLink);
    atomic->clump = clump;
    return clump;
}""")
REPL["v4"] = [DECL[0:1] and ("void fn_80457608();\n", "void *fn_80457608();\n"), (OLD, V4)]
REPL["v5"] = [("void fn_80457608();\n", "void *fn_80457608();\n"), (OLD, V5)]
V6 = V4.replace("""    LINK_ADD(&((ClumpHead *)clump)->atomicList, &((AtomicHead *)atomic)->inClumpLink);""", """    ClumpLink *next = ((ClumpHead *)clump)->atomicList.next;
    ClumpLink *list = &((ClumpHead *)clump)->atomicList;
    ClumpLink *link = &((AtomicHead *)atomic)->inClumpLink;

    link->next = next;
    link->prev = list;
    list->next->prev = link;
    list->next = link;""")
V7 = V4.replace("""    LINK_ADD(&((ClumpHead *)clump)->atomicList, &((AtomicHead *)atomic)->inClumpLink);""", """    ((AtomicHead *)atomic)->inClumpLink.next = ((ClumpHead *)clump)->atomicList.next;
    ((AtomicHead *)atomic)->inClumpLink.prev = &((ClumpHead *)clump)->atomicList;
    ((ClumpHead *)clump)->atomicList.next->prev = &((AtomicHead *)atomic)->inClumpLink;
    ((ClumpHead *)clump)->atomicList.next = &((AtomicHead *)atomic)->inClumpLink;""")
V8 = V4.replace("void *fn_80457608(void *clump, void *atomic) {", "void *fn_80457608(void *clump, const void *atomicc) {\n    void *atomic = (void *)atomicc;")
V9 = V4.replace("#define LINK_ADD(list, link)          \\n    do {                              \\n        (link)->next = (list)->next;  \\n        (link)->prev = (list);        \\n", "#define LINK_ADD(list, link)          \\n    do {                              \\n        (link)->prev = (list);        \\n        (link)->next = (list)->next;  \\n")
for k, v in (("v6", V6), ("v7", V7), ("v8", V8), ("v9", V9)):
    REPL[k] = [("void fn_80457608();\n", "void *fn_80457608();\n"), (OLD, v)]
