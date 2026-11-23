"""Wave 41: BELOW_HALF's constructor: an int moveCamera (retail's registers) tested as a byte."""
SCORE = ["__ct__32DevilDoomModeCtrlUnit_BELOW_HALFFP17DevilDoomVarParam"]
D = "        bool moveCamera = false;\n        fn_804165D8(m_owner->m_motion, 0, -1, -1);\n"
T = "        if (moveCamera) {\n            Vec3f position;\n"
TR = "            moveCamera = true;\n"
def v(decl, test, tr=None):
    r = [(D, decl + "        fn_804165D8(m_owner->m_motion, 0, -1, -1);\n"), (T, "        if (" + test + ") {\n            Vec3f position;\n")]
    if tr:
        r.append((TR, tr))
    return r
REPL = {
  "int_u8": v("        int moveCamera = false;\n", "(u8)moveCamera"),
  "int_uchar": v("        int moveCamera = 0;\n", "(unsigned char)moveCamera", "            moveCamera = 1;\n"),
  "int_and": v("        int moveCamera = 0;\n", "moveCamera & 0xFF", "            moveCamera = 1;\n"),
  "u32_u8": v("        u32 moveCamera = 0;\n", "(u8)moveCamera", "            moveCamera = 1;\n"),
  "int_bool": v("        int moveCamera = 0;\n", "(bool)moveCamera", "            moveCamera = 1;\n"),
}
