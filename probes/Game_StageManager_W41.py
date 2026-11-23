"""Wave 41: OnStageDataLoaded with copy propagation off and named copies (the wave 40 lever)."""
SCORE = ["OnStageDataLoaded__12StageManagerFP12StageManager"]
SRC = open("src/Game/StageManager.cpp", encoding="utf-8").read()
s = SRC.index("void StageManager::OnStageDataLoaded(StageManager *manager) {")
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
def sub(text, pairs):
    for a, b in pairs:
        assert a in text, a
        text = text.replace(a, b)
    return text
V = {}
V["plain"] = FN
V["noentry"] = sub(FN, [("        UnkArchiveEntry_8004C358 *entry = header->GetEntry(i);\n", ""), ("entry->", "header->m_entries[i].")])
V["self"] = sub(FN, [("    UnkArchiveHeader_8004C358 *header = s_archive->m_header;\n", "    StageManager *self = manager;\n    UnkArchiveHeader_8004C358 *header = s_archive->m_header;\n"), ("manager->m_loadPath", "self->m_loadPath")])
V["hdrcopy"] = sub(FN, [("    for (int i = 2; i < header->m_count + 2; i++) {\n        UnkArchiveEntry_8004C358 *entry = header->GetEntry(i);",
    "    for (int i = 2; i < header->m_count + 2; i++) {\n        UnkArchiveHeader_8004C358 *h = header;\n        UnkArchiveEntry_8004C358 *entry = h->GetEntry(i);")])
V["entry_first"] = sub(FN, [("    UnkArchiveHeader_8004C358 *header = s_archive->m_header;\n", "    UnkArchiveEntry_8004C358 *entry;\n    UnkArchiveHeader_8004C358 *header = s_archive->m_header;\n"),
    ("        UnkArchiveEntry_8004C358 *entry = header->GetEntry(i);", "        entry = header->GetEntry(i);")])
V["i_first"] = sub(FN, [("    UnkArchiveHeader_8004C358 *header = s_archive->m_header;\n    for (int i = 2;", "    int i;\n    UnkArchiveHeader_8004C358 *header = s_archive->m_header;\n    for (i = 2;")])
V["bytes"] = sub(FN, [("        UnkArchiveEntry_8004C358 *entry = header->GetEntry(i);", "        UnkArchiveEntry_8004C358 *entry = (UnkArchiveEntry_8004C358 *)((u8 *)header + i * 0x38 + 0x34);")])
V["bytes2"] = sub(FN, [("        UnkArchiveEntry_8004C358 *entry = header->GetEntry(i);", "        UnkArchiveEntry_8004C358 *entry = (UnkArchiveEntry_8004C358 *)((u8 *)header + i * 0x38);\n        entry = (UnkArchiveEntry_8004C358 *)((u8 *)entry + 0x34);")])
REPL = {}
for k, b in V.items():
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + b + "#pragma opt_propagation reset\n")]
    if k != "plain":
        REPL[k] = [(FN, b)]
