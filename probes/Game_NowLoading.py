"""Source shapes for NowLoadingScreen::Load (wave 40): retail forms the game state's address in r6
(the register of sprintf's fourth argument) and the project name's in r5; GC/2.7 forms the game
state's in r5 and moves the project name there from r6."""
SCORE = ["Load__16NowLoadingScreenFv"]
OLD = "    sprintf(path, \"%s%s\", s_projectName, lbl_8051D528[lbl_8057D6A0.m_language]);\n"
T_OLD = "extern const char *lbl_8051D528[];\n"
S_OLD = "extern UnkGameState_8057D6A0 lbl_8057D6A0;\n"
H = """inline s32 Language_() { return lbl_8057D6A0.m_language; }
inline const char *Suffix_() { return lbl_8051D528[lbl_8057D6A0.m_language]; }
inline const char *SuffixOf_(s32 language) { return lbl_8051D528[language]; }
inline const char *ProjectName_() { return s_projectName; }
inline UnkGameState_8057D6A0 *State_() { return &lbl_8057D6A0; }
bool NowLoadingScreen::Load() {"""
HP = [("bool NowLoadingScreen::Load() {", H)]


def v(new):
    return HP + [(OLD, new)]


REPL = {
    "const_table": [(T_OLD, "extern const char *const lbl_8051D528[];\n")],
    "const_state": [(S_OLD, "extern const UnkGameState_8057D6A0 lbl_8057D6A0;\n")],
    "const_both": [(T_OLD, "extern const char *const lbl_8051D528[];\n"),
                   (S_OLD, "extern const UnkGameState_8057D6A0 lbl_8057D6A0;\n")],
    "lang_inl": v("    sprintf(path, \"%s%s\", s_projectName, lbl_8051D528[Language_()]);\n"),
    "suffix_inl": v("    sprintf(path, \"%s%s\", s_projectName, Suffix_());\n"),
    "suffixof": v("    sprintf(path, \"%s%s\", s_projectName, SuffixOf_(lbl_8057D6A0.m_language));\n"),
    "suffixof2": v("    sprintf(path, \"%s%s\", s_projectName, SuffixOf_(Language_()));\n"),
    "name_inl": v("    sprintf(path, \"%s%s\", ProjectName_(), lbl_8051D528[lbl_8057D6A0.m_language]);\n"),
    "name_suffix": v("    sprintf(path, \"%s%s\", ProjectName_(), Suffix_());\n"),
    "state_inl": v("    sprintf(path, \"%s%s\", s_projectName, lbl_8051D528[State_()->m_language]);\n"),
    "suffix_local": v("    const char *suffix = lbl_8051D528[lbl_8057D6A0.m_language];\n    sprintf(path, \"%s%s\", s_projectName, suffix);\n"),
    "lang_local": v("    s32 language = lbl_8057D6A0.m_language;\n    sprintf(path, \"%s%s\", s_projectName, lbl_8051D528[language]);\n"),
    "name_local": v("    const char *name = s_projectName;\n    sprintf(path, \"%s%s\", name, lbl_8051D528[lbl_8057D6A0.m_language]);\n"),
    "name_ptr": [("static const char s_projectName[] = \"csdFiles/Loading\";", "static const char *const s_projectName = \"csdFiles/Loading\";")],
    "name_lit": v("    sprintf(path, \"%s%s\", \"csdFiles/Loading\", lbl_8051D528[lbl_8057D6A0.m_language]);\n"),
}
