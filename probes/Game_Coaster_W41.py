"""Wave 41: Coaster::OnCameraUpdate with copy propagation off (the wave-40 lever): retail loads
m_chara's pointer into r31 before each fn_8040B67C call (the second argument, evaluated first)."""
SCORE = ["OnCameraUpdate__7CoasterFRQ512SonicteamUSA6System6Camera14CameraModeUser15UpdateEventArgs"]
F = "void Coaster::OnCameraUpdate(CameraModeUser::UpdateEventArgs &args) {"
END = "void Coaster::Hang(boost::shared_ptr<CharInfo> rider) {"
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
REPL = {
  "prag": P,
}

FB = "void Coaster::Behave(float dt) {"
EB = "void Coaster::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {"
PB = [(FB, "#pragma opt_propagation off\n" + FB), (EB, "#pragma opt_propagation reset\n" + EB)]
CALL = """            bool end = fn_8001C91C(fn_8001B494(), m_path.get(), m_backward, m_chara.get(), &m_rotation,
                                   lbl_805EEEC0->returnLoops, m_params.moveSpeed * dt);
"""
SCORE = ["OnCameraUpdate__7CoasterFRQ512SonicteamUSA6System6Camera14CameraModeUser15UpdateEventArgs", "Behave__7CoasterFf"]
REPL.update({
  "b_prag": P + PB,
  "b_loops_local": P + [(CALL, "            s32 loops = lbl_805EEEC0->returnLoops;\n" + CALL.replace("lbl_805EEEC0->returnLoops", "loops"))],
  "b_rec_local": P + [(CALL, "            const CoasterStageConfig *record = lbl_805EEEC0;\n" + CALL.replace("lbl_805EEEC0->returnLoops", "record->returnLoops"))],
})

HB = "inline const CoasterStageConfig *Config_() { return lbl_805EEEC0; }\ninline u8 ReturnLoops_() { return lbl_805EEEC0->returnLoops; }\n"
REPL.update({
  "b_cfg_inl": P + [(FB, HB + FB), (CALL, CALL.replace("lbl_805EEEC0->returnLoops", "Config_()->returnLoops"))],
  "b_loops_inl": P + [(FB, HB + FB), (CALL, CALL.replace("lbl_805EEEC0->returnLoops", "ReturnLoops_()"))],
  "b_rec_local_c": P + [(CALL, "            const CoasterStageConfig *const record = lbl_805EEEC0;\n" + CALL.replace("lbl_805EEEC0->returnLoops", "record->returnLoops"))],
  "b_speed_local": P + [(CALL, "            float step = m_params.moveSpeed * dt;\n" + CALL.replace("m_params.moveSpeed * dt", "step"))],
})

REPL.update({
  "b_cast_int": P + [(CALL, CALL.replace("lbl_805EEEC0->returnLoops", "(int)lbl_805EEEC0->returnLoops"))],
  "b_cast_u32": P + [(CALL, CALL.replace("lbl_805EEEC0->returnLoops", "(u32)lbl_805EEEC0->returnLoops"))],
  "b_deref": P + [(CALL, CALL.replace("lbl_805EEEC0->returnLoops", "(*lbl_805EEEC0).returnLoops"))],
  "b_ptrarith": P + [(CALL, CALL.replace("lbl_805EEEC0->returnLoops", "*(&lbl_805EEEC0->returnLoops)"))],
})

DECL = "bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int backward,\n                 GadgetCharInfo *charInfo, Vec3f *rotation, int arg5, float distance);"
REPL.update({
  "b_u8": P + [(DECL, DECL.replace("int arg5", "u8 arg5"))],
  "b_bool": P + [(DECL, DECL.replace("int arg5", "bool arg5"))],
  "b_s8": P + [(DECL, DECL.replace("int arg5", "s8 arg5"))],
  "b_u32": P + [(DECL, DECL.replace("int arg5", "u32 arg5"))],
  "b_u16": P + [(DECL, DECL.replace("int arg5", "u16 arg5"))],
})

REPL.update({
  "b_nonconst": P + [("static const CoasterStageConfig *lbl_805EEEC0;", "static CoasterStageConfig *lbl_805EEEC0;"), ("            lbl_805EEEC0 = &lbl_804BED6C[i];", "            lbl_805EEEC0 = (CoasterStageConfig *)&lbl_804BED6C[i];")],
  "b_constptr_local": P + [(CALL, "            const u8 &loops = lbl_805EEEC0->returnLoops;\n" + CALL.replace("lbl_805EEEC0->returnLoops", "loops"))],
})

FB2 = "void Coaster::Behave(float dt) {"
REPL["b_prag2"] = [(FB2, "#pragma opt_propagation off\n" + FB2), ("void Coaster::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {", "#pragma opt_propagation reset\nvoid Coaster::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {")]

def bc(new):
    return P + [(CALL, new)]
REPL.update({
  "c_arrow": bc("            bool end = fn_8001C91C(fn_8001B494(), m_path.operator->(), m_backward, m_chara.operator->(), &m_rotation,\n                                   lbl_805EEEC0->returnLoops, m_params.moveSpeed * dt);\n"),
  "c_deref": bc("            bool end = fn_8001C91C(fn_8001B494(), &*m_path, m_backward, &*m_chara, &m_rotation,\n                                   lbl_805EEEC0->returnLoops, m_params.moveSpeed * dt);\n"),
  "c_path_arrow": bc("            bool end = fn_8001C91C(fn_8001B494(), m_path.operator->(), m_backward, m_chara.get(), &m_rotation,\n                                   lbl_805EEEC0->returnLoops, m_params.moveSpeed * dt);\n"),
  "c_chara_arrow": bc("            bool end = fn_8001C91C(fn_8001B494(), m_path.get(), m_backward, m_chara.operator->(), &m_rotation,\n                                   lbl_805EEEC0->returnLoops, m_params.moveSpeed * dt);\n"),
  "c_mgr_local": bc("            UnkPathManager_8001B494 *manager = fn_8001B494();\n            bool end = fn_8001C91C(manager, m_path.get(), m_backward, m_chara.get(), &m_rotation,\n                                   lbl_805EEEC0->returnLoops, m_params.moveSpeed * dt);\n"),
})

HBK = "inline s32 Backward_(const Coaster *c) { return c->m_backward; }\n"
REPL.update({
  "c_bk_inl": P + [(FB, HBK + FB), (CALL, CALL.replace("m_backward", "Backward_(this)"))],
})
