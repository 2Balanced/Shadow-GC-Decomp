"""TTextureChange::Setup (wave 40): the dictionary and the entry declared ahead of the counter
(saved registers go to the locals in declaration order), the inner counter's offset."""
H = "    m_count = 0;\n    m_time = 0.0f;\n"
D = "        RwTexDictionary *dict = fn_8048D0D8();\n"
E = "        UnkTextureChangeEntry_8004F0A4 *entry = &m_entries[m_count];\n"
P = "        UnkTextureChangeParam_8004F22C *param = &params[i];\n"
F = "    for (s32 i = 0; i < count; i++) {\n"
REPL = {
 "dict_top": [(H, "    RwTexDictionary *dict;\n" + H), (D, "        dict = fn_8048D0D8();\n")],
 "dict_entry_top": [(H, "    RwTexDictionary *dict;\n    UnkTextureChangeEntry_8004F0A4 *entry;\n" + H), (D, "        dict = fn_8048D0D8();\n"), (E, "        entry = &m_entries[m_count];\n")],
 "dict_entry_i_top": [(H, "    RwTexDictionary *dict;\n    UnkTextureChangeEntry_8004F0A4 *entry;\n    s32 i;\n" + H), (D, "        dict = fn_8048D0D8();\n"), (E, "        entry = &m_entries[m_count];\n"), (F, "    for (i = 0; i < count; i++) {\n")],
 "all_top": [(H, "    RwTexDictionary *dict;\n    UnkTextureChangeEntry_8004F0A4 *entry;\n    s32 i;\n    UnkTextureChangeParam_8004F22C *param;\n" + H), (D, "        dict = fn_8048D0D8();\n"), (E, "        entry = &m_entries[m_count];\n"), (F, "    for (i = 0; i < count; i++) {\n"), (P, "        param = &params[i];\n")],
 "dict_first_in_loop": [(P + E, "        RwTexDictionary *dict;\n" + P + E), (D, "        dict = fn_8048D0D8();\n")],
 "dict_entry_in_loop": [(P + E, "        RwTexDictionary *dict;\n        UnkTextureChangeEntry_8004F0A4 *entry;\n" + P + "        entry = &m_entries[m_count];\n"), (D, "        dict = fn_8048D0D8();\n")],
}
SCORE = ["Setup__14TTextureChangeFP30UnkTextureChangeParam_8004F22Cl"]
