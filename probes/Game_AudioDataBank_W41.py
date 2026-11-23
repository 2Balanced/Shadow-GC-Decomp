"""Wave 41: fn_801D852C (retail: key r6, entry r5) under '#pragma opt_propagation off', locals in
every declaration order and the key in several types."""
import itertools
SCORE = ["fn_801D852C__21UnkSoundBank_801D8890Fi"]
H = "UnkSoundBankEntry *UnkSoundBank_801D8890::fn_801D852C(int id) {\n"
B = """    UnkSoundBankGroup *group = GetGroup(id);
    if (group == 0) {
        return 0;
    }
    s32 key = (u16)id;
    UnkSoundBankEntry *entry;
    UnkSoundBankEntry *found = 0;
    for (int i = 0; i < group->m_count; i++) {
        entry = &group->m_entries[i];
        if (key == (s32)entry->m_id) {
"""
P0 = "#pragma opt_propagation off\n"
DECL = {"key": "    s32 key = (u16)id;\n", "entry": "    UnkSoundBankEntry *entry;\n", "found": "    UnkSoundBankEntry *found = 0;\n"}
HEAD = """    UnkSoundBankGroup *group = GetGroup(id);
    if (group == 0) {
        return 0;
    }
"""
LOOP = """    for (int i = 0; i < group->m_count; i++) {
        entry = &group->m_entries[i];
        if (key == (s32)entry->m_id) {
"""
REPL = {}
for perm in itertools.permutations(["key", "entry", "found"]):
    body = HEAD + "".join(DECL[p] for p in perm) + LOOP
    REPL["prag_" + "_".join(perm)] = [(H + B, P0 + H + body)]
REPL["prag_u16key"] = [(H + B, P0 + H + B.replace("    s32 key = (u16)id;\n", "    u16 key = id;\n").replace("if (key == (s32)entry->m_id)", "if (key == entry->m_id)"))]
REPL["prag_key_in_loop_decl_entry"] = [(H + B, P0 + H + HEAD + "    s32 key = (u16)id;\n    UnkSoundBankEntry *found = 0;\n" + LOOP.replace("        entry = &group", "        UnkSoundBankEntry *entry = &group"))]
REPL["prag_idcopy"] = [(H + B, P0 + H + B.replace("    s32 key = (u16)id;\n", "    int copy = id;\n    s32 key = (u16)copy;\n"))]
