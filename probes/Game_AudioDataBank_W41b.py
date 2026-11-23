"""Wave 41: fn_801D852C: the masked id in the comparison (hoisted by the optimizer)."""
SCORE = ["fn_801D852C__21UnkSoundBank_801D8890Fi"]
H = "UnkSoundBankEntry *UnkSoundBank_801D8890::fn_801D852C(int id) {\n"
B = """    s32 key = (u16)id;
    UnkSoundBankEntry *entry;
    UnkSoundBankEntry *found = 0;
    for (int i = 0; i < group->m_count; i++) {
        entry = &group->m_entries[i];
        if (key == (s32)entry->m_id) {
"""
P0 = "#pragma opt_propagation off\n"
def v(cmp, prag=False):
    body = B.replace("    s32 key = (u16)id;\n", "").replace("if (key == (s32)entry->m_id) {", "if (" + cmp + ") {")
    r = [(B, body)]
    if prag:
        r.append((H, P0 + H))
    return r
REPL = {
  "a": v("(s32)(u16)id == (s32)entry->m_id"),
  "b": v("(u16)id == (s32)entry->m_id"),
  "a_prag": v("(s32)(u16)id == (s32)entry->m_id", True),
  "c": v("(s32)entry->m_id == (u16)id"),
  "c_prag": v("(s32)entry->m_id == (u16)id", True),
  "d_prag": v("(u16)id == (s32)entry->m_id", True),
}
