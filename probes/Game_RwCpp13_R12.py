"""Wave 40, RwCpp13 round 12: fn_8044DD1C's bone counter (retail keeps bone++ beside the matrix
offset += 64)."""
SCORE = ["fn_8044DD1C"]
B0 = """        u32 bone = run->start;
        u32 count = run->length;
        u32 j;

        for (j = 0; j < count; j++) {
            fn_8044DC4C((R2Matrix *)lbl_8056D708.matrixCache + bone, id, normals);
            id += 3;
            bone++;
        }
"""
REPL = {
 "base": [(B0, B0)],
 "offset": [(B0, """        u32 bone = run->start;
        u32 count = run->length;
        u32 offset = bone * sizeof(R2Matrix);
        u32 j;

        for (j = 0; j < count; j++) {
            fn_8044DC4C((R2Matrix *)((u8 *)lbl_8056D708.matrixCache + offset), id, normals);
            id += 3;
            bone++;
            offset += sizeof(R2Matrix);
        }
""")],
 "volbone": [(B0, B0.replace("u32 bone = run->start;", "volatile u32 bone = run->start;"))],
 "postinc": [(B0, B0.replace("+ bone, id, normals);\n            id += 3;\n            bone++;", "+ bone++, id, normals);\n            id += 3;"))],
 "decl_order": [(B0, """        u32 j;
        u32 bone = run->start;
        u32 count = run->length;

        for (j = 0; j < count; j++) {
            fn_8044DC4C((R2Matrix *)lbl_8056D708.matrixCache + bone, id, normals);
            id += 3;
            bone++;
        }
""")],
 "whilej": [(B0, """        u32 bone = run->start;
        u32 count = run->length;
        u32 j = 0;

        while (j < count) {
            fn_8044DC4C((R2Matrix *)lbl_8056D708.matrixCache + bone, id, normals);
            id += 3;
            bone++;
            j++;
        }
""")],
}
