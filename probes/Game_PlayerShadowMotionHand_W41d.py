"""Wave 41: Load: retail reads the hand's animation before its clump (an inline function's
arguments, right to left) and colours the loader above the counter."""
SRC = open("src/Game/PlayerShadowMotionHand.cpp", encoding="utf-8").read()
SCORE = ["Load__Q36Player6Shadow14MotionHandImplFv"]
s = SRC.index("void MotionHandImpl::Load() {")
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
OLDSET = """            hand->m_clump = m_clumps[model];
            hand->m_anim = m_anims[model];
            if (hand->m_clump != 0 && hand->m_anim != 0) {
                hand->m_updater = new UnkClumpUpdater_80015A7C(hand->m_clump, hand->m_anim);
            } else {
                hand->m_updater = 0;
            }
"""
HSET = """inline void SetupHand_(MotionHandImpl::Hand *hand, RpClump *clump, _stMtnPlayWork *anim) {
    hand->m_clump = clump;
    hand->m_anim = anim;
    if (hand->m_clump != 0 && hand->m_anim != 0) {
        hand->m_updater = new UnkClumpUpdater_80015A7C(hand->m_clump, hand->m_anim);
    } else {
        hand->m_updater = 0;
    }
}

"""
NEWSET = "            SetupHand_(hand, m_clumps[model], m_anims[model]);\n"
V = {}
V["set"] = HSET + FN.replace(OLDSET, NEWSET)
V["set_locals"] = FN.replace(OLDSET, "            _stMtnPlayWork *anim = m_anims[model];\n            RpClump *clump = m_clumps[model];\n            hand->m_clump = clump;\n            hand->m_anim = anim;\n" + OLDSET[OLDSET.index("            if"):])
REPL = {}
for k, v in V.items():
    REPL[k] = [(FN, v)]
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + v + "#pragma opt_propagation reset\n")]
BASE = V["set"]
L1 = "    for (u32 i = 0; i < 5; i++) {\n"
L2 = "    for (int slot = 0; slot < 4; slot++) {\n        for (int i = 0; i < 2; i++) {\n"
def sub(t, pairs):
    for a, b in pairs:
        assert a in t, a
        t = t.replace(a, b, 1)
    return t
W = {}
W["i_top"] = sub(BASE, [("    void *loader = fn_800122D4();\n", "    u32 i;\n    void *loader = fn_800122D4();\n"), (L1, "    for (i = 0; i < 5; i++) {\n")])
W["i_after_loader"] = sub(BASE, [("    void *loader = fn_800122D4();\n", "    void *loader = fn_800122D4();\n    u32 i;\n"), (L1, "    for (i = 0; i < 5; i++) {\n")])
W["i_after_bones"] = sub(BASE, [("    u32 bones[2];\n", "    u32 bones[2];\n    u32 i;\n"), (L1, "    for (i = 0; i < 5; i++) {\n")])
W["shared_i"] = sub(BASE, [("    void *loader = fn_800122D4();\n", "    int i;\n    void *loader = fn_800122D4();\n"), (L1, "    for (i = 0; i < 5; i++) {\n"), (L2, "    for (int slot = 0; slot < 4; slot++) {\n        for (i = 0; i < 2; i++) {\n")])
W["shared_i_u"] = sub(BASE, [("    void *loader = fn_800122D4();\n", "    u32 i;\n    void *loader = fn_800122D4();\n"), (L1, "    for (i = 0; i < 5; i++) {\n"), (L2, "    for (int slot = 0; slot < 4; slot++) {\n        for (i = 0; i < 2; i++) {\n")])
W["loader_later"] = sub(BASE, [("    void *loader = fn_800122D4();\n    fn_80078C4C(m_player.get());\n", "    void *loader;\n    loader = fn_800122D4();\n    fn_80078C4C(m_player.get());\n")])
for k, v in W.items():
    REPL[k] = [(FN, v)]
