"""ResultTask::Init (wave 40): the language or the suffix read in a loop of one pass, named locals."""
O = "    sprintf(path, \"CsdFiles/Result%s\", lbl_8051D528[lbl_8057D6A0.m_language]);\n"
F = "\"CsdFiles/Result%s\""
REPL = {
 "for_lang": [(O, "    int lang;\n    for (int n = 0; n < 1; n++) {\n        lang = lbl_8057D6A0.m_language;\n    }\n    sprintf(path, " + F + ", lbl_8051D528[lang]);\n")],
 "for_suffix": [(O, "    const char *suffix;\n    for (int n = 0; n < 1; n++) {\n        suffix = lbl_8051D528[lbl_8057D6A0.m_language];\n    }\n    sprintf(path, " + F + ", suffix);\n")],
 "for_all": [(O, "    for (int n = 0; n < 1; n++) {\n    " + O + "    }\n")],
 "for_fmt": [(O, "    const char *fmt;\n    for (int n = 0; n < 1; n++) {\n        fmt = " + F + ";\n    }\n    sprintf(path, fmt, lbl_8051D528[lbl_8057D6A0.m_language]);\n")],
 "state_ptr": [(O, "    UnkGameState_8057D6A0 *state = &lbl_8057D6A0;\n    sprintf(path, " + F + ", lbl_8051D528[state->m_language]);\n")],
 "tbl_ptr": [(O, "    const char **table = lbl_8051D528;\n    sprintf(path, " + F + ", table[lbl_8057D6A0.m_language]);\n")],
 "for_tbl": [(O, "    const char **table;\n    for (int n = 0; n < 1; n++) {\n        table = lbl_8051D528;\n    }\n    sprintf(path, " + F + ", table[lbl_8057D6A0.m_language]);\n")],
 "for_state": [(O, "    UnkGameState_8057D6A0 *state;\n    for (int n = 0; n < 1; n++) {\n        state = &lbl_8057D6A0;\n    }\n    sprintf(path, " + F + ", lbl_8051D528[state->m_language]);\n")],
}
SCORE = ["Init__10ResultTaskFv"]
