"""Signatures of fn_800C7F7C tried for BossCharaColli::Impl::Impl (wave 39, second pass)."""
D = "boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &charInfo,\n                                              const void *part, void *work, int arg4, int arg5);"
def d(s):
    return [(D, s)]
OLD = """                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli->m_shape, m_param->m_motion, colli->field_0x8, 0);
"""
REPL = {
 "byval": d("boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, boost::weak_ptr<CharInfo> charInfo, const void *part, void *work, int arg4, int arg5);"),
 "part_nonconst": d("boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &charInfo, void *part, void *work, int arg4, int arg5);") + [(OLD, OLD.replace("colli->m_shape", "(void *)colli->m_shape"))],
 "work_typed": d("boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &charInfo, const void *part, _stMtnPlayWork *work, int arg4, int arg5);"),
 "u32args": d("boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &charInfo, const void *part, void *work, u32 arg4, u32 arg5);"),
 "bool5": d("boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &charInfo, const void *part, void *work, int arg4, bool arg5);"),
 "nonconst_ref": d("boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, boost::weak_ptr<CharInfo> &charInfo, const void *part, void *work, int arg4, int arg5);"),
 "shape_ref": d("boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &charInfo, const void *part, void *work, int arg4, int arg5);\ninline boost::shared_ptr<CharaColliBase> MakeColli(void *manager, const boost::weak_ptr<CharInfo> &charInfo, const UnkBossColliShape *part, void *work, int arg4, int arg5) {\n    return fn_800C7F7C(manager, charInfo, part, work, arg4, arg5);\n}") + [(OLD, OLD.replace("fn_800C7F7C", "MakeColli"))],
}
SCORE = ["__ct__Q214BossCharaColli4ImplFP12BossVarParam"]
