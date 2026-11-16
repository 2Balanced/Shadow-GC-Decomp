"""Shapes for the first fn_800C7F7C call of BossCharaColli::Impl::Impl (wave 39, second pass)."""
OLD = """                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli->m_shape, m_param->m_motion, colli->field_0x8, 0);
"""
DECL = "boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &charInfo,\n                                              const void *part, void *work, int arg4, int arg5);"
def v(s):
    return [(OLD, s)]
REPL = {
 "shape_local": v("""                    const void *shape = colli->m_shape;
                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           shape, m_param->m_motion, colli->field_0x8, 0);
"""),
 "weak_local": v("""                    boost::weak_ptr<CharInfo> chara(m_param->m_charaPtr);
                    collider = fn_800C7F7C(manager, chara,
                                           colli->m_shape, m_param->m_motion, colli->field_0x8, 0);
"""),
 "motion_local": v("""                    void *motion = m_param->m_motion;
                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli->m_shape, motion, colli->field_0x8, 0);
"""),
 "param_local": v("""                    BossVarParam *p = m_param;
                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(p->m_charaPtr),
                                           colli->m_shape, p->m_motion, colli->field_0x8, 0);
"""),
 "arg_local": v("""                    int arg = colli->field_0x8;
                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli->m_shape, m_param->m_motion, arg, 0);
"""),
 "result_local": v("""                    boost::shared_ptr<CharaColliBase> made = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli->m_shape, m_param->m_motion, colli->field_0x8, 0);
                    collider = made;
"""),
 "cast": v("""                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           (const void *)colli->m_shape, (void *)m_param->m_motion, colli->field_0x8, 0);
"""),
 "inl_shape": [("BossCharaColli::Impl::Impl(BossVarParam *param)", "inline const void *ShapeOf(UnkBossColli *c) {\n    return c->m_shape;\n}\n\nBossCharaColli::Impl::Impl(BossVarParam *param)"),
   (OLD, """                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           ShapeOf(colli), m_param->m_motion, colli->field_0x8, 0);
""")],
 "inl_motion": [("BossCharaColli::Impl::Impl(BossVarParam *param)", "inline void *MotionOf(BossVarParam *c) {\n    return c->m_motion;\n}\n\nBossCharaColli::Impl::Impl(BossVarParam *param)"),
   (OLD, """                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli->m_shape, MotionOf(m_param), colli->field_0x8, 0);
""")],
 "inl_weak": [("BossCharaColli::Impl::Impl(BossVarParam *param)", "inline boost::weak_ptr<CharInfo> WeakOf(BossVarParam *c) {\n    return boost::weak_ptr<CharInfo>(c->m_charaPtr);\n}\n\nBossCharaColli::Impl::Impl(BossVarParam *param)"),
   (OLD, """                    collider = fn_800C7F7C(manager, WeakOf(m_param),
                                           colli->m_shape, m_param->m_motion, colli->field_0x8, 0);
""")],
 "inl_make": [("BossCharaColli::Impl::Impl(BossVarParam *param)", "inline boost::shared_ptr<CharaColliBase> Make(void *manager, BossVarParam *p, const void *part, int arg) {\n    return fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(p->m_charaPtr), part, p->m_motion, arg, 0);\n}\n\nBossCharaColli::Impl::Impl(BossVarParam *param)"),
   (OLD, """                    collider = Make(manager, m_param, colli->m_shape, colli->field_0x8);
""")],
 "this_arrow": v("""                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(this->m_param->m_charaPtr),
                                           colli->m_shape, this->m_param->m_motion, colli->field_0x8, 0);
"""),
}
SCORE = ["__ct__Q214BossCharaColli4ImplFP12BossVarParam"]
