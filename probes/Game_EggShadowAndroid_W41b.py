"""Wave 41: vfDisp_0x64 (retail: this r31, loader r30, model r29): this as a local through an
inline function's parameter, with and without propagation."""
SCORE = ["vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv"]
H = "void EggShadowAndroid::vfDisp_0x64() {\n"
B = """    void *loader = fn_800122D4();
    void *model = fn_80012684(loader, m_enemy->m_motionBase, 5);
    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");
    m_render = fn_802CDB08(m_enemy->m_motionBase, MotionTable(lbl_805F8670, 1), model, motion);
}
"""
S = B.replace("m_enemy", "self->m_enemy").replace("    m_render", "    self->m_render")
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"
INL = "inline void LoadRender_(EggShadowAndroid *self) {\n" + S
REPL = {
  "inl": [(H + B, INL + H + "    LoadRender_(this);\n}\n")],
  "inl_prag": [(H + B, INL + P0 + H + "    LoadRender_(this);\n}\n" + P1)],
  "self_reassigned_prag": [(H + B, P0 + H + "    EggShadowAndroid *self = this;\n" + S.replace("    void *motion", "    self = this;\n    void *motion") + P1)],
  "volatile_like": [(H + B, P0 + H + "    EggShadowAndroid *self;\n    self = this;\n" + S + P1)],
}
