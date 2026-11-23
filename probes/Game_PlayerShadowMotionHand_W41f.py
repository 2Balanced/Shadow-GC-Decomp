"""Wave 41: Load, the loops' variables (retail: loader r29, counter r30, address r23)."""
SRC = open("src/Game/PlayerShadowMotionHand.cpp", encoding="utf-8").read()
SCORE = ["Load__Q36Player6Shadow14MotionHandImplFv"]
s = SRC.index("void MotionHandImpl::Load() {")
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
L2 = FN[FN.index("    for (int slot = 0;"):FN.rindex("}")]
L2F = ("inline void SetupHands_(MotionHandImpl *impl) {\n"
       + L2.replace("&m_hands", "&impl->m_hands")
           .replace("m_clumps[model], m_anims[model]", "impl->m_clumps[model], impl->m_anims[model]")
           .replace("            Hand *hand", "            MotionHandImpl::Hand *hand")
       + "}\n\n")
V = {}
V["l2inline"] = L2F + FN.replace(L2, "    SetupHands_(this);\n")
V["slot_top"] = FN.replace("    u32 i;\n", "    u32 i;\n    int slot;\n").replace("    for (int slot = 0;", "    for (slot = 0;")
V["slot_first"] = FN.replace("    u32 i;\n", "    int slot;\n    u32 i;\n").replace("    for (int slot = 0;", "    for (slot = 0;")
V["hand_top"] = FN.replace("    u32 i;\n", "    u32 i;\n    Hand *hand;\n").replace("            Hand *hand = &m_hands", "            hand = &m_hands")
V["l1_u32_int"] = FN.replace("    u32 i;\n", "    int i;\n")
L1 = FN[FN.index("    for (i = 0; i < 5; i++) {"):FN.index("    for (int slot = 0;")]
L1BODY = L1[L1.index("{\n") + 2:]
L1BODY = (L1BODY.replace("m_anims", "impl->m_anims").replace("m_clumps", "impl->m_clumps")
          .replace("m_files", "impl->m_files").replace("m_player", "impl->m_player"))
L1F = ("inline void LoadModels_(MotionHandImpl *impl, void *loader, const u32 *bones) {\n"
       "    for (u32 i = 0; i < 5; i++) {\n" + L1BODY + "}\n\n")
V["l1inline"] = L1F + FN.replace(L1, "    LoadModels_(this, loader, bones);\n").replace("    u32 i;\n", "")
V["l1l2inline"] = L1F + L2F + FN.replace(L1, "    LoadModels_(this, loader, bones);\n").replace("    u32 i;\n", "").replace(L2, "    SetupHands_(this);\n")
REPL = {k: [(FN, v)] for k, v in V.items()}
