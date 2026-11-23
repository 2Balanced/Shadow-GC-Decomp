"""Wave 40: vfDisp_0x64 (retail: this r31, loader r30, model r29). A local copy of this declared
first and assigned after the first call (the shape that gave TMessageDisp's fn_80049BC0 its
parameter-order copy)."""
SCORE = ["vfDisp_0x64__Q216EggShadowAndroid16EggShadowAndroidFv"]
B = """    void *loader = fn_800122D4();
    void *model = fn_80012684(loader, m_enemy->m_motionBase, 5);
    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");
    m_render = fn_802CDB08(m_enemy->m_motionBase, MotionTable(lbl_805F8670, 1), model, motion);
"""
S = B.replace("m_enemy", "self->m_enemy").replace("    m_render", "    self->m_render")
REPL = {
  "self_first": [(B, "    EggShadowAndroid *self = this;\n" + S)],
  "self_decl_first_assign_after_call": [(B, "    EggShadowAndroid *self;\n" + S.replace("    void *loader = fn_800122D4();\n", "    void *loader = fn_800122D4();\n    self = this;\n"))],
  "self_after_loader": [(B, S.replace("    void *loader = fn_800122D4();\n", "    void *loader = fn_800122D4();\n    EggShadowAndroid *self = this;\n"))],
  "self_decl_first_assign_after_model": [(B, "    EggShadowAndroid *self;\n" + S.replace("    void *model = fn_80012684(loader, self->m_enemy->m_motionBase, 5);\n", "    self = this;\n    void *model = fn_80012684(loader, self->m_enemy->m_motionBase, 5);\n").replace("self = this;\n    void *model = fn_80012684(loader, self->m_enemy", "self = this;\n    void *model = fn_80012684(loader, self->m_enemy"))],
  "decl_all_first": [(B, "    EggShadowAndroid *self;\n    void *loader;\n    void *model;\n    loader = fn_800122D4();\n    self = this;\n" + S.replace("    void *loader = fn_800122D4();\n", "").replace("    void *model = ", "    model = "))],
}
