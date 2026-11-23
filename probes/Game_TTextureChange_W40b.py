"""TTextureChange::Setup (wave 40, round 2): on top of the dictionary and the entry declared first,
the inner counter (retail keeps it in the parameter's register, r22, and copies its zero into the
frame offset: `li r22,0` ... `mr r23,r22`)."""
H = "    m_count = 0;\n    m_time = 0.0f;\n"
D = "        RwTexDictionary *dict = fn_8048D0D8();\n"
E = "        UnkTextureChangeEntry_8004F0A4 *entry = &m_entries[m_count];\n"
F = "    for (s32 i = 0; i < count; i++) {\n"
J = "                for (s32 j = 0; j < entry->m_anim->m_count; j++) {\n"
S = "                    sprintf(name, \"%s.%d\", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);\n"
X = "                    entry->m_textures[j] = fn_8048CFFC(dict, name);\n"
W = "for (int n = 0; n < 1; n++) {\n"
TOP = "    RwTexDictionary *dict;\n    UnkTextureChangeEntry_8004F0A4 *entry;\n"
BASE = [(H, TOP + H), (D, "        dict = fn_8048D0D8();\n"), (E, "        entry = &m_entries[m_count];\n")]
JTOP = [(H, TOP + "    s32 i;\n    s32 j;\n" + H), (D, "        dict = fn_8048D0D8();\n"), (E, "        entry = &m_entries[m_count];\n"), (F, "    for (i = 0; i < count; i++) {\n")]
REPL = {
 "j_for": BASE + [(J, "                s32 j;\n                " + W + "                    j = 0;\n                }\n                for (; j < entry->m_anim->m_count; j++) {\n")],
 "j_top": JTOP + [(J, "                for (j = 0; j < entry->m_anim->m_count; j++) {\n")],
 "j_top_for": JTOP + [(J, "                " + W + "                    j = 0;\n                }\n                for (; j < entry->m_anim->m_count; j++) {\n")],
 "j_before": BASE + [(J, "                s32 j = 0;\n                for (; j < entry->m_anim->m_count; j++) {\n")],
 "j_while": BASE + [(J, "                s32 j = 0;\n                while (j < entry->m_anim->m_count) {\n"), (X, X + "                    j++;\n")],
 "frame_local": BASE + [(S, "                    UnkTextureFrame_8004F0A4 *frame = &entry->m_anim->m_frames[j];\n                    sprintf(name, \"%s.%d\", entry->m_anim->m_name, frame->m_index);\n")],
 "anim_local": BASE + [(S, "                    UnkTextureAnim_8004F0A4 *anim = entry->m_anim;\n                    sprintf(name, \"%s.%d\", anim->m_name, anim->m_frames[j].m_index);\n")],
 "anim_for": BASE + [(S, "                    UnkTextureAnim_8004F0A4 *anim;\n                    " + W + "                        anim = entry->m_anim;\n                    }\n                    sprintf(name, \"%s.%d\", anim->m_name, anim->m_frames[j].m_index);\n")],
 "sprintf_for": BASE + [(S, "                    " + W + "    " + S + "                    }\n")],
 "body_for": BASE + [(S + X, "                    " + W + "    " + S + "    " + X + "                    }\n")],
 "index_for": BASE + [(S, "                    s32 index;\n                    " + W + "                        index = entry->m_anim->m_frames[j].m_index;\n                    }\n                    sprintf(name, \"%s.%d\", entry->m_anim->m_name, index);\n")],
}
SCORE = ["Setup__14TTextureChangeFP30UnkTextureChangeParam_8004F22Cl"]
