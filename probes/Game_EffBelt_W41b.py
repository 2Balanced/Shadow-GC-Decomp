"""Wave 41: EffBelt::Impl::Add: Width() at inline depth 1 (not inside AssignVector/Scale)."""
SCORE = ["Add__Q34Test7EffBelt4ImplFRC5Vec3f"]
S = "    AssignVector(side, Scale(fn_8040CEE8(&side), Width()));\n"
REPL = {
  "direct_all": [(S, "    fn_800091D0(&side, &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&side), Width()));\n")],
  "assign_direct_scale": [(S, "    AssignVector(side, fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&side), Width()));\n")],
  "direct_scale_inline": [(S, "    fn_800091D0(&side, &(const Vec3f &)Scale(fn_8040CEE8(&side), Width()));\n")],
}
