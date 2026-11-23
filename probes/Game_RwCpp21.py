"""_rpMaterialListFindMaterialIndex (fn_8045B8EC) of Game/RwCpp21.cpp: retail loads the material
array into r0 and the scaled count into r5 (wave 40, agent r3)."""
F = """    s32 i = matList->numMaterials;
    RpMaterial **entry = matList->materials + i;
    while (entry--, i-- > 0) {
        if (*entry == material) {
            return i;
        }
    }
    return i;
"""
REPL = {
    "base": [],
    "index": [(F, F.replace("matList->materials + i", "&matList->materials[i]"))],
    "body_dec": [(F, """    s32 i = matList->numMaterials;
    RpMaterial **entry = matList->materials + i;
    while (i-- > 0) {
        entry--;
        if (*entry == material) {
            return i;
        }
    }
    return i;
""")],
    "predec": [(F, """    s32 i = matList->numMaterials;
    RpMaterial **entry = matList->materials + i;
    while (i-- > 0) {
        if (*--entry == material) {
            return i;
        }
    }
    return i;
""")],
    "idx_loop": [(F, """    s32 i = matList->numMaterials;
    while (i-- > 0) {
        if (matList->materials[i] == material) {
            return i;
        }
    }
    return i;
""")],
    "two_step": [(F, """    s32 i = matList->numMaterials;
    RpMaterial **entry = matList->materials;
    entry += i;
    while (entry--, i-- > 0) {
        if (*entry == material) {
            return i;
        }
    }
    return i;
""")],
    "entry_first": [(F, """    RpMaterial **entry;
    s32 i;
    i = matList->numMaterials;
    entry = matList->materials + i;
    while (entry--, i-- > 0) {
        if (*entry == material) {
            return i;
        }
    }
    return i;
""")],
    "const_entry": [(F, F.replace("RpMaterial **entry", "RpMaterial *const *entry"))],
    "mats_local": [(F, """    RpMaterial **materials = matList->materials;
    s32 i = matList->numMaterials;
    RpMaterial **entry = materials + i;
    while (entry--, i-- > 0) {
        if (*entry == material) {
            return i;
        }
    }
    return i;
""")],
}
SCORE = ["fn_8045B8EC"]
REPL.update({
    "swap_add": [(F, F.replace("matList->materials + i", "i + matList->materials"))],
    "bytes": [(F, F.replace("matList->materials + i", "(RpMaterial **)((u8 *)matList->materials + i * sizeof(RpMaterial *))"))],
    "bytes2": [(F, F.replace("matList->materials + i", "(RpMaterial **)(i * sizeof(RpMaterial *) + (u8 *)matList->materials)"))],
    "for_ptr": [(F, """    s32 i;
    RpMaterial **entry;
    for (i = matList->numMaterials, entry = matList->materials + i; entry--, i-- > 0;) {
        if (*entry == material) {
            return i;
        }
    }
    return i;
""")],
})
REPL.update({
    "entry_then_i": [(F, """    RpMaterial **entry = matList->materials + matList->numMaterials;
    s32 i = matList->numMaterials;
    while (entry--, i-- > 0) {
        if (*entry == material) {
            return i;
        }
    }
    return i;
""")],
    "n_local": [(F, """    s32 n = matList->numMaterials;
    RpMaterial **entry = matList->materials + n;
    s32 i = n;
    while (entry--, i-- > 0) {
        if (*entry == material) {
            return i;
        }
    }
    return i;
""")],
    "goto": [(F, """    s32 i = matList->numMaterials;
    RpMaterial **entry = matList->materials + i;
    goto test;
loop:
    if (*entry == material) {
        return i;
    }
test:
    if (entry--, i-- > 0) {
        goto loop;
    }
    return i;
""")],
    "ent_i_dec": [(F, F.replace("while (entry--, i-- > 0)", "while (i-- > 0 && (entry--, 1))"))],
})
D = """    s32 count;
    u32 size;
    u32 version;
    s32 *indices;
    s32 i;
    s32 status;

"""
IF1 = "    if (version >= 0x35000 && version <= 0x37002) {\n        status = (fn_8047D868(stream, &count"
REPL.update({
    "inner_all": [(D, "    s32 count;\n    u32 size;\n    u32 version;\n\n"),
                  (IF1, "    if (version >= 0x35000 && version <= 0x37002) {\n        s32 *indices;\n        s32 i;\n        s32 status;\n\n        status = (fn_8047D868(stream, &count")],
    "inner_all2": [(D, "    u32 size;\n    u32 version;\n\n"),
                  (IF1, "    if (version >= 0x35000 && version <= 0x37002) {\n        s32 count;\n        s32 *indices;\n        s32 i;\n        s32 status;\n\n        status = (fn_8047D868(stream, &count")],
    "params_first": [("RpMaterialList *fn_8045B924(RwStream *stream, RpMaterialList *matList) {\n" + D,
                      "RpMaterialList *fn_8045B924(RwStream *stream, RpMaterialList *matList) {\n" + D.replace("    s32 *indices;\n    s32 i;\n    s32 status;\n", "    s32 status;\n    s32 i;\n    s32 *indices;\n"))],
})
SCORE = ["fn_8045B8EC", "fn_8045B924"]
SIG = "RpMaterialList *fn_8045B924(RwStream *stream, RpMaterialList *matList) {\n"
REPL.update({
    "reg_both": [(SIG, "RpMaterialList *fn_8045B924(register RwStream *stream, register RpMaterialList *matList) {\n")],
    "reg_list": [(SIG, "RpMaterialList *fn_8045B924(RwStream *stream, register RpMaterialList *matList) {\n")],
    "reg_locals": [(D, D.replace("    s32 *indices;\n    s32 i;\n", "    register s32 *indices;\n    register s32 i;\n"))],
})
REPL.update({
    "inl_deinit": [("RpMaterialList *fn_8045B6F0(RpMaterialList *matList) {", "__inline RpMaterialList *fn_8045B6F0(RpMaterialList *matList) {")],
    "inl_all": [("RpMaterialList *fn_8045B6F0(RpMaterialList *matList) {", "__inline RpMaterialList *fn_8045B6F0(RpMaterialList *matList) {"),
                ("RpMaterialList *fn_8045B788(RpMaterialList *matList) {", "__inline RpMaterialList *fn_8045B788(RpMaterialList *matList) {"),
                ("RpMaterial *fn_8045B79C(const RpMaterialList *matList, s32 index) {", "__inline RpMaterial *fn_8045B79C(const RpMaterialList *matList, s32 index) {"),
                ("static RpMaterialList *RpMaterialListSetSize(", "static __inline RpMaterialList *RpMaterialListSetSize("),
                ("s32 fn_8045B7AC(RpMaterialList *matList, RpMaterial *material) {", "__inline s32 fn_8045B7AC(RpMaterialList *matList, RpMaterial *material) {")],
})
END = """        RwFree(indices);
        return matList;
    }
    RWERROR((E_RW_BADVERSION));
    return NULL;
}"""
REPL.update({
    "all_inner": [(SIG + D, SIG + "    {\n" + D), (END, END[:-1] + "    }\n}")],
})
