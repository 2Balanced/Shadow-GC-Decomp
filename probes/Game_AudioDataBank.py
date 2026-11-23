"""Wave 40: fn_801D852C's entry pointer and key (retail r5 and r6) with one of them read through
an inline function."""
SCORE = ["fn_801D852C__21UnkSoundBank_801D8890Fi"]
F = "UnkSoundBankEntry *UnkSoundBank_801D8890::fn_801D852C(int id) {"
HELP = """inline UnkSoundBankEntry *EntryAt_(UnkSoundBankGroup *g, int i) { return &g->m_entries[i]; }
inline s32 KeyOf_(int id) { return (u16)id; }
inline s32 IdOf_(UnkSoundBankEntry *e) { return e->m_id; }
"""
H = (F, HELP + F)
E1 = "        entry = &group->m_entries[i];\n"
K = "    s32 key = (u16)id;\n"
C = "        if (key == (s32)entry->m_id) {\n"
REPL = {
  "entry_inl": [H, (E1, "        entry = EntryAt_(group, i);\n")],
  "key_inl": [H, (K, "    s32 key = KeyOf_(id);\n")],
  "both_inl": [H, (E1, "        entry = EntryAt_(group, i);\n"), (K, "    s32 key = KeyOf_(id);\n")],
  "id_inl": [H, (C, "        if (key == IdOf_(entry)) {\n")],
  "entry_id_inl": [H, (E1, "        entry = EntryAt_(group, i);\n"), (C, "        if (key == IdOf_(entry)) {\n")],
  "key_in_cmp": [H, (K, ""), (C, "        if (KeyOf_(id) == (s32)entry->m_id) {\n")],
}
