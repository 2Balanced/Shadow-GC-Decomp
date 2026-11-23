"""Wave 40, RwCpp13 round 6: fn_8044DD1C's run tables (retail: lbzx of the mesh's first run, the
run address as meshRLE + i * 2 + start * 2, and a bone counter that stays incremented)."""
SCORE = ["fn_8044DD1C"]
H0 = """    const R2SkinRLE *mesh = &((const R2SkinRLE *)skin->split.meshRLECount)[meshIndex];
    u32 start = mesh->start & 0x7F;
    u32 numRuns = mesh->length;
"""
H1 = """    const u8 *counts = (const u8 *)skin->split.meshRLECount;
    u32 start = counts[meshIndex * 2] & 0x7F;
    u32 numRuns = counts[meshIndex * 2 + 1];
"""
R0 = "        const R2SkinRLE *run = (const R2SkinRLE *)skin->split.meshRLE + i + start;\n"
L0 = """        for (j = 0; j < count; j++) {
            fn_8044DC4C((R2Matrix *)lbl_8056D708.matrixCache + bone, id, normals);
            id += 3;
            bone++;
        }
"""
REPL = {
 "base": [(H0, H0)],
 "bytes": [(H0, H1)],
 "bytes_r1": [(H0, H1), (R0, "        const R2SkinRLE *run = (const R2SkinRLE *)((const u8 *)skin->split.meshRLE + i * 2 + start * 2);\n")],
 "bytes_r2": [(H0, H1), (R0, "        const R2SkinRLE *run = &((const R2SkinRLE *)skin->split.meshRLE)[start + i];\n")],
 "bytes_r3": [(H0, H1), (R0, "        const R2SkinRLE *run = &((const R2SkinRLE *)skin->split.meshRLE)[i] + start;\n")],
 "bytes_r4": [(H0, H1.replace("u32 start = counts[meshIndex * 2] & 0x7F;", "u32 start = (counts[meshIndex * 2] & 0x7F) * 2;")), (R0, "        const R2SkinRLE *run = (const R2SkinRLE *)((const u8 *)skin->split.meshRLE + i * 2 + start);\n")],
 "bytes_for": [(H0, H1), (L0, L0.replace("j++) {", "j++, bone++) {").replace("            bone++;\n", ""))],
 "bytes_idx": [(H0, H1), (L0, "        for (j = 0; j < count; j++) {\n            fn_8044DC4C(&((R2Matrix *)lbl_8056D708.matrixCache)[bone], id, normals);\n            id += 3;\n            bone++;\n        }\n")],
}
