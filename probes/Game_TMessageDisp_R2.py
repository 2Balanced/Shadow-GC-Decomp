"""Wave 40: fn_8004A7B4's loop (retail copies the zeroed counter into the offset and the two null
values)."""
SCORE = ["fn_8004A7B4"]
F = 'extern "C" void fn_8004A7B4(TMessageDisp *self) {\n'
L = """    for (int i = 0; i < 10; i++) {
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
REPL = {
  "inl": [(F, "static inline void FreeFonts_(TMessageDisp *self) {\n" + L + "}\n" + F), (L, "    FreeFonts_(self);\n")],
  "inl_font": [(F, """static inline void FreeFont_(TMessageDisp *self, int i) {
    if (self->m_fonts[i].m_font != 0) {
        fn_804736CC(self->m_fonts[i].m_font);
        self->m_fonts[i].m_font = 0;
        if (self->m_fonts[i].m_chars != 0) {
            fn_80049B34(self->m_fonts[i].m_chars);
            self->m_fonts[i].m_chars = 0;
        }
    }
}
""" + F), (L, "    for (int i = 0; i < 10; i++) {\n        FreeFont_(self, i);\n    }\n")],
  "i_out": [(L, "    int i;\n" + L.replace("for (int i = 0;", "for (i = 0;"))],
  "i_init": [(L, "    int i = 0;\n" + L.replace("for (int i = 0;", "for (;"))],
  "while": [(L, "    int i = 0;\n    do {\n" + "\n".join(L.split("\n")[1:-2]) + "\n    } while (++i < 10);\n")],
  "fontp": [(L, L.replace("        if (self->m_fonts[i].m_font != 0) {", "        UnkMessageFont_80049BC0 *font = &self->m_fonts[i];\n        if (font->m_font != 0) {").replace("self->m_fonts[i].", "font->"))],
}
