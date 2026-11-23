"""Wave 41: Mission::OnFailed's sprintf (retail: the game state's address in r5, the format's
address last in r4) with a named language local and '#pragma opt_propagation off' (the shape
that matched NowLoadingScreen::Load)."""
SCORE = ["OnFailed__Q27Mission7MissionFv"]
H = "void Mission::OnFailed() {\n"
P0 = "#pragma opt_propagation off\n"
S = """            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[lbl_8057D6A0.m_language]);
"""
REPL = {
  "lang": [(H, P0 + H), (S, """            s32 language = lbl_8057D6A0.m_language;
            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[language]);
""")],
  "lang_top": [(H, P0 + H), ("            char path[0x40];\n", "            s32 language;\n            char path[0x40];\n"), (S, """            language = lbl_8057D6A0.m_language;
            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[language]);
""")],
  "suffix": [(H, P0 + H), (S, """            const char *suffix = lbl_8051D528[lbl_8057D6A0.m_language];
            sprintf(path, "CsdFiles/MissionFailed%s", suffix);
""")],
  "lang_before_memset": [(H, P0 + H), ("            memset(path, 0, sizeof(path));\n", "            s32 language = lbl_8057D6A0.m_language;\n            memset(path, 0, sizeof(path));\n"), (S, """            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[language]);
""")],
}
