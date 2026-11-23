"""RpGeometryStreamRead (fn_804599C8) of Game/RwCpp20.cpp: the morph-target loop keeps the
morph target in r28 and the strength-reduced offset in r29; retail has them the other way
(wave 40, agent r3)."""
DECL = """    RpMorphTarget *morphTarget;
    u32 size;
    s32 i;
    s32 j;
"""
LOOP = """        for (j = 0; j < geometry->numMorphTargets; j++) {
            morphTarget = &geometry->morphTarget[j];
"""
REPL = {
    "base": [],
    "scoped": [(DECL, DECL.replace("    RpMorphTarget *morphTarget;\n", "")),
               (LOOP, LOOP.replace("            morphTarget =", "            RpMorphTarget *morphTarget ="))],
    "decl_after_j": [(DECL, "    u32 size;\n    s32 i;\n    s32 j;\n    RpMorphTarget *morphTarget;\n")],
    "decl_j_first": [(DECL, "    s32 j;\n    RpMorphTarget *morphTarget;\n    u32 size;\n    s32 i;\n")],
    "plus": [(LOOP, LOOP.replace("&geometry->morphTarget[j]", "geometry->morphTarget + j"))],
    "reuse_i": [(LOOP, LOOP.replace("j", "i"))],
    "while": [(LOOP, "        j = 0;\n        while (j < geometry->numMorphTargets) {\n            morphTarget = &geometry->morphTarget[j];\n"),
              ("            }\n        }\n\n        if (!fn_8047CF50(stream, 8", "            }\n            j++;\n        }\n\n        if (!fn_8047CF50(stream, 8")],
    "decl_first": [("    RpGeometry *geometry;\n    u32 version;\n", "    RpMorphTarget *morphTarget;\n    RpGeometry *geometry;\n    u32 version;\n"),
                   (DECL, DECL.replace("    RpMorphTarget *morphTarget;\n", ""))],
    "decl_second": [("    RpGeometry *geometry;\n    u32 version;\n", "    RpGeometry *geometry;\n    RpMorphTarget *morphTarget;\n    u32 version;\n"),
                    (DECL, DECL.replace("    RpMorphTarget *morphTarget;\n", ""))],
}
SCORE = ["fn_804599C8"]
REPL2 = {
    "novar": [(LOOP, "        for (j = 0; j < geometry->numMorphTargets; j++) {\n"),
              ("morphTarget->boundingSphere = morphInfo", "geometry->morphTarget[j].boundingSphere = morphInfo"),
              ("fn_8047D784(stream, morphTarget->verts,\n                                geometry", "fn_8047D784(stream, geometry->morphTarget[j].verts,\n                                geometry"),
              ("fn_8047D784(stream, morphTarget->verts,\n                                    geometry", "fn_8047D784(stream, geometry->morphTarget[j].verts,\n                                    geometry"),
              ("fn_8047D784(stream, morphTarget->normals,", "fn_8047D784(stream, geometry->morphTarget[j].normals,")],
    "novar_plus": [(LOOP, "        for (j = 0; j < geometry->numMorphTargets; j++) {\n"),
              ("morphTarget->boundingSphere = morphInfo", "(geometry->morphTarget + j)->boundingSphere = morphInfo"),
              ("fn_8047D784(stream, morphTarget->verts,\n                                geometry", "fn_8047D784(stream, (geometry->morphTarget + j)->verts,\n                                geometry"),
              ("fn_8047D784(stream, morphTarget->verts,\n                                    geometry", "fn_8047D784(stream, (geometry->morphTarget + j)->verts,\n                                    geometry"),
              ("fn_8047D784(stream, morphTarget->normals,", "fn_8047D784(stream, (geometry->morphTarget + j)->normals,")],
    "assign_late": [(LOOP, "        for (j = 0; j < geometry->numMorphTargets; j++) {\n"),
              ("            fn_8047D254(&morphInfo, sizeof(morphInfo));\n            morphTarget->boundingSphere", "            fn_8047D254(&morphInfo, sizeof(morphInfo));\n            morphTarget = &geometry->morphTarget[j];\n            morphTarget->boundingSphere")],
}
REPL.update(REPL2)
REPL3 = {
    "block": [(DECL, "    u32 size;\n    s32 i;\n"),
              (LOOP, "        {\n        RpMorphTarget *morphTarget;\n        s32 j;\n" + LOOP),
              ("            }\n        }\n\n        if (!fn_8047CF50(stream, 8", "            }\n        }\n        }\n\n        if (!fn_8047CF50(stream, 8")],
    "block_jfirst": [(DECL, "    u32 size;\n    s32 i;\n"),
              (LOOP, "        {\n        s32 j;\n        RpMorphTarget *morphTarget;\n" + LOOP),
              ("            }\n        }\n\n        if (!fn_8047CF50(stream, 8", "            }\n        }\n        }\n\n        if (!fn_8047CF50(stream, 8")],
    "geom_reg": [("    RpGeometry *geometry;\n    u32 version;\n", "    register RpGeometry *geometry;\n    u32 version;\n")],
}
REPL.update(REPL3)
