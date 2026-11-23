"""Wave 40: fn_80049BC0, retail copies the branch to r31 in the parameters' order."""
SCORE = ["fn_80049BC0"]
CUR = """    s32 b = branch;
    if (lbl_8057D6A0.m_showMessages == 0) {
        return 0;
    }
    const u16 *text = fn_80049D38(self, id, b, index, 0, voice);
    if (text == 0 && b != 0) {
        b = 0;
        text = fn_80049D38(self, id, b, index, 0, voice);
    }
"""
ORIG = """    if (lbl_8057D6A0.m_showMessages == 0) {
        return 0;
    }
    const u16 *text = fn_80049D38(self, id, branch, index, 0, voice);
    if (text == 0 && branch != 0) {
        branch = 0;
        text = fn_80049D38(self, id, branch, index, 0, voice);
    }
"""
RET = "    return self->Find(id, b, index);\n"
REPL = {
  "orig": [(CUR, ORIG), (RET, RET.replace(", b,", ", branch,"))],
  "orig_textfirst": [(CUR, "    const u16 *text;\n" + ORIG.replace("const u16 *text =", "text =")), (RET, RET.replace(", b,", ", branch,"))],
  "orig_text_top": [(CUR, ORIG.replace("    const u16 *text = fn", "    const u16 *text;\n    text = fn")), (RET, RET.replace(", b,", ", branch,"))],
}

SHOW = """    if (lbl_8057D6A0.m_showMessages == 0) {
        return 0;
    }
"""
REPL = {
  "b_after_check": [(CUR, SHOW + "    s32 b = branch;\n" + CUR.replace("    s32 b = branch;\n", "").replace(SHOW, ""))],
  "b_assigned_later": [(CUR, "    s32 b;\n" + SHOW + "    b = branch;\n" + CUR.replace("    s32 b = branch;\n", "").replace(SHOW, ""))],
  "b_after_first_call": [(CUR, SHOW + """    const u16 *text = fn_80049D38(self, id, branch, index, 0, voice);
    s32 b = branch;
    if (text == 0 && b != 0) {
        b = 0;
        text = fn_80049D38(self, id, b, index, 0, voice);
    }
""")],
  "b_decl_first_assign_later": [(CUR, "    s32 b;\n" + SHOW + """    const u16 *text = fn_80049D38(self, id, branch, index, 0, voice);
    b = branch;
    if (text == 0 && b != 0) {
        b = 0;
        text = fn_80049D38(self, id, b, index, 0, voice);
    }
""")],
  "b_in_test": [(CUR, "    s32 b;\n" + SHOW + """    const u16 *text = fn_80049D38(self, id, branch, index, 0, voice);
    if (text == 0 && (b = branch) != 0) {
        b = 0;
        text = fn_80049D38(self, id, b, index, 0, voice);
    }
""")],
  "text_and_b": [(CUR, "    const u16 *text;\n    s32 b = branch;\n" + SHOW + """    text = fn_80049D38(self, id, b, index, 0, voice);
    if (text == 0 && b != 0) {
        b = 0;
        text = fn_80049D38(self, id, b, index, 0, voice);
    }
""")],
}
