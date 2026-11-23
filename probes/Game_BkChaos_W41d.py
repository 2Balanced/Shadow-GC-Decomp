"""Wave 41: StretchCallback under propagation off: index forms without the bool's clrlwi."""
SCORE = ["StretchCallback__7BkChaosFP19_stMtnCallbackParam"]
H = "static void StretchCallback(_stMtnCallbackParam *param) {\n"
P0 = "#pragma opt_propagation off\n"
I0 = "    int index = param->m_id == 0x1774 ? 0 : 1;\n"
def v(idx):
    return [(H, P0 + H), (I0, idx)]
REPL = {
  "int_cast": v("    u32 index = (int)(param->m_id != 0x1774);\n"),
  "if_set1": v("    u32 index = 1;\n    if (param->m_id == 0x1774) {\n        index = 0;\n    }\n"),
  "if_set0": v("    u32 index = 0;\n    if (param->m_id != 0x1774) {\n        index = 1;\n    }\n"),
  "tern_u": v("    u32 index = param->m_id == 0x1774 ? 0u : 1u;\n"),
  "tern_local_id": v("    s32 id = param->m_id;\n    u32 index = id == 0x1774 ? 0 : 1;\n"),
  "ne_local_id": v("    s32 id = param->m_id;\n    u32 index = id != 0x1774;\n"),
  "tern_idx_decl_first": [(H, P0 + H + "    u32 index;\n"), (I0, "    index = param->m_id == 0x1774 ? 0 : 1;\n")],
}
