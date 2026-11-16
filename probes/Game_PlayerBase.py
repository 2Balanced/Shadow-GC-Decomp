"""Source shapes for UnkWords_8007A4D4's constructor in Game/PlayerBase.cpp (wave 39).

Retail: li r4,0 (offset); li r0,3; mr r5,r4 (the zero is a copy of the offset).  The header's
constructor calls the inline Clear() and gives li r5,0; mr r4,r5."""
SCORE = ["__ct__17UnkWords_8007A4D4Fv"]
H = "Game/UnkWords_8007A4D4.h"
OLD = """    UnkWords_8007A4D4() {
        Clear();
    }"""
def ctor(body, extra=""):
    return {H: [(OLD, extra + "    UnkWords_8007A4D4() {\n" + body + "    }")]}
HDR = {
    "word_ref": ctor("        for (int i = 0; i < 3; i++) {\n            Word(i) = 0;\n        }\n", "    u32 &Word(int i) { return m_words[i]; }\n"),
    "set_word": ctor("        for (int i = 0; i < 3; i++) {\n            SetWord(i, 0);\n        }\n", "    void SetWord(int i, u32 v) { m_words[i] = v; }\n"),
    "clear_word": ctor("        for (int i = 0; i < 3; i++) {\n            ClearWord(i);\n        }\n", "    void ClearWord(int i) { m_words[i] = 0; }\n"),
    "clear_val": ctor("        ClearTo(0);\n", "    void ClearTo(u32 v) { for (int i = 0; i < 3; i++) { m_words[i] = v; } }\n"),
    "static_clear": ctor("        ClearWords(m_words);\n", "    static void ClearWords(u32 *w) { for (int i = 0; i < 3; i++) { w[i] = 0; } }\n"),
    "static_clear_n": ctor("        ClearWords(m_words, 3);\n", "    static void ClearWords(u32 *w, int n) { for (int i = 0; i < n; i++) { w[i] = 0; } }\n"),
    "begin_ptr": ctor("        for (int i = 0; i < 3; i++) {\n            Begin()[i] = 0;\n        }\n", "    u32 *Begin() { return m_words; }\n"),
    "size_fn": ctor("        for (int i = 0; i < Size(); i++) {\n            m_words[i] = 0;\n        }\n", "    static int Size() { return 3; }\n"),
    "zero_fn": ctor("        for (int i = 0; i < 3; i++) {\n            m_words[i] = Zero();\n        }\n", "    static u32 Zero() { return 0; }\n"),
}
