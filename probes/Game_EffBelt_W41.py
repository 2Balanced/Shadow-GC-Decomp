"""Wave 41: EffBelt::Impl::Add: the width (retail: lfs f31 before the normalize call)."""
SCORE = ["Add__Q34Test7EffBelt4ImplFRC5Vec3f"]
S = "    AssignVector(side, Scale(fn_8040CEE8(&side), Width()));\n"
REPL = {
  "member": [(S, "    AssignVector(side, Scale(fn_8040CEE8(&side), m_width));\n")],
  "local": [(S, "    float width = m_width;\n    AssignVector(side, Scale(fn_8040CEE8(&side), width));\n")],
  "direct_width": [(S, "    AssignVector(side, fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&side), Width()));\n")],
  "direct_member": [(S, "    AssignVector(side, fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&side), m_width));\n")],
  "this_width": [(S, "    AssignVector(side, Scale(fn_8040CEE8(&side), this->m_width));\n")],
}
