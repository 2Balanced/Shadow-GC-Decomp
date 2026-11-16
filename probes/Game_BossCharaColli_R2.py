"""More shapes for the first fn_800C7F7C call of BossCharaColli::Impl::Impl (wave 39, second pass)."""
OLD = """                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli->m_shape, m_param->m_motion, colli->field_0x8, 0);
"""
C0 = "        UnkBossColli *colli = m_param->m_def->m_collis;\n        if (colli) {\n            while (colli->m_shape"
def v(s):
    return [(OLD, s)]
REPL = {
 "const_colli": [(C0, C0.replace("UnkBossColli *colli", "const UnkBossColli *colli"))],
 "shape_if": [("                if (colli->m_shape) {\n"+OLD, """                if (const UnkBossColliShape *shape = colli->m_shape) {
                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           shape, m_param->m_motion, colli->field_0x8, 0);
""")],
 "deref": v("""                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           &*colli->m_shape, m_param->m_motion, colli->field_0x8, 0);
"""),
 "idx": v("""                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli[0].m_shape, m_param->m_motion, colli[0].field_0x8, 0);
"""),
 "mgr_decl_after": [("        void *manager = fn_80021B08();\n        UnkBossColli *colli = m_param->m_def->m_collis;\n        if (colli) {\n            while (colli->m_shape", "        void *manager = fn_80021B08();\n        UnkBossColli *colli;\n        colli = m_param->m_def->m_collis;\n        if (colli) {\n            while (colli->m_shape")],
 "voidp_local_in_if": v("""                    void *part = (void *)colli->m_shape;
                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr), part,
                                           m_param->m_motion, colli->field_0x8, 0);
"""),
 "param_ref": v("""                    BossVarParam &p = *m_param;
                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli->m_shape, p.m_motion, colli->field_0x8, 0);
"""),
 "shared_copy": v("""                    collider = fn_800C7F7C(manager, m_param->m_charaPtr,
                                           colli->m_shape, m_param->m_motion, colli->field_0x8, 0);
"""),
 "zero_local": v("""                    int zero = 0;
                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli->m_shape, m_param->m_motion, colli->field_0x8, zero);
"""),
 "reset": v("""                    collider = boost::shared_ptr<CharaColliBase>(fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli->m_shape, m_param->m_motion, colli->field_0x8, 0));
"""),
}
SCORE = ["__ct__Q214BossCharaColli4ImplFP12BossVarParam"]
