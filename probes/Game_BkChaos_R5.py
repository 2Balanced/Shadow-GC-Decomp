"""Wave 40: StretchCallback's node index, round 2: an unsigned index (cmplwi) declared at other
points (the ternary gives subfe without clrlwi but swaps index and parameter registers)."""
SCORE = ["StretchCallback__7BkChaosFP19_stMtnCallbackParam"]
I = "    int index = param->m_id == 0x1774 ? 0 : 1;\n"
T = "    index = param->m_id == 0x1774 ? 0u : 1u;\n"
C = "    BkChaos *chaos = static_cast<BkChaos *>(param->m_data);\n"
M = "    Mtx34f matrix;\n"
V = "    Vec3f toTarget;\n"
D = "    float distance = fn_8040CF50(&toTarget);\n"
REPL = {
  "decl_first": [(C, "    u32 index;\n" + C), (I, T)],
  "decl_after_chaos": [(C, C + "    u32 index;\n"), (I, T)],
  "decl_after_matrix": [(M, M + "    u32 index;\n"), (I, T)],
  "decl_after_target": [(V, V + "    u32 index;\n"), (I, T)],
  "decl_after_dist": [(D, D + "    u32 index;\n"), (I, T)],
  "cast_tern": [(I, "    u32 index = (u32)(param->m_id != 0x1774 ? 1 : 0);\n")],
  "id_local": [(I, "    s32 id = param->m_id;\n    u32 index = id == 0x1774 ? 0u : 1u;\n")],
}
