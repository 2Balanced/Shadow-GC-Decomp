"""Wave 41: StretchCallback (retail: param r31, index r27 unsigned; pitch straight into f29)."""
SCORE = ["StretchCallback__7BkChaosFP19_stMtnCallbackParam"]
H = "static void StretchCallback(_stMtnCallbackParam *param) {\n"
P0 = "#pragma opt_propagation off\n"
I0 = "    int index = param->m_id == 0x1774 ? 0 : 1;\n"
REPL = {
  "prag": [(H, P0 + H)],
  "prag_u32tern": [(H, P0 + H), (I0, "    u32 index = param->m_id == 0x1774 ? 0 : 1;\n")],
  "prag_u32ne": [(H, P0 + H), (I0, "    u32 index = param->m_id != 0x1774;\n")],
  "u32tern": [(I0, "    u32 index = param->m_id == 0x1774 ? 0 : 1;\n")],
  "prag_pcopy_u32tern": [(H, P0 + H + "    _stMtnCallbackParam *p = param;\n"), (I0, "    u32 index = p->m_id == 0x1774 ? 0 : 1;\n")],
  "prag_u32tern_angles": [(H, P0 + H), (I0, "    u32 index = param->m_id == 0x1774 ? 0 : 1;\n")],
}
