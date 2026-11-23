"""Wave 40: BkChibiChaos::Update, case 1's subtraction (retail: m_info loaded into r30 before the
GetCorePosition call, then r5, r3, r4 at fn_8040CD74). With the height test read through an
inline PosY (R2: inl_posy)."""
SCORE = ["Update__Q27BkChaos12BkChibiChaosFf"]
F = "void BkChibiChaos::Update(float dt) {"
A = """        if (m_info->m_position.y > m_owner->m_maxY) {
            m_info->m_position.y = m_owner->m_maxY;"""
A2 = """        if (m_owner->m_maxY < PosY2_(m_info.get())) {
            m_info->m_position.y = m_owner->m_maxY;"""
HELP = """inline float PosY2_(CharInfo *info) { return info->m_position.y; }
inline Vec3f SubtractP_(const Vec3f *a, const Vec3f *b) { return ((VectorSubtractFn)fn_8040CD74)(a, b); }
"""
H = (F, HELP + F)
C1 = """        Vec3f toCore;
        fn_800091EC(&toCore, &(const Vec3f &)Subtract(
                                 reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition()),
                                 m_info->m_position));
"""
BASE = [H, (A, A2)]
REPL = {
  "base": BASE,
  "info_local": BASE + [(C1, """        Vec3f toCore;
        CharInfo *info = m_info.get();
        fn_800091EC(&toCore, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                 &reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition()),
                                 &info->m_position));
""")],
  "pos_ref": BASE + [(C1, """        Vec3f toCore;
        const Vec3f &position = m_info->m_position;
        fn_800091EC(&toCore, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                 &reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition()),
                                 &position));
""")],
  "subp": BASE + [(C1, """        Vec3f toCore;
        fn_800091EC(&toCore, &(const Vec3f &)SubtractP_(
                                 &reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition()),
                                 &m_info->m_position));
""")],
  "info_local_sub": BASE + [(C1, """        Vec3f toCore;
        CharInfo *info = m_info.get();
        fn_800091EC(&toCore, &(const Vec3f &)Subtract(
                                 reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition()),
                                 info->m_position));
""")],
  "core_local": BASE + [(C1, """        Vec3f toCore;
        Fn8040CopyVector3 core = m_owner->GetCorePosition();
        fn_800091EC(&toCore, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                 &reinterpret_cast<const Vec3f &>(core), &m_info->m_position));
""")],
}

HELP2 = HELP + """inline const Vec3f &Ref_(const Vec3f &v) { return v; }
inline const Vec3f *PosOf_(CharInfo *info) { return &info->m_position; }
"""
H2 = (F, HELP2 + F)
BASE2 = [H2, (A, A2)]
REPL = {
  "ref_b": BASE2 + [(C1, """        Vec3f toCore;
        fn_800091EC(&toCore, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                 &reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition()),
                                 &Ref_(m_info->m_position)));
""")],
  "posof_b": BASE2 + [(C1, """        Vec3f toCore;
        fn_800091EC(&toCore, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                 &reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition()),
                                 PosOf_(m_info.get())));
""")],
}
