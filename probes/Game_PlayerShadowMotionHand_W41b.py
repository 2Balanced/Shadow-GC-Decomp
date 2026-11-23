"""Wave 41: SetSlot: retail stores the two zero times as floats and copies the motion to a saved
register after the null test."""
SRC = open("src/Game/PlayerShadowMotionHand.cpp", encoding="utf-8").read()
SCORE = ["SetSlot__Q36Player6Shadow14MotionHandImplFi"]
s = SRC.index("void MotionHandImpl::SetSlot(int slot) {")
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
T0 = "                float time[2] = {0.0f, 0.0f};\n"
V = {
  "t_assign": FN.replace(T0, "                float time[2];\n                time[0] = 0.0f;\n                time[1] = 0.0f;\n"),
  "t_chain": FN.replace(T0, "                float time[2];\n                time[0] = time[1] = 0.0f;\n"),
  "t_assign_after": FN.replace(T0, "").replace("                const UnkMotionNode_800AA760 *nodes[2];\n", "                float time[2];\n                const UnkMotionNode_800AA760 *nodes[2];\n                time[0] = 0.0f;\n                time[1] = 0.0f;\n"),
  "t_orig": FN,
}
REPL = {}
for k, v in V.items():
    REPL[k] = [(FN, v)]
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + v + "#pragma opt_propagation reset\n")]
