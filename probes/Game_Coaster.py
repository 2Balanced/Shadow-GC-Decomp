"""Coaster (wave 39, second pass): OnCameraUpdate loads m_chara's pointer before fn_8040B67C."""
SCORE = ["OnCameraUpdate__7CoasterFRQ512SonicteamUSA6System6Camera14CameraModeUser15UpdateEventArgs"]
A = "void Coaster::OnCameraUpdate("
E = "&(const Vec3f &)fn_8040B67C(&matrix, &eye), &m_chara->m_position));"
T = "&(const Vec3f &)fn_8040B67C(&matrix, &target), &m_chara->m_position));"
def v(expr, pre):
    return [(E, E.replace("&m_chara->m_position", expr)), (T, T.replace("&m_chara->m_position", expr)), (A, pre + A)]
REPL = {
 "free_ref": v("CharaPosition(m_chara)", "inline const Vec3f *CharaPosition(const boost::shared_ptr<GadgetCharInfo> &chara) {\n    return &chara->m_position;\n}\n"),
 "free_ptr": v("CharaPosition(m_chara.get())", "inline const Vec3f *CharaPosition(GadgetCharInfo *chara) {\n    return &chara->m_position;\n}\n"),
 "free_val": v("CharaPosition(m_chara)", "inline const Vec3f *CharaPosition(boost::shared_ptr<GadgetCharInfo> chara) {\n    return &chara->m_position;\n}\n"),
 "get": v("&m_chara.get()->m_position", ""),
 "deref": v("&(*m_chara).m_position", ""),
}
REPL = {
 "ref_get": v("CharaPosition(m_chara)", "inline const Vec3f *CharaPosition(const boost::shared_ptr<GadgetCharInfo> &chara) {\n    return &chara.get()->m_position;\n}\n"),
 "ref_px": v("CharaPosition(m_chara)", "inline const Vec3f *CharaPosition(const boost::shared_ptr<GadgetCharInfo> &chara) {\n    return &chara.px->m_position;\n}\n"),
 "ref_star": v("CharaPosition(m_chara)", "inline const Vec3f *CharaPosition(const boost::shared_ptr<GadgetCharInfo> &chara) {\n    return &(*chara).m_position;\n}\n"),
 "this_arrow": v("CharaPosition(this)", "inline const Vec3f *CharaPosition(Coaster *coaster) {\n    return &coaster->m_chara->m_position;\n}\n"),
 "this_px": v("CharaPosition(this)", "inline const Vec3f *CharaPosition(Coaster *coaster) {\n    return &coaster->m_chara.px->m_position;\n}\n"),
 "ref_ref": v("&CharaPosition(m_chara)", "inline const Vec3f &CharaPosition(const boost::shared_ptr<GadgetCharInfo> &chara) {\n    return chara.px->m_position;\n}\n"),
}
M = "    /* 0x038 */ boost::shared_ptr<GadgetCharInfo> m_chara;\n"
def vm(expr, member):
    return [(E, E.replace("&m_chara->m_position", expr)), (T, T.replace("&m_chara->m_position", expr)), (M, member + M)]
REPL = {
 "two_level": v("CharaPosition(m_chara)", "inline GadgetCharInfo *RawChara(const boost::shared_ptr<GadgetCharInfo> &chara) {\n    return chara.px;\n}\ninline const Vec3f *CharaPosition(const boost::shared_ptr<GadgetCharInfo> &chara) {\n    return &RawChara(chara)->m_position;\n}\n"),
 "mem_arrow": vm("GetPosition()", "    const Vec3f *GetPosition() {\n        return &m_chara->m_position;\n    }\n"),
 "mem_get": vm("GetPosition()", "    const Vec3f *GetPosition() {\n        return &m_chara.get()->m_position;\n    }\n"),
 "mem_chara": vm("&GetChara()->m_position", "    GadgetCharInfo *GetChara() {\n        return m_chara.get();\n    }\n"),
 "mem_chara_arrow": vm("&GetChara()->m_position", "    GadgetCharInfo *GetChara() {\n        return m_chara.operator->();\n    }\n"),
 "mem_chara_sp": vm("&GetChara()->m_position", "    boost::shared_ptr<GadgetCharInfo> &GetChara() {\n        return m_chara;\n    }\n"),
}
E1 = "    fn_800091D0(&eye, &(const Vec3f &)fn_8040CDD0(\n"
T1 = "    fn_800091D0(&target, &(const Vec3f &)fn_8040CDD0(\n"
def vl(init, use, ty="GadgetCharInfo *"):
    return [(E, E.replace("&m_chara->m_position", use % "a")), (T, T.replace("&m_chara->m_position", use % "b")),
            (E1, "    %sa = %s;\n" % (ty, init) + E1), (T1, "    %sb = %s;\n" % (ty, init) + T1)]
REPL = {
 "loc_px": vl("m_chara.px", "&%s->m_position"),
 "loc_get": vl("m_chara.get()", "&%s->m_position"),
 "loc_arrow": vl("m_chara.operator->()", "&%s->m_position"),
 "loc_pos_px": vl("&m_chara.px->m_position", "%s", "const Vec3f *"),
 "loc_pos_arrow": vl("&m_chara->m_position", "%s", "const Vec3f *"),
 "loc_pos_nc": vl("&m_chara.px->m_position", "%s", "Vec3f *"),
}
D = "Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);"
REPL = {
 "nc_both": [(D, "Vec3f fn_8040B67C(Mtx34f *matrix, Vec3f *point);")],
 "nc_mtx": [(D, "Vec3f fn_8040B67C(Mtx34f *matrix, const Vec3f *point);")],
 "nc_pt": [(D, "Vec3f fn_8040B67C(const Mtx34f *matrix, Vec3f *point);")],
}
HDR = {k: {"Game/GadgetUtil.h": [(D, v[0][1])]} for k, v in REPL.items()}
for k in list(REPL):
    REPL[k + "_loc"] = REPL[k] + vl("m_chara.px", "&%s->m_position")
    HDR[k + "_loc"] = HDR[k]
HDR = {}
REPL = {
 "ref_chara": vl("*m_chara", "&%s.m_position", "GadgetCharInfo &"),
 "ref_pos": vl("m_chara->m_position", "&%s", "const Vec3f &"),
 "ref_pos_nc": vl("m_chara->m_position", "&%s", "Vec3f &"),
 "sp_copy": vl("m_chara", "&%s->m_position", "const boost::shared_ptr<GadgetCharInfo> &"),
}
# one local for both statements, declared at the top
TOP = "    Mtx34f matrix;\n\n    fn_8040C148(&matrix, &lbl_805E0720);\n    fn_8040B80C(&matrix, m_rotation.z);\n"
REPL["top_px"] = [(E, E.replace("&m_chara->m_position", "&chara->m_position")), (T, T.replace("&m_chara->m_position", "&chara->m_position")), (TOP, "    GadgetCharInfo *chara;\n" + TOP), (E1, "    chara = m_chara.px;\n" + E1), (T1, "    chara = m_chara.px;\n" + T1)]
