"""Wave 40: fn_8004A7B4's font loop in an inline helper (the lever that gives the `mr` copies of
the loop's zero, as in ClumpUpdater's fn_80015570 and TGiHdGage)."""
SCORE = ["fn_8004A7B4"]
LOOP = """    for (int i = 0; i < 10; i++) {
        if (self->m_fonts[i].m_font != 0) {
            fn_804736CC(self->m_fonts[i].m_font);
            self->m_fonts[i].m_font = 0;
            if (self->m_fonts[i].m_chars != 0) {
                fn_80049B34(self->m_fonts[i].m_chars);
                self->m_fonts[i].m_chars = 0;
            }
        }
    }
"""
FN = 'extern "C" void fn_8004A7B4(TMessageDisp *self) {\n'
def helper(sig, call, body=LOOP):
    return [(FN + LOOP, "inline void " + sig + " {\n" + body + "}\n\n" + FN + "    " + call + ";\n")]
REPL = {
  "self": helper("ReleaseFonts_(TMessageDisp *self)", "ReleaseFonts_(self)"),
  "self_pre": helper("ReleaseFonts_(TMessageDisp *self)", "ReleaseFonts_(self)", LOOP.replace("i++", "++i")),
}
