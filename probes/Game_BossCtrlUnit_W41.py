"""Wave 41: BossCtrlUnit_StageObject::Tick. Retail gives both line tests' hits and the area's world
one register (r27) and both clumps r28: shapes where they are one variable."""
SCORE = ["Tick__24BossCtrlUnit_StageObjectFf"]
OLD = open("src/Game/BossCtrlUnit.cpp", encoding="utf-8").read()
s = OLD.index("int BossCtrlUnit_StageObject::Tick(float dt) {")
e = OLD.index("    return false;\n}\n", s) + len("    return false;\n}\n")
FN = OLD[s:e]

def sub(text, pairs):
    for a, b in pairs:
        assert a in text, a
        text = text.replace(a, b)
    return text

HITS_ONE = [
    ("        UnkLineHits_80021FE4 *backHits = fn_80021FE4(world, &back, 1, 6, 0, 0);",
     "        hits = fn_80021FE4(world, &back, 1, 6, 0, 0);"),
    ("        if (backHits != 0) {", "        if (hits != 0) {"),
    ("HitObject(backHits)", "HitObject(hits)"),
    ("            fn_80026FB4(backHits, 1);", "            fn_80026FB4(hits, 1);"),
]
CLUMP_ONE = [
    ("            void *current = fn_8046256C(m_clumps[i]);\n",
     "            void *current = fn_8046256C(m_clumps[i]);\n            RpClump *clump;\n"),
    ("                    RpClump *clump = m_clumps[i];", "                    clump = m_clumps[i];"),
    ("                RpClump *clump = m_clumps[i];", "                clump = m_clumps[i];"),
]
AW_HITS = [
    ("    void *areaWorld;\n", ""),
    ("            areaWorld = lbl_8051D208[area].m_world;", "            hits = (UnkLineHits_80021FE4 *)lbl_8051D208[area].m_world;"),
    ("areaWorld == current", "(void *)hits == current"),
    ("areaWorld != current", "(void *)hits != current"),
    ("SetClumpWorld(clump, areaWorld);", "SetClumpWorld(clump, hits);"),
]
V = {}
V["hits1"] = sub(FN, HITS_ONE)
V["clump1"] = sub(FN, CLUMP_ONE)
V["hits1_clump1"] = sub(FN, HITS_ONE + CLUMP_ONE)
V["all1"] = sub(FN, HITS_ONE + CLUMP_ONE + AW_HITS)
V["hitsaw"] = sub(FN, HITS_ONE + AW_HITS)
REPL = {k: [(FN, v)] for k, v in V.items()}
for k, v in list(V.items()):
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + v + "#pragma opt_propagation reset\n")]
