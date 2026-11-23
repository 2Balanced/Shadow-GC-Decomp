"""Wave 41: the normal quantisers' 64.0f and 16384.0f. As literals, GC 1.3.2 swaps the operands
of the fmuls (constant first); retail has the component first, which the old size-cut units got
from extern constants. Variants: how the constant is named."""
SCORE = ["fn_80467BC8", "fn_80468ADC", "fn_804683AC", "fn_804692C0"]
_B = "s32 fn_80467BC8("
_C1 = "    case 1: {\n        s8 *out;\n\n"
_C3 = "    case 3: {\n        s16 *out;\n\n"
def caselocal(n1, n3):
    return [(_C1, "    case 1: {\n        s8 *out;\n        f32 scale = 64.0f;\n\n")] * n1 + \
           [(_C3, "    case 3: {\n        s16 *out;\n        f32 scale = 16384.0f;\n\n")] * n3
REPL = {
    "base": [],
    "case_local": caselocal(4, 4) + [("* 64.0f)", "* scale)")] * 12 + [("* 16384.0f)", "* scale)")] * 12,
    "extern_late": [(_B, "extern const f32 byteNormalScale;\nextern const f32 shortNormalScale;\n\n" + _B)]
        + [("* 64.0f)", "* byteNormalScale)")] * 12 + [("* 16384.0f)", "* shortNormalScale)")] * 12
        + [("/* RwRGBA colours to", "const f32 byteNormalScale = 64.0f;\nconst f32 shortNormalScale = 16384.0f;\n\n/* RwRGBA colours to")],
    "static_array": [(_B, "static const f32 byteNormalScale[1] = { 64.0f };\nstatic const f32 shortNormalScale[1] = { 16384.0f };\n\n" + _B)]
        + [("* 64.0f)", "* byteNormalScale[0])")] * 12 + [("* 16384.0f)", "* shortNormalScale[0])")] * 12,
}
REPL["table2"] = [(_B, "/* The fixed-point scales of the signed normal types: bytes 2.6, shorts 2.14. */\nstatic const f32 normalScale[2] = { 64.0f, 16384.0f };\n\n" + _B)] \
    + [("* 64.0f)", "* normalScale[0])")] * 12 + [("* 16384.0f)", "* normalScale[1])")] * 12
REPL["struct2"] = [(_B, "/* The fixed-point scales of the signed normal types: bytes 2.6, shorts 2.14. */\nstatic const struct {\n    f32 byte;\n    f32 shrt;\n} normalScale = { 64.0f, 16384.0f };\n\n" + _B)] \
    + [("* 64.0f)", "* normalScale.byte)")] * 12 + [("* 16384.0f)", "* normalScale.shrt)")] * 12
REPL["v2d"] = [(_B, "/* The fixed-point scales of the signed normal types: bytes 2.6, shorts 2.14. */\nstatic const RwV2d normalScale = { 64.0f, 16384.0f };\n\n" + _B)] \
    + [("* 64.0f)", "* normalScale.x)")] * 12 + [("* 16384.0f)", "* normalScale.y)")] * 12
