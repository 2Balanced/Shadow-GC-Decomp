"""Wave 41: vfDisp_0x64 (retail: this r31, loader r30, model r29) with a named copy of this and
'#pragma opt_propagation off' scoped to the function (the wave 40 lever)."""
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
def v(body):
    return [(H + B, P0 + H + body + P1)]
REPL = {
  "prag_only": v(B),
  "prag_self_first": v("    EggShadowAndroid *self = this;\n" + S),
  "prag_self_first_const": v("    EggShadowAndroid *const self = this;\n" + S),
  "prag_self_after_loader": v(S.replace("    void *loader = fn_800122D4();\n", "    void *loader = fn_800122D4();\n    EggShadowAndroid *self = this;\n")),
  "prag_self_decl_first_assign_after_call": v("    EggShadowAndroid *self;\n" + S.replace("    void *loader = fn_800122D4();\n", "    void *loader = fn_800122D4();\n    self = this;\n")),
  "prag_self_first_partial": v("    EggShadowAndroid *self = this;\n" + B.replace("    m_render", "    self->m_render")),
  "prag_self_first_enemy_only": v("    EggShadowAndroid *self = this;\n" + B.replace("m_enemy", "self->m_enemy")),
}
