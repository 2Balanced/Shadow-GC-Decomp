"""Wave 41: StretchCallback: the index through an inline function / late assignment, propagation off."""
SCORE = ["StretchCallback__7BkChaosFP19_stMtnCallbackParam"]
H = "static void StretchCallback(_stMtnCallbackParam *param) {\n"
P0 = "#pragma opt_propagation off\n"
I0 = "    int index = param->m_id == 0x1774 ? 0 : 1;\n"
INL = "inline u32 Side_(_stMtnCallbackParam *p) {\n    return p->m_id == 0x1774 ? 0 : 1;\n}\n"
INL2 = "inline u32 Side_(const _stMtnCallbackParam *p) {\n    return p->m_id != 0x1774 ? 1 : 0;\n}\n"
REPL = {
  "inl_tern": [(H, INL + P0 + H), (I0, "    u32 index = Side_(param);\n")],
  "inl_tern_const": [(H, INL2 + P0 + H), (I0, "    u32 index = Side_(param);\n")],
  "inl_tern_noprag": [(H, INL + H), (I0, "    u32 index = Side_(param);\n")],
  "top_decl_tern": [(H, P0 + H + "    u32 index;\n"), (I0, "    index = param->m_id != 0x1774 ? 1 : 0;\n")],
  "sub_form": [(H, P0 + H), (I0, "    u32 index = 1 - (param->m_id == 0x1774);\n")],
}
