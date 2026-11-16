"""Mission::OnFailed (wave 39, second pass): the registers of the game state and the suffix table."""
SCORE = ["OnFailed__Q27Mission7MissionFv"]
O = '            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[lbl_8057D6A0.m_language]);\n'
A = "void Mission::OnFailed() {"
def v(new, pre=""):
    r = [(O, new)]
    if pre:
        r.append((A, pre + A))
    return r
REPL = {
 "inl_lang": v('            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[GetLanguage()]);\n', "inline int GetLanguage() {\n    return lbl_8057D6A0.m_language;\n}\n"),
 "inl_suffix": v('            sprintf(path, "CsdFiles/MissionFailed%s", GetSuffix());\n', "inline const char *GetSuffix() {\n    return lbl_8051D528[lbl_8057D6A0.m_language];\n}\n"),
 "inl_suffix_arg": v('            sprintf(path, "CsdFiles/MissionFailed%s", GetSuffix(lbl_8057D6A0.m_language));\n', "inline const char *GetSuffix(int language) {\n    return lbl_8051D528[language];\n}\n"),
 "inl_both": v('            sprintf(path, "CsdFiles/MissionFailed%s", GetSuffix(GetLanguage()));\n', "inline int GetLanguage() {\n    return lbl_8057D6A0.m_language;\n}\ninline const char *GetSuffix(int language) {\n    return lbl_8051D528[language];\n}\n"),
 "loc_lang": v('            int language = lbl_8057D6A0.m_language;\n            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[language]);\n'),
 "loc_suffix": v('            const char *suffix = lbl_8051D528[lbl_8057D6A0.m_language];\n            sprintf(path, "CsdFiles/MissionFailed%s", suffix);\n'),
 "ptr_state": v('            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[(&lbl_8057D6A0)->m_language]);\n'),
 "inl_state": v('            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[GetState()->m_language]);\n', "inline UnkGameState_8057D6A0 *GetState() {\n    return &lbl_8057D6A0;\n}\n"),
 "inl_table": v('            sprintf(path, "CsdFiles/MissionFailed%s", GetTable()[lbl_8057D6A0.m_language]);\n', "inline const char **GetTable() {\n    return (const char **)lbl_8051D528;\n}\n"),
}
