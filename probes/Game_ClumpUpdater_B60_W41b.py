"""Wave 41: fn_80015B60: the negated angle through a folded conditional (as fn_80015EC8 writes it)."""
SCORE = ["fn_80015B60"]
A = "    fn_8040B884(b, -angle);\n"
C = "    fn_8040B884(c, -angle);\n"
REPL = {
  "tern_true": [(A, "    fn_8040B884(b, true ? -angle : angle);\n"), (C, "    fn_8040B884(c, true ? -angle : angle);\n")],
  "const_flip": [("    float distance = fn_80016104(a, target, base, true);\n", "    const bool flip = true;\n    float distance = fn_80016104(a, target, base, flip);\n"), (A, "    fn_8040B884(b, flip ? -angle : angle);\n"), (C, "    fn_8040B884(c, flip ? -angle : angle);\n")],
  "zero_minus": [(A, "    fn_8040B884(b, -1.0f * angle);\n"), (C, "    fn_8040B884(c, -1.0f * angle);\n")],
  "neg_inline": [("float fn_80015B60(Mtx34f *a,", "inline float Neg_(float x) { return -x; }\nfloat fn_80015B60(Mtx34f *a,"), (A, "    fn_8040B884(b, Neg_(angle));\n"), (C, "    fn_8040B884(c, Neg_(angle));\n")],
}
