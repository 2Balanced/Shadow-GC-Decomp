"""Colour setters of Game/RwCpp27.cpp (wave 40, agent r3): retail loads the 255.0f constant first."""
F = """void fn_80465370(const RwRGBAReal *ambient, f32 scale) {
    GXColor amb;
    f32 s = lbl_805FB1E8 * scale;
    amb.r = (u8)(ambient->red * s);
    amb.g = (u8)(ambient->green * s);
    amb.b = (u8)(ambient->blue * s);
    amb.a = 0;
"""
REPL = {
    "base": [],
    "literal": [(F, F.replace("lbl_805FB1E8 * scale", "255.0f * scale"))],
    "const_ext": [("extern f32 lbl_805FB1E8;", "extern const f32 lbl_805FB1E8;")],
    "inline_expr": [(F, """void fn_80465370(const RwRGBAReal *ambient, f32 scale) {
    GXColor amb;
    amb.r = (u8)(ambient->red * (lbl_805FB1E8 * scale));
    amb.g = (u8)(ambient->green * (lbl_805FB1E8 * scale));
    amb.b = (u8)(ambient->blue * (lbl_805FB1E8 * scale));
    amb.a = 0;
""")],
    "alpha_first": [(F, """void fn_80465370(const RwRGBAReal *ambient, f32 scale) {
    GXColor amb;
    f32 s = lbl_805FB1E8 * scale;
    amb.a = 0;
    amb.r = (u8)(ambient->red * s);
    amb.g = (u8)(ambient->green * s);
    amb.b = (u8)(ambient->blue * s);
""")],
    "lit_inline": [(F, """void fn_80465370(const RwRGBAReal *ambient, f32 scale) {
    GXColor amb;
    amb.r = (u8)(ambient->red * (255.0f * scale));
    amb.g = (u8)(ambient->green * (255.0f * scale));
    amb.b = (u8)(ambient->blue * (255.0f * scale));
    amb.a = 0;
""")],
}
SCORE = ["fn_80465370"]
CE = ("extern f32 lbl_805FB1E8;", "extern const f32 lbl_805FB1E8;")
REPL.update({
    "ce_nonconst": [CE, (F, F.replace("const RwRGBAReal *ambient", "RwRGBAReal *ambient"))],
    "ce_lit": [CE, (F, F.replace("lbl_805FB1E8 * scale", "255.0f * scale"))],
    "ce_noS": [CE, (F, """void fn_80465370(const RwRGBAReal *ambient, f32 scale) {
    GXColor amb;
    scale = lbl_805FB1E8 * scale;
    amb.r = (u8)(ambient->red * scale);
    amb.g = (u8)(ambient->green * scale);
    amb.b = (u8)(ambient->blue * scale);
    amb.a = 0;
""")],
    "ce_tmp": [CE, (F, """void fn_80465370(const RwRGBAReal *ambient, f32 scale) {
    GXColor amb;
    f32 s = lbl_805FB1E8 * scale;
    f32 r = ambient->red * s;
    f32 g = ambient->green * s;
    f32 b = ambient->blue * s;
    amb.r = (u8)r;
    amb.g = (u8)g;
    amb.b = (u8)b;
    amb.a = 0;
""")],
    "ce_alpha_first": [CE, (F, """void fn_80465370(const RwRGBAReal *ambient, f32 scale) {
    GXColor amb;
    f32 s = lbl_805FB1E8 * scale;
    amb.a = 0;
    amb.r = (u8)(ambient->red * s);
    amb.g = (u8)(ambient->green * s);
    amb.b = (u8)(ambient->blue * s);
""")],
    "ce_u32": [CE, (F, F.replace("(u8)(", "(u32)("))],
    "ce_s32": [CE, (F, F.replace("(u8)(", "(s32)("))],
    "ce_noconv": [CE, (F, F.replace("(u8)(", "("))],
})
M = """    meshes = (RxGCInstanceMesh *)((u8 *)header + sizeof(RxGCInstanceHeader) +
                                  (header->numArrays - 1) * sizeof(RxGCInstanceArray));
    base = (u8 *)(((u32)meshes + numMeshes * sizeof(RxGCInstanceMesh) + 31) & ~31);
    for (i = 0; i < header->numArrays; i++) {
        header->arrays[i].data = base + (u32)header->arrays[i].data;
    }"""
REPL.update({
    "r_a": [(M, M.replace("""(RxGCInstanceMesh *)((u8 *)header + sizeof(RxGCInstanceHeader) +
                                  (header->numArrays - 1) * sizeof(RxGCInstanceArray));""", "(RxGCInstanceMesh *)(&header->arrays[header->numArrays - 1] + 1);"))],
    "r_b": [(M, M.replace("""(RxGCInstanceMesh *)((u8 *)header + sizeof(RxGCInstanceHeader) +
                                  (header->numArrays - 1) * sizeof(RxGCInstanceArray));""", "(RxGCInstanceMesh *)((u8 *)(header + 1) + (header->numArrays - 1) * sizeof(RxGCInstanceArray));"))],
    "r_c": [(M, M.replace("""(RxGCInstanceMesh *)((u8 *)header + sizeof(RxGCInstanceHeader) +
                                  (header->numArrays - 1) * sizeof(RxGCInstanceArray));""", "(RxGCInstanceMesh *)((u8 *)header + (header->numArrays - 1) * sizeof(RxGCInstanceArray) + sizeof(RxGCInstanceHeader));"))],
    "r_d": [(M, M.replace("""(RxGCInstanceMesh *)((u8 *)header + sizeof(RxGCInstanceHeader) +
                                  (header->numArrays - 1) * sizeof(RxGCInstanceArray));""", "(RxGCInstanceMesh *)(header->arrays + header->numArrays - 1 + 1);"))],
    "r_e": [(M, M.replace("""(RxGCInstanceMesh *)((u8 *)header + sizeof(RxGCInstanceHeader) +
                                  (header->numArrays - 1) * sizeof(RxGCInstanceArray));""", "(RxGCInstanceMesh *)&header->arrays[header->numArrays - 1];\n    meshes++;"))],
})
SCORE = ["fn_80464A94"]
RB = REPL["r_b"][0]
DR = """    u32 version;
    u32 size;
    u32 headerSize;
    u32 dataSize;
    s32 platform;
    RxGCInstanceHeader *header;
    RxGCInstanceMesh *meshes;
    u8 *base;
    u32 i;

    if (!fn_8047CF50(stream, 1, &size, &version)) {"""
REPL.update({
    "b_ifirst": [RB, (DR, DR.replace("    u32 i;\n\n", "\n").replace("    u32 version;\n", "    u32 i;\n    u32 version;\n", 1))],
    "b_imid": [RB, (DR, DR.replace("    u32 i;\n\n", "\n").replace("    RxGCInstanceHeader *header;\n", "    u32 i;\n    RxGCInstanceHeader *header;\n", 1))],
    "b_imid2": [RB, (DR, DR.replace("    u32 i;\n\n", "\n").replace("    u8 *base;\n", "    u32 i;\n    u8 *base;\n", 1))],
    "b_s32": [RB, (DR, DR.replace("    u32 i;\n", "    s32 i;\n"))],
    "b_basefirst": [RB, (DR, DR.replace("    RxGCInstanceHeader *header;\n    RxGCInstanceMesh *meshes;\n    u8 *base;\n", "    u8 *base;\n    RxGCInstanceMesh *meshes;\n    RxGCInstanceHeader *header;\n"))],
    "b_meshfirst": [RB, (DR, DR.replace("    RxGCInstanceHeader *header;\n    RxGCInstanceMesh *meshes;\n    u8 *base;\n", "    RxGCInstanceHeader *header;\n    u8 *base;\n    RxGCInstanceMesh *meshes;\n"))],
})
LA = """    for (i = 0; i < header->numArrays; i++) {
        header->arrays[i].data = base + (u32)header->arrays[i].data;
    }
    for (i = 0; i < numMeshes; i++) {
        meshes[i].displayList = base + (u32)meshes[i].displayList;
    }
    (*repEntryRef)->link.next = NULL;"""
REPL.update({
    "b_walk": [RB, (LA, """    {
        RxGCInstanceArray *array = header->arrays;
        for (i = 0; i < header->numArrays; i++) {
            array->data = base + (u32)array->data;
            array++;
        }
    }
    for (i = 0; i < numMeshes; i++) {
        meshes[i].displayList = base + (u32)meshes[i].displayList;
    }
    (*repEntryRef)->link.next = NULL;""")],
    "b_walk2": [RB, (LA, """    {
        RxGCInstanceArray *array = header->arrays;
        for (i = 0; i < header->numArrays; i++, array++) {
            array->data = (void *)((u32)base + (u32)array->data);
        }
    }
    for (i = 0; i < numMeshes; i++) {
        meshes[i].displayList = base + (u32)meshes[i].displayList;
    }
    (*repEntryRef)->link.next = NULL;""")],
    "b_add_order": [RB, (LA, LA.replace("header->arrays[i].data = base + (u32)header->arrays[i].data;", "header->arrays[i].data = (u8 *)header->arrays[i].data + (u32)base;"))],
    "b_pluseq": [RB, (LA, LA.replace("header->arrays[i].data = base + (u32)header->arrays[i].data;", "*(u32 *)&header->arrays[i].data += (u32)base;"))],
})
REPL.update({
    "b_ij": [RB, (DR, DR.replace("    u32 i;\n", "    u32 i;\n    u32 j;\n")),
             (LA, LA.replace("    for (i = 0; i < numMeshes; i++) {\n        meshes[i].displayList = base + (u32)meshes[i].displayList;", "    for (j = 0; j < numMeshes; j++) {\n        meshes[j].displayList = base + (u32)meshes[j].displayList;"))],
    "b_ji": [RB, (DR, DR.replace("    u32 i;\n", "    u32 j;\n    u32 i;\n")),
             (LA, LA.replace("    for (i = 0; i < numMeshes; i++) {\n        meshes[i].displayList = base + (u32)meshes[i].displayList;", "    for (j = 0; j < numMeshes; j++) {\n        meshes[j].displayList = base + (u32)meshes[j].displayList;"))],
    "b_ji2": [RB, (DR, DR.replace("    u32 i;\n", "    u32 j;\n    u32 i;\n")),
             (LA, LA.replace("    for (i = 0; i < header->numArrays; i++) {\n        header->arrays[i].data = base + (u32)header->arrays[i].data;", "    for (j = 0; j < header->numArrays; j++) {\n        header->arrays[j].data = base + (u32)header->arrays[j].data;"))],
    "b_scoped": [RB, (DR, DR.replace("    u32 i;\n", "")),
             (LA, "    {\n        u32 i;\n" + LA.replace("    (*repEntryRef)->link.next = NULL;", "    }\n    (*repEntryRef)->link.next = NULL;"))],
})
BW = "    base = (u8 *)(((u32)meshes + numMeshes * sizeof(RxGCInstanceMesh) + 31) & ~31);\n    for (i = 0; i < header->numArrays; i++) {\n        header->arrays[i].data = (void *)((u8 *)header->arrays[i].data - base);"
REPL.update({
    "w_base1": [(BW, BW.replace("((u32)meshes + numMeshes * sizeof(RxGCInstanceMesh) + 31)", "((u32)(meshes + numMeshes) + 31)"))],
    "w_base2": [(BW, BW.replace("((u32)meshes + numMeshes * sizeof(RxGCInstanceMesh) + 31)", "((u32)&meshes[numMeshes] + 31)"))],
    "w_base3": [(BW, BW.replace("((u32)meshes + numMeshes * sizeof(RxGCInstanceMesh) + 31)", "((u32)meshes + (numMeshes * sizeof(RxGCInstanceMesh) + 31))"))],
})
SCORE = ["fn_80464DAC"]
W3 = REPL["w_base3"][0]
DF = "    DCFlushRange(header, repEntry->size);\n    GXInvalidateVtxCache();\n    return stream;"
REPL.update({
    "w_df": [W3, (DF, DF.replace("DCFlushRange(header,", "DCFlushRange(repEntry + 1,"))],
    "w_u16": [W3, (DF, DF.replace("DCFlushRange(header,", "DCFlushRange(repEntry + 1,")),
              ("RwStream *fn_80464DAC(RwStream *stream, RwResEntry *repEntry, u32 numMeshes) {", "RwStream *fn_80464DAC(RwStream *stream, RwResEntry *repEntry, u16 numMeshes) {")],
    "w_hs": [W3, (DF, DF.replace("DCFlushRange(header,", "DCFlushRange(repEntry + 1,")),
             ("""    headerSize = sizeof(RxGCInstanceHeader) + (header->numArrays - 1) * sizeof(RxGCInstanceArray) +
                 numMeshes * sizeof(RxGCInstanceMesh);""", """    headerSize = (header->numArrays - 1) * sizeof(RxGCInstanceArray) + numMeshes * sizeof(RxGCInstanceMesh) +
                 sizeof(RxGCInstanceHeader);""")],
})
IN = """        for (j = 0; j < numIndices; j++) {
            u16 index = *src;
            u32 below = 0;
            u16 k;
            if (remap[index] != -1) {
                index = remap[index];
            }
            for (k = 0; k < index; k++) {
                if (remap[k] != -1) {
                    below++;
                }
            }
            src++;
            *dst = index - below;
            dst++;
        }"""
HELP = """static u32 RwCpp27CountWelded(const s32 *remap, u16 index) {
    u32 below = 0;
    u16 k;
    for (k = 0; k < index; k++) {
        if (remap[k] != -1) {
            below++;
        }
    }
    return below;
}

u16 **fn_80465CDC("""
REPL.update({
    "c_help": [("u16 **fn_80465CDC(", HELP), (IN, """        for (j = 0; j < numIndices; j++) {
            u16 index = *src;
            if (remap[index] != -1) {
                index = remap[index];
            }
            src++;
            *dst = index - RwCpp27CountWelded(remap, index);
            dst++;
        }""")],
    "c_help2": [("u16 **fn_80465CDC(", HELP), (IN, """        for (j = 0; j < numIndices; j++) {
            u16 index = *src++;
            if (remap[index] != -1) {
                index = remap[index];
            }
            *dst++ = index - RwCpp27CountWelded(remap, index);
        }""")],
    "c_limit": [(IN, IN.replace("            for (k = 0; k < index; k++) {", "            {\n            u16 limit = index;\n            for (k = 0; k < limit; k++) {").replace("            src++;\n", "            }\n            src++;\n"))],
})
SCORE = ["fn_80465CDC"]
REPL.update({
    "c_u32": [("u16 **fn_80465CDC(", HELP), (IN, """        for (j = 0; j < numIndices; j++) {
            u32 index = *src++;
            if (remap[index] != -1) {
                index = (u16)remap[index];
            }
            *dst++ = index - RwCpp27CountWelded(remap, index);
        }""")],
    "c_u32b": [(IN, """        for (j = 0; j < numIndices; j++) {
            u32 index = *src;
            u32 below = 0;
            u16 k;
            if (remap[index] != -1) {
                index = (u16)remap[index];
            }
            for (k = 0; k < (u16)index; k++) {
                if (remap[k] != -1) {
                    below++;
                }
            }
            src++;
            *dst = index - below;
            dst++;
        }""")],
})
REPL.update({
    "c_idx": [(IN, """        for (j = 0; j < numIndices; j++) {
            u16 index = src[j];
            u32 below = 0;
            u16 k;
            if (remap[index] != -1) {
                index = remap[index];
            }
            for (k = 0; k < index; k++) {
                if (remap[k] != -1) {
                    below++;
                }
            }
            dst[j] = index - below;
        }""")],
    "c_order": [(IN, """        for (j = 0; j < numIndices; j++) {
            u32 below;
            u16 k;
            u16 index;
            index = *src;
            if (remap[index] != -1) {
                index = remap[index];
            }
            below = 0;
            for (k = 0; k < index; k++) {
                if (remap[k] != -1) {
                    below++;
                }
            }
            *dst = index - below;
            src++;
            dst++;
        }""")],
    "c_s32below": [(IN, IN.replace("u32 below = 0;", "s32 below = 0;"))],
    "c_u16below": [(IN, IN.replace("u32 below = 0;", "u16 below = 0;"))],
    "c_postinc": [(IN, """        for (j = 0; j < numIndices; j++) {
            u16 index = *src++;
            u32 below = 0;
            u16 k;
            if (remap[index] != -1) {
                index = remap[index];
            }
            for (k = 0; k < index; k++) {
                if (remap[k] != -1) {
                    below++;
                }
            }
            *dst++ = index - below;
        }""")],
})
REPL.update({
    "c_sub1": [(IN, IN.replace("            *dst = index - below;", "            index -= below;\n            *dst = index;"))],
    "c_sub2": [(IN, IN.replace("            *dst = index - below;", "            *dst = index;\n            *dst -= below;"))],
    "c_sub3": [(IN, IN.replace("            *dst = index - below;", "            *dst = (u16)(index - below);"))],
    "c_sub4": [(IN, IN.replace("            *dst = index - below;", "            *dst = (u16)index - (u16)below;"))],
})
