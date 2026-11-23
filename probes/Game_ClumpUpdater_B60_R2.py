"""Wave 40: fn_80015B60, where -angle goes (retail f26 = the dead `sum`; GC/2.7 f28 in place).
Fresh shapes: a named negation declared at other points of the declaration order, other dead
locals reused, the angle negated in place."""
SCORE = ["fn_80015B60"]
BODY = """    float diff = length2 - length0;
    float dd = diff;
    dd *= dd;
    float sum = length0 + length2;
    float qa = 4.0f * length0 * length2;
    float qb = length1 * sum;
    dd += length1 * length1 - distance * distance;
    float disc = qb * qb - qa * dd;
    if (disc < 0.0f) {
        return 0.0f;
    }
    float t = (fn_8000FEE4(disc) - qb) / qa;
    float angle = fn_80015D68(t);
    float offset = fn_8000F3F8(diff * std::sin(angle) / (t * sum + length1));
    offset += angle;
    fn_8040B884(a, offset);
    fn_8040C148(b, a);
    fn_8040B52C(b, 0.0f, 0.0f, -length0);
    fn_8040B884(b, -angle);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, -angle);
    return distance;
"""
NEGUSE = """    fn_8040B884(b, -angle);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, -angle);
"""
def use(v, pre=""):
    return pre + f"""    {v} = -angle;
    fn_8040B884(b, {v});
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, {v});
"""
def decl_at(anchor, after=True):
    body = BODY.replace(NEGUSE, use("neg"))
    line = "    float neg;\n"
    return [(BODY, body.replace(anchor, anchor + line if after else line + anchor))]
REPL = {
  "neg_first": [(BODY, "    float neg;\n" + BODY.replace(NEGUSE, use("neg")))],
  "neg_after_diff": decl_at("    float diff = length2 - length0;\n"),
  "neg_after_sum": decl_at("    float sum = length0 + length2;\n"),
  "neg_after_qa": decl_at("    float qa = 4.0f * length0 * length2;\n"),
  "neg_after_qb": decl_at("    float qb = length1 * sum;\n"),
  "neg_after_t": decl_at("    float t = (fn_8000FEE4(disc) - qb) / qa;\n"),
  "neg_after_angle": decl_at("    float angle = fn_80015D68(t);\n"),
  "neg_after_offset": decl_at("    float offset = fn_8000F3F8(diff * std::sin(angle) / (t * sum + length1));\n"),
  "reuse_diff": [(NEGUSE, use("diff"))],
  "reuse_t": [(NEGUSE, use("t"))],
  "reuse_qa": [(NEGUSE, use("qa"))],
  "reuse_qb": [(NEGUSE, use("qb"))],
  "reuse_dd": [(NEGUSE, use("dd"))],
  "reuse_disc": [(NEGUSE, use("disc"))],
  "reuse_offset": [(NEGUSE, use("offset"))],
  "angle_inplace": [(NEGUSE, use("angle"))],
  "reuse_len0": [(NEGUSE, use("length0"))],
}
