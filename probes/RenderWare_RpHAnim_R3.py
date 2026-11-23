"""Wave 41, RenderWare/RpHAnim.c round 3: fn_804472AC (retail moves the key frame size out of r31
before the new hierarchy takes it)."""
SCORE = ["fn_804472AC"]
NL = chr(10)
A = """    R1Hierarchy *hierarchy =
        ((R1Hierarchy *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))(lbl_805F1440[1], 0x3011E);
    int i;
    hierarchy->m_interpolator = fn_804779DC(count, keyFrameSize);
"""
ALLOC = "((R1Hierarchy *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))(lbl_805F1440[1], 0x3011E)"
REPL = {
    "base": [],
    "decl_top": [(A, "    R1Hierarchy *hierarchy;" + NL + "    int i;" + NL + "    hierarchy = " + ALLOC + ";" + NL + "    hierarchy->m_interpolator = fn_804779DC(count, keyFrameSize);" + NL)],
    "i_first": [(A, "    int i;" + NL + "    R1Hierarchy *hierarchy;" + NL + "    hierarchy = " + ALLOC + ";" + NL + "    hierarchy->m_interpolator = fn_804779DC(count, keyFrameSize);" + NL)],
    "interp_local": [(A, "    R1Hierarchy *hierarchy;" + NL + "    void *interp;" + NL + "    int i;" + NL + "    hierarchy = " + ALLOC + ";" + NL + "    interp = fn_804779DC(count, keyFrameSize);" + NL + "    hierarchy->m_interpolator = interp;" + NL)],
    "interp_first": [(A, "    R1Hierarchy *hierarchy;" + NL + "    void *interp;" + NL + "    int i;" + NL + "    hierarchy = " + ALLOC + ";" + NL + "    hierarchy->m_interpolator = (interp = fn_804779DC(count, keyFrameSize));" + NL)],
}
