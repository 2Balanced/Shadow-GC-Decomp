"""Wave 40: LoadParts (Game/UnkMotion_8022F058.cpp). Retail keeps the part in r30, above the
manager (r29), the stage (r28), the counter (r27) and the set (r26)."""
LP = "LoadParts__Q218UnkMotion_8022F0584ImplFlPC15BirdMotionTable"
SYMBOLS = {LP: "void UnkMotion_8022F058::Impl::LoadParts(s32 slot, const BirdMotionTable *table) {"}
HEAD = "    m_partCount = table->m_setCount;\n    m_parts.reset(new Part[m_partCount]);\n"
MS = "    void *manager = fn_800122D4();\n    UnkStage_8003A840 *stage = fn_8003A840();\n"
def loop(setdecl="const BirdMotionSet *set = ", partdecl="Part *part = ", order="sp"):
    s = "        " + setdecl + "&table->m_sets[i];\n"
    p = "        " + partdecl + "&m_parts[i];\n"
    return "    for (int i = 0; i < m_partCount; i++) {\n" + (s + p if order == "sp" else p + s) + """        RpClump *model = fn_8001277C(manager, slot, set->m_file);
        if (model != 0) {
            part->m_clump = fn_80457090(model);
            fn_80038C84(part->m_clump);
            int area = fn_80169350(fn_80169364(part->m_clump));
            RpClump *clump = part->m_clump;
            SetClumpWorld(clump, stage->GetWorld(area));
            if (set->field_0x0 >= 0) {
                part->m_attach = fn_80038D24(m_parts[0].m_clump, set->field_0x0);
            }
            if (part->m_attach == 0) {
                part->m_attach = m_frame;
            }
        }
        SetupPart(slot, part, set);
    }
"""
BODIES = {LP: {
    "base": HEAD + MS + loop(),
    "part_top": "    Part *part;\n" + HEAD + MS + loop(partdecl="part = "),
    "part_top2": HEAD + "    Part *part;\n" + MS + loop(partdecl="part = "),
    "part_set_top": "    Part *part;\n    const BirdMotionSet *set;\n" + HEAD + MS + loop(partdecl="part = ", setdecl="set = "),
    "part_first_in_loop": HEAD + MS + loop(order="ps"),
    "part_const": HEAD + MS + loop(partdecl="Part *const part = "),
    "part_const_ps": HEAD + MS + loop(partdecl="Part *const part = ", order="ps"),
    "both_const": HEAD + MS + loop(partdecl="Part *const part = ", setdecl="const BirdMotionSet *const set = "),
    "both_const_ps": HEAD + MS + loop(partdecl="Part *const part = ", setdecl="const BirdMotionSet *const set = ", order="ps"),
    "part_top_ps": "    Part *part;\n" + HEAD + MS + loop(partdecl="part = ", order="ps"),
}}
B = BODIES[LP]
def alt(expr, top=False, order="sp"):
    t = loop(partdecl="part = " if top else "Part *part = ", order=order).replace("&m_parts[i];", expr + ";")
    return ("    Part *part;\n" if top else "") + HEAD + MS + t
B["get_plus"] = alt("m_parts.get() + i")
B["get_plus_top"] = alt("m_parts.get() + i", True)
B["get_idx"] = alt("&m_parts.get()[i]")
B["get_idx_top"] = alt("&m_parts.get()[i]", True)
B["get_idx_ps"] = alt("&m_parts.get()[i]", False, "ps")
B["parts_local"] = "    Part *part;\n" + HEAD + MS + "    Part *parts = m_parts.get();\n" + loop(partdecl="part = ").replace("&m_parts[i];", "&parts[i];")
B["set_ref"] = HEAD + MS + loop(setdecl="const BirdMotionSet &rset = ").replace("&table->m_sets[i];", "table->m_sets[i];\n        const BirdMotionSet *set = &rset;")
B["part_ref"] = HEAD + MS + loop(partdecl="Part &rpart = ").replace("&m_parts[i];", "m_parts[i];\n        Part *part = &rpart;")
B["raw"] = alt("*(Part **)&m_parts + i")
B["raw_idx"] = alt("&(*(Part **)&m_parts)[i]")
B["raw_top"] = alt("*(Part **)&m_parts + i", True)
B["raw_ps"] = alt("*(Part **)&m_parts + i", False, "ps")
B["raw_idx_ps"] = alt("&(*(Part **)&m_parts)[i]", False, "ps")
B["raw_all"] = alt("&(*(Part **)&m_parts)[i]", False, "ps").replace("m_parts[0].m_clump", "(*(Part **)&m_parts)[0].m_clump")
B["raw_all_sp"] = alt("&(*(Part **)&m_parts)[i]").replace("m_parts[0].m_clump", "(*(Part **)&m_parts)[0].m_clump")
L2 = loop(partdecl="part = ")
B["top_zero"] = "    Part *part = 0;\n" + HEAD + MS + L2
B["top_after_ms"] = HEAD + MS + "    Part *part;\n" + L2
B["top_volatile_use"] = "    Part *part;\n" + HEAD + MS + L2.replace("        SetupPart(slot, part, set);\n", "        SetupPart(slot, part, set);\n        part = 0;\n")
B["int_i_top"] = "    Part *part;\n" + HEAD + MS + "    int i;\n" + L2.replace("for (int i = 0;", "for (i = 0;")
B["all_top"] = "    Part *part;\n    void *manager;\n    UnkStage_8003A840 *stage;\n    int i;\n    const BirdMotionSet *set;\n" + HEAD + "    manager = fn_800122D4();\n    stage = fn_8003A840();\n" + loop(partdecl="part = ", setdecl="set = ").replace("for (int i = 0;", "for (i = 0;")
B["all_top_ps"] = "    Part *part;\n    void *manager;\n    UnkStage_8003A840 *stage;\n    int i;\n    const BirdMotionSet *set;\n" + HEAD + "    manager = fn_800122D4();\n    stage = fn_8003A840();\n" + loop(partdecl="part = ", setdecl="set = ", order="ps").replace("for (int i = 0;", "for (i = 0;")
B["while_loop"] = HEAD + MS + "    int i = 0;\n" + loop().replace("for (int i = 0; i < m_partCount; i++) {", "while (i < m_partCount) {").replace("        SetupPart(slot, part, set);\n", "        SetupPart(slot, part, set);\n        i++;\n")
B["count_local"] = HEAD + MS + loop().replace("part->m_attach = m_frame;", "part->m_attach = this->m_frame;").replace("Part *part = &m_parts[i];", "Part *part = &this->m_parts[i];")
