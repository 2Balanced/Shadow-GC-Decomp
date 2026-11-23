"""Wave 40, RwCpp13 round 7: types of fn_8044DD1C's run counters (retail keeps the bone counter
incremented beside its strength-reduced matrix offset)."""
SCORE = ["fn_8044DD1C"]
B0 = """        u32 bone = run->start;
        u32 count = run->length;
        u32 j;
"""
REPL = {
 "base": [(B0, B0)],
 "s32bone": [(B0, B0.replace("u32 bone", "s32 bone"))],
 "u16bone": [(B0, B0.replace("u32 bone", "u16 bone"))],
 "s32all": [(B0, B0.replace("u32", "s32"))],
 "s32j": [(B0, B0.replace("u32 j", "s32 j"))],
 "u8count": [(B0, B0.replace("u32 count", "u8 count"))],
 "boneptr": [(B0, "        const R2Matrix *bone;\n        u32 count = run->length;\n        u32 j;\n        u32 first = run->start;\n"),
             ("            fn_8044DC4C((R2Matrix *)lbl_8056D708.matrixCache + bone, id, normals);\n", "            fn_8044DC4C((R2Matrix *)lbl_8056D708.matrixCache + first, id, normals);\n"),
             ("            bone++;\n", "            first++;\n")],
 "whilecnt": [(B0, B0), ("        for (j = 0; j < count; j++) {\n            fn_8044DC4C", "        for (j = count; j != 0; j--) {\n            fn_8044DC4C")],
}
