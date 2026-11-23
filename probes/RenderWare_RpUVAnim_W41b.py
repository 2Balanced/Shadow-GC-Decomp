"""Wave 41, RenderWare/RpUVAnim: the eight-slot scans as loops (fn_804549DC, fn_80454294)."""
SCORE = ["fn_804549DC", "fn_80454294"]
SRC = open("src/RenderWare/RpUVAnim.c", newline="").read().replace("\r\n", "\n")


def _span(text, start, end, after=0):
    """The text from start up to and including end (searched from start); a marker when the
    anchor is gone because a variant of this table was kept in the source."""
    try:
        a = text.index(start)
        b = text.index(end, a) + len(end) + after
    except ValueError:
        return "<<anchor gone: a variant of this table is in the source>>"
    return text[a:b]

OLD_DC = _span(SRC, "s32 fn_804549DC(u32 *object) {", "\n}\n", 3 - len("\n}\n"))
OLD_294 = _span(SRC, "u32 fn_80454294(u32 *object) {", "\n}\n", 3 - len("\n}\n"))
DC1 = """s32 fn_804549DC(u32 *object) {
    u32 *slots = (u32 *)((u8 *)object + uvAnimMaterialOffset);
    s32 i;

    for (i = 0; i < 8; i++) {
        if (slots[2 + i] != 0) {
            return 1;
        }
    }
    return 0;
}
"""
DC2 = """s32 fn_804549DC(u32 *object) {
    u32 *slot = (u32 *)((u8 *)object + uvAnimMaterialOffset);
    s32 i;

    for (i = 0; i < 8; i++) {
        if (slot[2] != 0) {
            return 1;
        }
        slot++;
    }
    return 0;
}
"""
S1 = """u32 fn_80454294(u32 *object) {
    u32 *slots;
    u32 size;
    s32 i;

    if (fn_804549DC(object) == 0) {
        return 0;
    }
    slots = (u32 *)((u8 *)object + uvAnimMaterialOffset);
    size = 16;
    for (i = 0; i < 8; i++) {
        if (slots[2 + i] != 0) {
            size += 32;
        }
    }
    return size;
}
"""
S2 = S1.replace("    size = 16;\n", "    size = 0x10;\n")
REPL = {
    "base": [],
    "dc1": [(OLD_DC, DC1)],
    "dc2": [(OLD_DC, DC2)],
    "dc1_s1": [(OLD_DC, DC1), (OLD_294, S1)],
    "dc2_s1": [(OLD_DC, DC2), (OLD_294, S1)],
}
