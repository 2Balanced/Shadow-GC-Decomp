"""Wave 40: fn_801D852C's loop registers (retail: offset r4, entry r5, key r6, found r7)."""
SCORE = ["fn_801D852C__21UnkSoundBank_801D8890Fi"]
B = """    s32 key = (u16)id;
    UnkSoundBankEntry *entry;
    UnkSoundBankEntry *found = 0;
    for (int i = 0; i < group->m_count; i++) {
        entry = &group->m_entries[i];
        if (key == (s32)entry->m_id) {
            found = entry;
            break;
        }
    }
"""
def v(decls, loop_pre=""):
    return [(B, decls + loop_pre + """    for (int i = 0; i < group->m_count; i++) {
        entry = &group->m_entries[i];
        if (key == (s32)entry->m_id) {
            found = entry;
            break;
        }
    }
""")]
REPL = {
  "e_k_f": v("    UnkSoundBankEntry *entry;\n    s32 key = (u16)id;\n    UnkSoundBankEntry *found = 0;\n"),
  "e_k_f_late": v("    UnkSoundBankEntry *entry;\n    s32 key;\n    UnkSoundBankEntry *found;\n", "    key = (u16)id;\n    found = 0;\n"),
  "e_k_f_late2": v("    UnkSoundBankEntry *entry;\n    s32 key;\n    UnkSoundBankEntry *found;\n", "    found = 0;\n    key = (u16)id;\n"),
  "e_f_k_late": v("    UnkSoundBankEntry *entry;\n    UnkSoundBankEntry *found;\n    s32 key;\n", "    key = (u16)id;\n    found = 0;\n"),
  "k_late_first": v("    s32 key;\n    UnkSoundBankEntry *entry;\n    UnkSoundBankEntry *found;\n", "    found = 0;\n    key = (u16)id;\n"),
  "u16key_e": v("    UnkSoundBankEntry *entry;\n    u16 key = id;\n    UnkSoundBankEntry *found = 0;\n"),
}
