"""Wave 41, RenderWare/RpUVAnimKeyFrame: fn_8045345C (linear keyframe mul-recip) float registers."""
SCORE = ["fn_8045345C"]
OLD = """    float determinant = from[2] * from[5] - from[3] * from[4];
    if (determinant != 0.0f) {
        float inverse = 1.0f / determinant;
        float upperRight = -from[3] * inverse;
        float lowerLeft = -from[4] * inverse;
        float lowerRight = from[5] * inverse;
        float m10 = out[4];
        float m11 = out[5];
        float m00 = out[2];
        float m01 = out[3];"""
def v(head, cond="determinant != 0.0f"):
    return [(OLD, head + "\n    if (" + cond + """) {
        float inverse = 1.0f / determinant;
        float upperRight = -from[3] * inverse;
        float lowerLeft = -from[4] * inverse;
        float lowerRight = from[5] * inverse;
        float m10 = out[4];
        float m11 = out[5];
        float m00 = out[2];
        float m01 = out[3];""")]
REPL = {
    "base": [],
    "rev_cond": v("    float determinant = from[2] * from[5] - from[3] * from[4];", "0.0f != determinant"),
    "named": v("    float a = from[2];\n    float b = from[3];\n    float c = from[4];\n    float d = from[5];\n    float determinant = a * d - b * c;"),
    "paren": v("    float determinant = (from[2] * from[5]) - (from[3] * from[4]);"),
    "split": v("    float determinant;\n    determinant = from[2] * from[5];\n    determinant -= from[3] * from[4];"),
    "bc_first": v("    float determinant = -(from[3] * from[4]) + from[2] * from[5];"),
    "decl_late": v("    float determinant;\n    determinant = from[2] * from[5] - from[3] * from[4];"),
}
INNER_OLD = """        float inverse = 1.0f / determinant;
        float upperRight = -from[3] * inverse;
        float lowerLeft = -from[4] * inverse;
        float lowerRight = from[5] * inverse;
        float m10 = out[4];
        float m11 = out[5];
        float m00 = out[2];
        float m01 = out[3];"""
REPL["m_first"] = [(INNER_OLD, """        float m10 = out[4];
        float m11 = out[5];
        float m00 = out[2];
        float m01 = out[3];
        float inverse = 1.0f / determinant;
        float upperRight = -from[3] * inverse;
        float lowerLeft = -from[4] * inverse;
        float lowerRight = from[5] * inverse;""")]
REPL["lr_first"] = [(INNER_OLD, """        float inverse = 1.0f / determinant;
        float lowerRight = from[5] * inverse;
        float upperRight = -from[3] * inverse;
        float lowerLeft = -from[4] * inverse;
        float m10 = out[4];
        float m11 = out[5];
        float m00 = out[2];
        float m01 = out[3];""")]
REPL["neg_inv"] = [(INNER_OLD, """        float inverse = 1.0f / determinant;
        float upperRight = -(from[3] * inverse);
        float lowerLeft = -(from[4] * inverse);
        float lowerRight = from[5] * inverse;
        float m10 = out[4];
        float m11 = out[5];
        float m00 = out[2];
        float m01 = out[3];""")]
FULL_OLD = OLD + """
        out[2] = lowerRight * m00 + upperRight * m10;
        out[3] = lowerRight * m01 + upperRight * m11;"""
SIG_OLD = "void fn_8045345C(float *out, float *from)\n{"
STRUCT = """typedef struct UVKey {
    void *prev;
    float time;
    float uv[6];
} UVKey;

void fn_8045345C(float *outv, float *fromv)
{
    UVKey *out = (UVKey *)outv;
    UVKey *from = (UVKey *)fromv;"""
def sv(text):
    return text
REPL.update({
    "struct": [(SIG_OLD, STRUCT), ("    float determinant = from[2] * from[5] - from[3] * from[4];\n    if (determinant != 0.0f) {\n        float inverse = 1.0f / determinant;\n        float upperRight = -from[3] * inverse;\n        float lowerLeft = -from[4] * inverse;\n        float lowerRight = from[5] * inverse;\n        float m10 = out[4];\n        float m11 = out[5];\n        float m00 = out[2];\n        float m01 = out[3];\n        out[2] = lowerRight * m00 + upperRight * m10;\n        out[3] = lowerRight * m01 + upperRight * m11;",
        "    float determinant = from->uv[0] * from->uv[3] - from->uv[1] * from->uv[2];\n    if (determinant != 0.0f) {\n        float inverse = 1.0f / determinant;\n        float upperRight = -from->uv[1] * inverse;\n        float lowerLeft = -from->uv[2] * inverse;\n        float lowerRight = from->uv[3] * inverse;\n        float m10 = out->uv[2];\n        float m11 = out->uv[3];\n        float m00 = out->uv[0];\n        float m01 = out->uv[1];\n        out->uv[0] = lowerRight * m00 + upperRight * m10;\n        out->uv[1] = lowerRight * m01 + upperRight * m11;"),
        ("        out[4] = lowerLeft * m00 + lowerLeft * m10;\n        out[5] = lowerLeft * m01 + lowerLeft * m11;\n    }\n    out[6] = out[6] - from[6];\n    out[7] = out[7] - from[7];",
         "        out->uv[2] = lowerLeft * m00 + lowerLeft * m10;\n        out->uv[3] = lowerLeft * m01 + lowerLeft * m11;\n    }\n    out->uv[4] = out->uv[4] - from->uv[4];\n    out->uv[5] = out->uv[5] - from->uv[5];")],
    "inv_first_decl": [(OLD, """    float inverse;
    float upperRight;
    float lowerLeft;
    float lowerRight;
    float m10;
    float m11;
    float m00;
    float m01;
    float determinant = from[2] * from[5] - from[3] * from[4];
    if (determinant != 0.0f) {
        inverse = 1.0f / determinant;
        upperRight = -from[3] * inverse;
        lowerLeft = -from[4] * inverse;
        lowerRight = from[5] * inverse;
        m10 = out[4];
        m11 = out[5];
        m00 = out[2];
        m01 = out[3];""")],
    "noms": [(FULL_OLD, """    float determinant = from[2] * from[5] - from[3] * from[4];
    if (determinant != 0.0f) {
        float inverse = 1.0f / determinant;
        float upperRight = -from[3] * inverse;
        float lowerLeft = -from[4] * inverse;
        float lowerRight = from[5] * inverse;
        float m10 = out[4];
        float m11 = out[5];
        float m00 = out[2];
        float m01 = out[3];
        out[2] = lowerRight * m00 + upperRight * m10;
        out[3] = lowerRight * m01 + upperRight * m11;""")],
    "lr_named_d": [(OLD, """    float d = from[5];
    float determinant = from[2] * d - from[3] * from[4];
    if (determinant != 0.0f) {
        float inverse = 1.0f / determinant;
        float upperRight = -from[3] * inverse;
        float lowerLeft = -from[4] * inverse;
        float lowerRight = d * inverse;
        float m10 = out[4];
        float m11 = out[5];
        float m00 = out[2];
        float m01 = out[3];""")],
})
REPL.update({
    "const_from": [("void fn_8045345C(float *out, float *from)", "void fn_8045345C(float *out, const float *from)")],
    "const_both": [("void fn_8045345C(float *out, float *from)", "void fn_8045345C(float *out, const float *from)"), (OLD, OLD.replace("float determinant", "const float determinant"))],
    "void_params": [("void fn_8045345C(float *out, float *from)\n{", "void fn_8045345C(void *outv, void *fromv)\n{\n    float *out = (float *)outv;\n    float *from = (float *)fromv;")],
    "void_params_c": [("void fn_8045345C(float *out, float *from)\n{", "void fn_8045345C(void *outv, void *fromv)\n{\n    float *out = (float *)outv;\n    const float *from = (const float *)fromv;")],
})
HELPER = """static inline float UVDeterminant(const float *m) {
    return m[2] * m[5] - m[3] * m[4];
}

void fn_8045345C(float *out, float *from)
{"""
REPL.update({
    "det_inline": [("void fn_8045345C(float *out, float *from)\n{", HELPER),
                   ("    float determinant = from[2] * from[5] - from[3] * from[4];", "    float determinant = UVDeterminant(from);")],
    "det_inline_nc": [("void fn_8045345C(float *out, float *from)\n{", HELPER.replace("const float *m", "float *m")),
                   ("    float determinant = from[2] * from[5] - from[3] * from[4];", "    float determinant = UVDeterminant(from);")],
    "det_macro": [("    float determinant = from[2] * from[5] - from[3] * from[4];", "#define DET(m) ((m)[2] * (m)[5] - (m)[3] * (m)[4])\n    float determinant = DET(from);")],
    "lr_inline": [("void fn_8045345C(float *out, float *from)\n{", "static inline float UVScaled(float v, float s) {\n    return v * s;\n}\n\nvoid fn_8045345C(float *out, float *from)\n{"),
                  ("        float lowerRight = from[5] * inverse;", "        float lowerRight = UVScaled(from[5], inverse);")],
})
