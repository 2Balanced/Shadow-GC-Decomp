"""Wave 41, RenderWare/RpHAnim.c round 2: fn_80446780 (li 36 + mullw) and fn_804472AC."""
SCORE = ["fn_80446780", "fn_804472AC"]
NL = chr(10)
F = """u32 fn_80446780(u32 *object) {
    return object[1] * 0x24;
}
"""
def f(body):
    return "u32 fn_80446780(u32 *object) {" + NL + body + NL + "}" + NL
REPL = {
    "base": [],
    "swap": [(F, f("    return 0x24 * object[1];"))],
    "sizeof": [(F, "typedef struct R1KeyFrame { float t[9]; } R1KeyFrame;" + NL + f("    return object[1] * sizeof(R1KeyFrame);"))],
    "sizeof2": [(F, "typedef struct R1KeyFrame { float t[9]; } R1KeyFrame;" + NL + f("    return sizeof(R1KeyFrame) * object[1];"))],
    "int": [(F, "int fn_80446780(int *object) {" + NL + "    return object[1] * 0x24;" + NL + "}" + NL)],
    "local": [(F, f("    int size = 0x24;" + NL + "    return object[1] * size;"))],
    "intsz": [(F, "typedef struct R1KeyFrame { float t[9]; } R1KeyFrame;" + NL + f("    return object[1] * (int)sizeof(R1KeyFrame);"))],
}

REPL.update({
    "sconst": [(F, "static const int s_keyFrameSize = 0x24;" + NL + f("    return object[1] * s_keyFrameSize;"))],
    "sconst2": [(F, "static const int s_keyFrameSize = 0x24;" + NL + f("    return s_keyFrameSize * object[1];"))],
    "gconst": [(F, "const int g_keyFrameSize = 0x24;" + NL + f("    return object[1] * g_keyFrameSize;"))],
    "enumc": [(F, "enum { KEYFRAME_SIZE = 0x24 };" + NL + f("    return object[1] * KEYFRAME_SIZE;"))],
})
