"""Wave 41, RenderWare/RpSkinGameCube: RpSkinAtomicSetType (fn_80451E6C) as a switch."""
SCORE = ["fn_80451E6C"]
SRC = open("src/RenderWare/RpSkinGameCube.c", newline="").read().replace("\r\n", "\n")


def _span(text, start, end, after=0):
    """The text from start up to and including end (searched from start); a marker when the
    anchor is gone because a variant of this table was kept in the source."""
    try:
        a = text.index(start)
        b = text.index(end, a) + len(end) + after
    except ValueError:
        return "<<anchor gone: a variant of this table is in the source>>"
    return text[a:b]

OLD = _span(SRC, "u32 *fn_80451E6C(u32 *object, s32 mode) {", "\n}\n", 3 - len("\n}\n"))
def v(tail, rec="u32 *record = (u32 *)fn_8044CCAC((R2Geometry *)object[6]);"):
    return """u32 *fn_80451E6C(u32 *object, s32 mode) {
    %s
    s32 value;

    switch (mode) {
    case 1:
        record[11] = 1;
        value = 1;
        break;
    case 2:
        record[11] = 2;
        value = 2;
        break;
    default:
        if (fn_80458C90(0x120) > 0 && fn_804A2064(object) != 0) {
            record[11] = 2;
            value = 2;
        } else {
            record[11] = 1;
            value = 1;
        }
        break;
    }
    %s
    return object;
}
""" % (rec, tail)
REPL = {
    "base": [],
    "sw": [(OLD, v("object[0x1B] = ((u32 *)&lbl_8056D708)[9 + value];"))],
    "sw_field": [(OLD, v("object[0x1B] = (u32)lbl_8056D708.pipelines[value];"))],
    "sw_field_skin": [(OLD, v("object[0x1B] = (u32)lbl_8056D708.pipelines[value];", "R2Skin *skin = fn_8044CCAC((R2Geometry *)object[6]);\n    u32 *record = (u32 *)skin;"))],
}
def v2(order):
    cases = {
        "1": "    case 1:\n        record[11] = 1;\n        value = 1;\n        break;\n",
        "2": "    case 2:\n        record[11] = 2;\n        value = 2;\n        break;\n",
        "d": "    default:\n        if (fn_80458C90(0x120) > 0 && fn_804A2064(object) != 0) {\n            record[11] = 2;\n            value = 2;\n        } else {\n            record[11] = 1;\n            value = 1;\n        }\n        break;\n",
        "0": "    case 0:\n",
    }
    return """u32 *fn_80451E6C(u32 *object, s32 mode) {
    u32 *record = (u32 *)fn_8044CCAC((R2Geometry *)object[6]);
    s32 value;

    switch (mode) {
""" + "".join(cases[c] for c in order) + """    }
    object[0x1B] = (u32)lbl_8056D708.pipelines[value];
    return object;
}
"""
for order in ("21d", "d12", "d21", "12d", "1d2", "2d1"):
    REPL["o_" + order] = [(OLD, v2(order))]
REPL["o_0d12"] = [(OLD, v2("0d12"))]
