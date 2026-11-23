"""Wave 40: fn_80015B60 with every local declared at the top (C style) in several orders, the
negated angle a local of its own (retail: distance f31, length1 f30, length0 f29, qa/angle f28,
diff f27, sum/-angle f26, length2/qb/t f25)."""
SCORE = ["fn_80015B60"]
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
"""
BODY = """    distance = fn_80016104(a, target, base, true);
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
    diff = length2 - length0;
    dd = diff;
    dd *= dd;
    sum = length0 + length2;
    qa = 4.0f * length0 * length2;
    qb = length1 * sum;
    dd += length1 * length1 - distance * distance;
    disc = qb * qb - qa * dd;
    if (disc < 0.0f) {
        return 0.0f;
    }
    t = (fn_8000FEE4(disc) - qb) / qa;
    angle = fn_80015D68(t);
    offset = fn_8000F3F8(diff * std::sin(angle) / (t * sum + length1));
    offset += angle;
    fn_8040B884(a, offset);
    fn_8040C148(b, a);
    fn_8040B52C(b, 0.0f, 0.0f, -length0);
    NEG
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, back);
    return distance;
"""
NEGL = "back = -angle;\n    fn_8040B884(b, back);"
def v(order):
    decl = "".join("    float %s;\n" % n for n in order)
    return [(OLD, decl + BODY.replace("NEG", NEGL))]
REPL = {
  "o1": v(["distance", "diff", "dd", "sum", "qa", "qb", "disc", "t", "angle", "offset", "back"]),
  "o2": v(["distance", "qa", "diff", "back", "t", "angle", "sum", "dd", "qb", "disc", "offset"]),
  "o3": v(["distance", "angle", "diff", "back", "t", "qa", "sum", "qb", "dd", "disc", "offset"]),
  "o4": v(["distance", "diff", "dd", "sum", "back", "qa", "qb", "disc", "t", "angle", "offset"]),
  "o5": v(["back", "distance", "diff", "dd", "sum", "qa", "qb", "disc", "t", "angle", "offset"]),
  "o6": v(["distance", "diff", "dd", "sum", "qa", "back", "qb", "disc", "t", "angle", "offset"]),
}
