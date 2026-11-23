"""Wave 41, RenderWare/RpUVAnimParam: fn_80453820 with a struct copy of the matrix."""
SCORE = ["fn_80453820"]
SRC = open("src/RenderWare/RpUVAnimParam.c", newline="").read().replace("\r\n", "\n")


def _span(text, start, end, after=0):
    """The text from start up to and including end (searched from start); a marker when the
    anchor is gone because a variant of this table was kept in the source."""
    try:
        a = text.index(start)
        b = text.index(end, a) + len(end) + after
    except ValueError:
        return "<<anchor gone: a variant of this table is in the source>>"
    return text[a:b]

OLD = _span(SRC, "float *fn_80453820(float *out, const float *key)", "\n }}\n", 5 - len("\n }}\n"))
def v(sig_t, conv):
    return """float *fn_80453820(float *out, %s *matrix)
{
    R2Matrix m = *matrix;
    R2V3d axis = {0.0f, 0.0f, 1.0f};

    out[0] = %s;
    fn_8047F59C((float *)&m, lbl_8051BD40, 2);
    fn_8047EFC8((float *)&m, (const float *)&axis, 180.0f * (-out[0] / 3.1415927f), 2);
    fn_8047F59C((float *)&m, lbl_8051BD4C, 2);
    out[1] = m.right.x;
    out[3] = m.right.y;
    out[2] = m.up.y;
    out[4] = m.pos.x;
    out[5] = m.pos.y;
    return out;
}
""" % (sig_t, conv)
REPL = {
    "base": [],
    "s1": [(OLD, v("const R2Matrix", "(float)fn_803B1C3C(-m.up.x, m.right.x)"))],
    "s2": [(OLD, v("R2Matrix", "(float)fn_803B1C3C(-m.up.x, m.right.x)"))],
    "s3": [(OLD, v("const R2Matrix", "(float)fn_803B1C3C(-(double)m.up.x, (double)m.right.x)"))],
}
