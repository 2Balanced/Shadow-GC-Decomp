"""Wave 41: fn_80015B60 under '#pragma opt_propagation off' with the negated angle a named local
declared at each position (retail: -angle in f26, sum's dead register)."""
SCORE = ["fn_80015B60"]
H = """float fn_80015B60(Mtx34f *a, Mtx34f *b, Mtx34f *c, const Vec3f *target, const Mtx34f *base,
                  float length0, float length1, float length2) {
"""
OLD = """    float distance = fn_80016104(a, target, base, true);
    if (distance < 0.0001f) {
        return 0.0f;
    }
    if (distance > length2 + (length0 + length1)) {
        fn_8040C148(b, a);
        fn_8040B52C(b, 0.0f, 0.0f, -length0);
        fn_8040C148(c, b);
        fn_8040B52C(c, 0.0f, 0.0f, -length1);
        return distance;
    }
    float diff = length2 - length0;
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
}
"""
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"
USE = OLD.replace("    fn_8040B884(b, -angle);\n", "    neg = -angle;\n    fn_8040B884(b, neg);\n").replace("    fn_8040B884(c, -angle);\n", "    fn_8040B884(c, neg);\n")
REPL = {"prag": [(H + OLD, P0 + H + OLD + P1)]}
anchors = {
 "top": "    float distance = fn_80016104",
 "after_distance": "    if (distance < 0.0001f) {",
 "after_diff": "    float dd = diff;",
 "after_dd": "    dd *= dd;",
 "after_sum": "    float qa = 4.0f",
 "after_qa": "    float qb = length1 * sum;",
 "after_qb": "    dd += length1",
 "after_disc": "    if (disc < 0.0f) {",
 "after_t": "    float angle = fn_80015D68(t);",
 "after_angle": "    float offset = fn_8000F3F8",
 "after_offset": "    offset += angle;",
}
for k, a in anchors.items():
    body = USE.replace(a, "    float neg;\n" + a, 1)
    REPL["prag_neg_" + k] = [(H + OLD, P0 + H + body + P1)]
    REPL["noprag_neg_" + k] = [(H + OLD, H + body)]
REPL["prag_neg_at_use"] = [(H + OLD, P0 + H + OLD.replace("    fn_8040B884(b, -angle);\n", "    float neg = -angle;\n    fn_8040B884(b, neg);\n").replace("    fn_8040B884(c, -angle);\n", "    fn_8040B884(c, neg);\n") + P1)]
