"""Wave 41: Load, the loader and the first loop in a block of their own (scopes)."""
SRC = open("src/Game/PlayerShadowMotionHand.cpp", encoding="utf-8").read()
SCORE = ["Load__Q36Player6Shadow14MotionHandImplFv"]
s = SRC.index("void MotionHandImpl::Load() {")
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
HEAD = "void MotionHandImpl::Load() {\n"
L2 = FN[FN.index("    for (int slot = 0;"):FN.rindex("}")]
PART1 = FN[len(HEAD):FN.index("    for (int slot = 0;")]
def indent(t):
    return "".join(("    " + l if l.strip() else l) for l in t.splitlines(True))
V = {}
V["block1"] = HEAD + "    {\n" + indent(PART1) + "    }\n" + L2 + "}\n"
V["block1_fori"] = HEAD + "    {\n" + indent(PART1.replace("    u32 i;\n", "").replace("    for (i = 0; i < 5; i++) {", "    for (u32 i = 0; i < 5; i++) {")) + "    }\n" + L2 + "}\n"
V["block2"] = HEAD + PART1 + "    {\n" + indent(L2) + "    }\n}\n"
REPL = {}
for k, v in V.items():
    REPL[k] = [(FN, v)]
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + v + "#pragma opt_propagation reset\n")]
