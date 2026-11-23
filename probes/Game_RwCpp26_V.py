"""Vertex descriptor setup fn_80462C30 of Game/RwCpp26.cpp (wave 40, agent r3)."""
D = """    RxGCInstanceHeader *header;
    u32 arrayIndex;

    if (desc == NULL) {
        desc = &lbl_805E1688;
    }
    header = (RxGCInstanceHeader *)(data->repEntry + 1);
"""
REPL = {
    "ai_first": [(D, """    u32 arrayIndex;
    RxGCInstanceHeader *header;

    if (desc == NULL) {
        desc = &lbl_805E1688;
    }
    header = (RxGCInstanceHeader *)(data->repEntry + 1);
""")],
    "ai_first_hdr": [(D, """    u32 arrayIndex;
    RxGCInstanceHeader *header;

    header = (RxGCInstanceHeader *)(data->repEntry + 1);
    if (desc == NULL) {
        desc = &lbl_805E1688;
    }
""")],
    "ai_first_nohdr": [(D, """    u32 arrayIndex;
    RxGCInstanceHeader *header = (RxGCInstanceHeader *)(data->repEntry + 1);

    if (desc == NULL) {
        desc = &lbl_805E1688;
    }
""")],
}
SCORE = ["fn_80462C30"]
AF = REPL["ai_first"][0]
REPL.update({
    "t_s32": [(D, AF[1].replace("u32 arrayIndex;", "s32 arrayIndex;"))],
    "t_u8": [(D, AF[1].replace("u32 arrayIndex;", "u8 arrayIndex;"))],
    "t_u16": [(D, AF[1].replace("u32 arrayIndex;", "u16 arrayIndex;"))],
    "t_s16": [(D, AF[1].replace("u32 arrayIndex;", "s16 arrayIndex;"))],
})
POS = """    GXSetVtxDesc(R3_GX_VA_POS, header->arrays[0].attrType);
    GXSetVtxAttrFmt(R3_GX_VTXFMT0, R3_GX_VA_POS, R3_GX_POS_XYZ, desc->type[0], desc->size[0]);
    GXSetArray(R3_GX_VA_POS, header->arrays[0].data, header->arrays[0].stride);
    arrayIndex = 1;
"""
POS0 = """    arrayIndex = 0;
    GXSetVtxDesc(R3_GX_VA_POS, header->arrays[arrayIndex].attrType);
    GXSetVtxAttrFmt(R3_GX_VTXFMT0, R3_GX_VA_POS, R3_GX_POS_XYZ, desc->type[0], desc->size[0]);
    GXSetArray(R3_GX_VA_POS, header->arrays[arrayIndex].data, header->arrays[arrayIndex].stride);
    arrayIndex++;
"""
REPL.update({
    "p0": [AF, (POS, POS0)],
    "p0_init": [(D, AF[1].replace("u32 arrayIndex;", "u32 arrayIndex = 0;")), (POS, POS0.replace("    arrayIndex = 0;\n", ""))],
    "p1_init": [(D, AF[1].replace("u32 arrayIndex;", "u32 arrayIndex = 1;")), (POS, POS.replace("    arrayIndex = 1;\n", ""))],
})
NB = """            GXSetArray(R3_GX_VA_NBT, header->arrays[arrayIndex].data, header->arrays[arrayIndex].stride);
        } else {
            GXSetVtxDesc(R3_GX_VA_NRM, header->arrays[arrayIndex].attrType);
            GXSetVtxAttrFmt(R3_GX_VTXFMT0, R3_GX_VA_NRM, R3_GX_NRM_XYZ, desc->type[1], 0);
            GXSetArray(R3_GX_VA_NRM, header->arrays[arrayIndex].data, header->arrays[arrayIndex].stride);
        }
        arrayIndex++;
"""
NB2 = """            GXSetArray(R3_GX_VA_NBT, header->arrays[arrayIndex].data, header->arrays[arrayIndex].stride);
            arrayIndex++;
        } else {
            GXSetVtxDesc(R3_GX_VA_NRM, header->arrays[arrayIndex].attrType);
            GXSetVtxAttrFmt(R3_GX_VTXFMT0, R3_GX_VA_NRM, R3_GX_NRM_XYZ, desc->type[1], 0);
            GXSetArray(R3_GX_VA_NRM, header->arrays[arrayIndex].data, header->arrays[arrayIndex].stride);
            arrayIndex++;
        }
"""
REPL.update({
    "p0_both": [AF, (POS, POS0), (NB, NB2)],
})
