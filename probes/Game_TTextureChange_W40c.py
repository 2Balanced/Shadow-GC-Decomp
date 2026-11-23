"""TTextureChange::Setup (wave 40, round 3): the frame's byte offset as a variable of its own that
starts as a copy of the counter (retail: `li r22,0` ... `mr r23,r22`, then `add r4,r5,r23` and a
load at +80)."""
H = "    m_count = 0;\n    m_time = 0.0f;\n"
D = "        RwTexDictionary *dict = fn_8048D0D8();\n"
E = "        UnkTextureChangeEntry_8004F0A4 *entry = &m_entries[m_count];\n"
F = "    for (s32 i = 0; i < count; i++) {\n"
J = "                for (s32 j = 0; j < entry->m_anim->m_count; j++) {\n"
S = "                    sprintf(name, \"%s.%d\", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);\n"
X = "                    entry->m_textures[j] = fn_8048CFFC(dict, name);\n"
W = "for (int n = 0; n < 1; n++) {\n"
TOP = "    RwTexDictionary *dict;\n    UnkTextureChangeEntry_8004F0A4 *entry;\n"
JTOP = [(H, TOP + "    s32 i;\n    s32 j;\n" + H), (D, "        dict = fn_8048D0D8();\n"), (E, "        entry = &m_entries[m_count];\n"), (F, "    for (i = 0; i < count; i++) {\n")]
JF = "                for (j = 0; j < entry->m_anim->m_count; j++) {\n"
FR = "((UnkTextureFrame_8004F0A4 *)((u8 *)entry->m_anim->m_frames + offset))->m_index"
S2 = "                    sprintf(name, \"%s.%d\", entry->m_anim->m_name, " + FR + ");\n"
REPL = {
 "off_copy": JTOP + [(J, "                s32 offset;\n                for (j = 0, offset = j; j < entry->m_anim->m_count; j++) {\n"), (S, S2), (X, X + "                    offset += 8;\n")],
 "off_copy_for": JTOP + [(J, "                s32 offset;\n                j = 0;\n                " + W + "                    offset = j;\n                }\n                for (; j < entry->m_anim->m_count; j++) {\n"), (S, S2), (X, X + "                    offset += 8;\n")],
 "off_zero": JTOP + [(J, "                s32 offset = 0;\n" + JF), (S, S2), (X, X + "                    offset += 8;\n")],
 "k_copy": JTOP + [(J, "                s32 k;\n                for (j = 0, k = j; j < entry->m_anim->m_count; j++, k++) {\n"), (S, S.replace("m_frames[j]", "m_frames[k]"))],
 "k_copy_for": JTOP + [(J, "                s32 k;\n                j = 0;\n                " + W + "                    k = j;\n                }\n                for (; j < entry->m_anim->m_count; j++, k++) {\n"), (S, S.replace("m_frames[j]", "m_frames[k]"))],
 "k_zero_for": JTOP + [(J, "                s32 k;\n                " + W + "                    k = 0;\n                }\n" + JF), (S, S.replace("m_frames[j]", "m_frames[k]")), (X, X + "                    k++;\n")],
 "frame_ptr": JTOP + [(J, JF), (S, S.replace("entry->m_anim->m_frames[j].m_index", "(entry->m_anim->m_frames + j)->m_index"))],
 "frame_ptr2": JTOP + [(J, JF), (S, S.replace("entry->m_anim->m_frames[j].m_index", "(&entry->m_anim->m_frames[0])[j].m_index"))],
}
SCORE = ["Setup__14TTextureChangeFP30UnkTextureChangeParam_8004F22Cl"]
