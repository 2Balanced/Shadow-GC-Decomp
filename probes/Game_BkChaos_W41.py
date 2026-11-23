"""Wave 41: BkChibiChaos::Update case 1 (retail r5, r3, r4 at fn_8040CD74 after GetCorePosition)."""
SCORE = ["Update__Q27BkChaos12BkChibiChaosFf"]
C1 = """        fn_800091EC(&toCore, &(const Vec3f &)Subtract(
                                 reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition()),
                                 m_info->m_position));
"""
CORE = "reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition())"
def v(stmt, pre=""):
    return [(C1, pre + stmt)]
REPL = {
  "direct": v("        fn_800091EC(&toCore, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&" + CORE + ", &m_info->m_position));\n"),
  "direct_px": v("        fn_800091EC(&toCore, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&" + CORE + ", &m_info.px->m_position));\n"),
  "sub_px": v("        fn_800091EC(&toCore, &(const Vec3f &)Subtract(" + CORE + ", m_info.px->m_position));\n"),
  "tmpref": v("        const Vec3f &d = Subtract(" + CORE + ", m_info->m_position);\n        fn_800091EC(&toCore, &d);\n"),
  "tmpref_direct": v("        const Vec3f &d = ((VectorSubtractFn)fn_8040CD74)(&" + CORE + ", &m_info->m_position);\n        fn_800091EC(&toCore, &d);\n"),
}
