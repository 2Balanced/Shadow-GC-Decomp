"""Wave 40: fn_801D852C's search as an inline finder."""
SCORE = ["fn_801D852C__21UnkSoundBank_801D8890Fi"]
F = "UnkSoundBankEntry *UnkSoundBank_801D8890::fn_801D852C(int id) {"
BODY = """    s32 key = (u16)id;
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
def fnd(sig, key, decl="    UnkSoundBankEntry *entry;\n    UnkSoundBankEntry *found = 0;\n", cmp="key == (s32)entry->m_id", pre=""):
    return "inline UnkSoundBankEntry *Find_(" + sig + ") {\n" + pre + decl + """    for (int i = 0; i < group->m_count; i++) {
        entry = &group->m_entries[i];
        if (""" + cmp + """) {
            found = entry;
            break;
        }
    }
    return found;
}
"""
def v(sig, call, **kw):
    return [(F, fnd(sig, None, **kw) + F), (BODY, "    UnkSoundBankEntry *found = " + call + ";\n")]
REPL = {
  "f_s32": v("UnkSoundBankGroup *group, s32 key", "Find_(group, (u16)id)"),
  "f_u16": v("UnkSoundBankGroup *group, u16 key", "Find_(group, id)"),
  "f_u16_cast": v("UnkSoundBankGroup *group, u16 key", "Find_(group, (u16)id)"),
  "f_int_mask": v("UnkSoundBankGroup *group, int id", "Find_(group, id)", pre="    s32 key = (u16)id;\n"),
  "f_found_first": v("UnkSoundBankGroup *group, s32 key", "Find_(group, (u16)id)", decl="    UnkSoundBankEntry *found = 0;\n    UnkSoundBankEntry *entry;\n"),
  "f_key_first": v("s32 key, UnkSoundBankGroup *group", "Find_((u16)id, group)"),
  "f_u32": v("UnkSoundBankGroup *group, u32 key", "Find_(group, (u16)id)", cmp="key == entry->m_id"),
}
