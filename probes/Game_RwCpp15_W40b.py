"""Wave 40 (r2), RwCpp15 round 2: fn_80451BCC's loop variables (retail: i r23, id r24; remaining
r24 signed, mesh r25) and the order of its first loads."""
SCORE = ["fn_80451BCC"]
L0 = "        s32 id = 0;\n        u32 i;\n"
M0 = "        u32 remaining = meshHeader->numMeshes;\n        R2Mesh *mesh = (R2Mesh *)(meshHeader + 1);\n"
H0 = """    node->resEntry->instance.token = lbl_805EDF40;
    list = (R2DisplayList *)&instance->normals + (instance->numArrays - 1);
    meshHeader = atomic->geometry->mesh;
"""
REPL = {
 "base": [(L0, L0)],
 "a": [(L0, "        u32 i;\n        s32 id = 0;\n"), (M0, "        s32 remaining = meshHeader->numMeshes;\n        R2Mesh *mesh = (R2Mesh *)(meshHeader + 1);\n")],
 "b": [(L0, "        u32 i;\n        s32 id = 0;\n"), (M0, "        R2Mesh *mesh = (R2Mesh *)(meshHeader + 1);\n        s32 remaining = meshHeader->numMeshes;\n")],
 "c": [(L0, "        u32 i;\n        s32 id = 0;\n"), (M0, "        R2Mesh *mesh;\n        s32 remaining;\n\n        remaining = meshHeader->numMeshes;\n        mesh = (R2Mesh *)(meshHeader + 1);\n")],
 "d": [(L0, "        u32 i;\n        s32 id = 0;\n"), (M0, "        s32 remaining;\n        R2Mesh *mesh;\n\n        remaining = meshHeader->numMeshes;\n        mesh = (R2Mesh *)(meshHeader + 1);\n")],
}
