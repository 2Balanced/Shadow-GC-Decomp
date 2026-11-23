"""UnkObject_801E67EC::Load (wave 40, round 2): retail keeps the string pool base in r31 and this in
r30; GC/2.7 the other way round. Game/GiGameOver.cpp flips the same pair the other way by routing a
string through a value of its own; here this goes through inline accessors, or a string does."""
SCORE = ["Load__18UnkObject_801E67ECFi"]
CLS = """    /* 0x00 */ TPjsCsd m_csd;
"""
CSD = """    TPjsCsd &Csd() { return m_csd; }
    /* 0x00 */ TPjsCsd m_csd;
"""
SET = """    void SetState(s32 state) { m_state = state; }
    /* 0x00 */ TPjsCsd m_csd;
"""
GET = """    s32 State() const { return m_state; }
    /* 0x00 */ TPjsCsd m_csd;
"""
BODY = """    if (fn_8003F938(m_csd, "csdFiles/stageTitle/StageTitle", path)) {
        strcpy(m_csd.m_sceneName, "StageTitle");
        fn_8003E414(m_csd, 1, 1);
        fn_8003E414(m_csd, 2, 0);
        fn_8003FB10(m_csd);
        m_state = 1;
        fn_8003FE2C(m_csd);
"""
BODY_CSD = BODY.replace("(m_csd", "(Csd()").replace("strcpy(m_csd.", "strcpy(Csd().")
FMT = """    sprintf(path, "csdFiles/stageTitle/StageTitle%04d%s.txd", stage,
            lbl_8051D528[lbl_8057D6A0.m_language]);
"""
REPL = {
  "csd_acc": [(CLS, CSD), (BODY, BODY_CSD)],
  "csd_acc_partial": [(CLS, CSD), (BODY, BODY.replace("fn_8003F938(m_csd", "fn_8003F938(Csd()"))],
  "set_state": [(CLS, SET), ("    m_state = 4;\n", "    SetState(4);\n"), ("        m_state = 1;\n", "        SetState(1);\n")],
  "get_state": [(CLS, GET), ("    if (m_state != 0) {\n        return false;\n    }\n    m_state = 4;\n", "    if (State() != 0) {\n        return false;\n    }\n    m_state = 4;\n")],
  "all_acc": [(CLS, CSD.replace("    TPjsCsd &Csd()", "    s32 State() const { return m_state; }\n    void SetState(s32 state) { m_state = state; }\n    TPjsCsd &Csd()")),
              (BODY, BODY_CSD.replace("        m_state = 1;\n", "        SetState(1);\n")),
              ("    if (m_state != 0) {\n        return false;\n    }\n    m_state = 4;\n", "    if (State() != 0) {\n        return false;\n    }\n    SetState(4);\n")],
  "fmt_comma": [(FMT, FMT.replace('path, "csdFiles', 'path, (0, "csdFiles').replace('.txd",', '.txd"),'))],
  "dir_comma": [(BODY, BODY.replace('m_csd, "csdFiles/stageTitle/StageTitle", path', 'm_csd, (0, "csdFiles/stageTitle/StageTitle"), path'))],
  "scene_comma": [(BODY, BODY.replace('"StageTitle");', '(0, "StageTitle"));'))],
  "lang_inline": [("extern const char *const lbl_8051D528[];\n", "extern const char *const lbl_8051D528[];\ninline const char *LanguageSuffix() { return lbl_8051D528[lbl_8057D6A0.m_language]; }\n"),
                  (FMT, FMT.replace("lbl_8051D528[lbl_8057D6A0.m_language]", "LanguageSuffix()"))],
  "exists_local": [("    if (!FileExists(path)) {\n", "    bool exists = FileExists(path);\n    if (!exists) {\n")],
  "csd_param": [(BODY, BODY.replace("strcpy(m_csd.m_sceneName", "TPjsCsd &csd = m_csd;\n        strcpy(csd.m_sceneName"))],
}
