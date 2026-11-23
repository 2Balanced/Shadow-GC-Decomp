"""Wave 40 (r2), RwCpp17 round 1: RpMaterialUVAnimExists (fn_804549DC) as a loop that the compiler
unrolls (retail: each copy loads 12(p) and steps p by 4)."""
SCORE = ["fn_804549DC"]
B0 = """s32 fn_804549DC(u32 *object) {
    u32 *entry = (u32 *)((u8 *)object + lbl_805F149C);
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    return 0;
}
"""
L1 = """s32 fn_804549DC(u32 *object) {
    u32 *entry = (u32 *)((u8 *)object + lbl_805F149C);
    s32 i;
    for (i = 0; i < 8; i++) {
        if (entry[2 + i] != 0) {
            return 1;
        }
    }
    return 0;
}
"""
L2 = """s32 fn_804549DC(u32 *object) {
    u32 *entry = (u32 *)((u8 *)object + lbl_805F149C);
    s32 i;
    for (i = 0; i < 8; i++) {
        if (entry[2] != 0) {
            return 1;
        }
        entry++;
    }
    return 0;
}
"""
L3 = """s32 fn_804549DC(u32 *object) {
    u32 *entry = (u32 *)((u8 *)object + lbl_805F149C) + 2;
    s32 i;
    for (i = 0; i < 8; i++) {
        if (*entry++ != 0) {
            return 1;
        }
    }
    return 0;
}
"""
L4 = """s32 fn_804549DC(u32 *object) {
    u32 *entry = (u32 *)((u8 *)object + lbl_805F149C);
    u32 i;
    for (i = 0; i < 8; i++) {
        if (entry[2 + i] != 0) {
            return 1;
        }
    }
    return 0;
}
"""
REPL = {"base": [(B0, B0)], "l1": [(B0, L1)], "l2": [(B0, L2)], "l3": [(B0, L3)], "l4": [(B0, L4)]}
