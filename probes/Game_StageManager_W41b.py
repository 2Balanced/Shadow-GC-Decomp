"""Wave 41: OnStageDataLoaded, declaration orders of header, block and entry under copy propagation off."""
SCORE = ["OnStageDataLoaded__12StageManagerFP12StageManager"]
SRC = open("src/Game/StageManager.cpp", encoding="utf-8").read()
s = SRC.index("void StageManager::OnStageDataLoaded(StageManager *manager) {")
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
H = "    UnkArchiveHeader_8004C358 *header = s_archive->m_header;\n"
EN = "        UnkArchiveEntry_8004C358 *entry = header->GetEntry(i);\n"
BL = "            void *block;\n"
def sub(t, pairs):
    for a, b in pairs:
        assert a in t, a[:50]
        t = t.replace(a, b, 1)
    return t
V = {}
V["hbe"] = sub(FN, [(H, H + "    void *block;\n    UnkArchiveEntry_8004C358 *entry;\n"), (EN, "        entry = header->GetEntry(i);\n"), (BL, "")])
V["bhe"] = sub(FN, [(H, "    void *block;\n" + H + "    UnkArchiveEntry_8004C358 *entry;\n"), (EN, "        entry = header->GetEntry(i);\n"), (BL, "")])
V["hb"] = sub(FN, [(H, H + "    void *block;\n"), (BL, "")])
V["he_b"] = sub(FN, [(H, H + "    UnkArchiveEntry_8004C358 *entry;\n    void *block;\n"), (EN, "        entry = header->GetEntry(i);\n"), (BL, "")])
V["eb_inner"] = sub(FN, [(EN, "        void *block;\n" + EN), (BL, "")])
REPL = {}
for k, v in V.items():
    REPL[k] = [(FN, v)]
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + v + "#pragma opt_propagation reset\n")]
