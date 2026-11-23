"""Wave 41, RenderWare/RpSkin: fn_8044C8C0 small shapes."""
SCORE = ["fn_8044C8C0"]
Z_OLD = """        usedBones = skin->usedBones;
        skin->numUsedBones = 0;
        for (i = 0; i < numVertices; i++) {"""
REPL = {
    "base": [],
    "z1": [(Z_OLD, """        usedBones = skin->usedBones;
        skin->numUsedBones = i = 0;
        for (; i < numVertices; i++) {""")],
    "z2": [(Z_OLD, """        i = 0;
        usedBones = skin->usedBones;
        skin->numUsedBones = 0;
        for (; i < numVertices; i++) {""")],
    "z3": [(Z_OLD, """        usedBones = skin->usedBones;
        i = 0;
        skin->numUsedBones = i;
        for (; i < numVertices; i++) {""")],
    "sk1": [("        skin = fn_8044BDF0(numVertices, numBones, numBones, 4, NULL, NULL, NULL);\n        if (!fn_804810FC(stream, 4)) {",
             "        if ((skin = fn_8044BDF0(numVertices, numBones, numBones, 4, NULL, NULL, NULL)), !fn_804810FC(stream, 4)) {")],
    "sk2": [("    R2Atomic *atomic = (R2Atomic *)object;\n    R2Skin *skin;\n    R2Geometry *geometry = atomic->geometry;",
             "    R2Skin *skin;\n    R2Atomic *atomic = (R2Atomic *)object;\n    R2Geometry *geometry = atomic->geometry;")],
}
Z1 = REPL["z1"][0]
REPL.update({
    "z1_sk2": [Z1, REPL["sk2"][0]],
    "z1_nv": [Z1, ("        numVertices = geometry->numVertices;\n        skin = fn_8044BDF0(numVertices, numBones, numBones, 4, NULL, NULL, NULL);",
                   "        skin = fn_8044BDF0(numVertices = geometry->numVertices, numBones, numBones, 4, NULL, NULL, NULL);")],
    "z1_nv2": [Z1, ("        numVertices = geometry->numVertices;\n        skin = fn_8044BDF0(numVertices, numBones, numBones, 4, NULL, NULL, NULL);",
                   "        skin = fn_8044BDF0(geometry->numVertices, numBones, numBones, 4, NULL, NULL, NULL);\n        numVertices = geometry->numVertices;")],
    "z1_decl": [Z1, ("    u32 numVertices;\n    u32 numBones;\n    u32 i;\n", "    u32 numBones;\n    u32 numVertices;\n    u32 i;\n")],
})
J_OLD = "            for (j = 0, shift = 0; j < skin->maxWeights; j++) {"
REPL.update({
    "z1_j1": [Z1, (J_OLD, "            for (j = shift = 0; j < skin->maxWeights; j++) {")],
    "z1_j2": [Z1, (J_OLD, "            for (shift = j = 0; j < skin->maxWeights; j++) {")],
    "z1_j3": [Z1, ("            weight = &weights->w[0];\n" + J_OLD, "            j = 0;\n            weight = &weights->w[0];\n            for (shift = j; j < skin->maxWeights; j++) {")],
})
