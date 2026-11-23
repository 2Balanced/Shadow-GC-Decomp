"""Wave 41: BossCtrlUnit_StageObject::Tick, declaration positions of the hits pointer(s)."""
SCORE = ["Tick__24BossCtrlUnit_StageObjectFf"]
OLD = open("src/Game/BossCtrlUnit.cpp", encoding="utf-8").read()
s = OLD.index("int BossCtrlUnit_StageObject::Tick(float dt) {")
e = OLD.index("    return false;\n}\n", s) + len("    return false;\n}\n")
FN = OLD[s:e]

def sub(text, pairs):
    for a, b in pairs:
        assert a in text, a
        text = text.replace(a, b, 1)
    return text

POS = {
    "p0": "int BossCtrlUnit_StageObject::Tick(float dt) {\n",
    "p1": "    bool hidden = false;\n",
    "p2": "    int index = fn_800623A8(fn_8000DCC0());\n",
    "p3": "    Vec3f camera;\n",
    "p4": "    boost::shared_ptr<CharInfo> chara = player.lock();\n",
    "p5": "    Vec3f target;\n",
    "p6": "    void *world = fn_80020D40();\n",
    "p7": "    UnkLine_80020A20 line;\n",
}
DEF1 = ("    UnkLineHits_80021FE4 *hits = fn_80021FE4(world, &line, 1, 6, 0, 0);",
        "    hits = fn_80021FE4(world, &line, 1, 6, 0, 0);")
MERGE = [
    ("        UnkLineHits_80021FE4 *backHits = fn_80021FE4(world, &back, 1, 6, 0, 0);",
     "        hits = fn_80021FE4(world, &back, 1, 6, 0, 0);"),
    ("        if (backHits != 0) {", "        if (hits != 0) {"),
    ("HitObject(backHits)", "HitObject(hits)"),
    ("            fn_80026FB4(backHits, 1);", "            fn_80026FB4(hits, 1);"),
]
BDEF = ("        UnkLineHits_80021FE4 *backHits = fn_80021FE4(world, &back, 1, 6, 0, 0);",
        "        backHits = fn_80021FE4(world, &back, 1, 6, 0, 0);")
V = {}
for k, anchor in POS.items():
    V["one_" + k] = sub(FN, [DEF1] + MERGE + [(anchor, anchor + "    UnkLineHits_80021FE4 *hits;\n")])
    V["two_" + k] = sub(FN, [DEF1, BDEF, (anchor, anchor + "    UnkLineHits_80021FE4 *hits;\n    UnkLineHits_80021FE4 *backHits;\n")])
REPL = {k: [(FN, v)] for k, v in V.items()}
