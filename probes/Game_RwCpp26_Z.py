"""Render callback fn_80462914 of Game/RwCpp26.cpp (wave 40, agent r3)."""
H = """    header = (RxGCInstanceHeader *)(data->repEntry + 1);
    header->serialNum = lbl_805EDF40;
    displayLists = (RxGCInstanceMesh *)&header->arrays[header->numArrays];
"""
DL = ["(RxGCInstanceMesh *)&header->arrays[header->numArrays]",
      "(RxGCInstanceMesh *)(header->arrays + header->numArrays)",
      "(RxGCInstanceMesh *)((u8 *)header + header->numArrays * sizeof(RxGCInstanceArray) + 12)",
      "(RxGCInstanceMesh *)((u8 *)(header + 1) + (header->numArrays - 1) * sizeof(RxGCInstanceArray))"]
RAS = "                fn_80497778(!(((u32 *)((u8 *)texture->raster + lbl_805F1658))[5] & 1));"
RAS2 = "                fn_80497778(!(((u32 *)((u8 *)texture->raster->parent + lbl_805F1658))[5] & 1));"
RAS3 = "                fn_80497778((((u32 *)((u8 *)texture->raster->parent + lbl_805F1658))[5] & 1) ^ 1);"
DECL = "    u32 numMeshes;\n    RpMesh *mesh;\n"
DECL2 = "    RpMesh *mesh;\n    u32 numMeshes;\n"
REPL = {}
for i, d in enumerate(DL):
    for order in (0, 1):
        if order == 0:
            h = "    header = (RxGCInstanceHeader *)(data->repEntry + 1);\n    header->serialNum = lbl_805EDF40;\n    displayLists = %s;\n" % d
        else:
            h = "    header = (RxGCInstanceHeader *)(data->repEntry + 1);\n    displayLists = %s;\n    header->serialNum = lbl_805EDF40;\n" % d
        REPL["d%d%d" % (i, order)] = [(H, h), (RAS, RAS3), (DECL, DECL2)]
REPL["ras2"] = [(RAS, RAS2), (DECL, DECL2)]
REPL["ras3"] = [(RAS, RAS3), (DECL, DECL2)]
SCORE = ["fn_80462914"]
DL2 = ["(RxGCInstanceMesh *)((u8 *)header + 12 + header->numArrays * sizeof(RxGCInstanceArray))",
       "(RxGCInstanceMesh *)((u8 *)header->arrays + header->numArrays * sizeof(RxGCInstanceArray))",
       "(RxGCInstanceMesh *)((RxGCInstanceArray *)header + header->numArrays) + 0",
       "(RxGCInstanceMesh *)((u8 *)&header->arrays[0] + header->numArrays * sizeof(RxGCInstanceArray))",
       "(RxGCInstanceMesh *)((u32)header + header->numArrays * sizeof(RxGCInstanceArray) + 12)",
       "(RxGCInstanceMesh *)(header->numArrays * sizeof(RxGCInstanceArray) + (u8 *)header + 12)"]
for i, d in enumerate(DL2):
    h = "    header = (RxGCInstanceHeader *)(data->repEntry + 1);\n    displayLists = %s;\n    header->serialNum = lbl_805EDF40;\n" % d
    REPL["e%d" % i] = [(H, h), (RAS, RAS3), (DECL, DECL2)]
