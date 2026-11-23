"""Wave 40: BkChibiChaos::Update case 1, the subtraction's argument set-up (retail r5, r3, r4 with
m_info loaded into r30 before GetCorePosition; the inline Subtract gives r4, r5, r3)."""
SCORE = ["Update__Q27BkChaos12BkChibiChaosFf"]
F = "void BkChibiChaos::Update(float dt) {"
HELP = """inline Vec3f SubRP_(const Vec3f &a, const Vec3f *b) { return ((VectorSubtractFn)fn_8040CD74)(&a, b); }
inline Vec3f SubPR_(const Vec3f *a, const Vec3f &b) { return ((VectorSubtractFn)fn_8040CD74)(a, &b); }
inline Vec3f SubLoc_(const Vec3f &a, const Vec3f &b) { const Vec3f *pb = &b; return ((VectorSubtractFn)fn_8040CD74)(&a, pb); }
inline Vec3f SubBA_(const Vec3f &b, const Vec3f &a) { return ((VectorSubtractFn)fn_8040CD74)(&a, &b); }
inline Vec3f SubInfo_(const Vec3f &a, CharInfo *info) { return ((VectorSubtractFn)fn_8040CD74)(&a, &info->m_position); }
"""
H = (F, HELP + F)
C1 = """        fn_800091EC(&toCore, &(const Vec3f &)Subtract(
                                 reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition()),
                                 m_info->m_position));
"""
CORE = "reinterpret_cast<const Vec3f &>(m_owner->GetCorePosition())"
def v(expr):
    return [H, (C1, "        fn_800091EC(&toCore, &(const Vec3f &)" + expr + ");\n")]
REPL = {
  "rp": v("SubRP_(" + CORE + ", &m_info->m_position)"),
  "pr": v("SubPR_(&" + CORE + ", m_info->m_position)"),
  "loc": v("SubLoc_(" + CORE + ", m_info->m_position)"),
  "ba": v("SubBA_(m_info->m_position, " + CORE + ")"),
  "info": v("SubInfo_(" + CORE + ", m_info.get())"),
}
