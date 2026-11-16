"""ResultTask::Init and ResultTask::ShowRank (wave 39, second pass)."""
A = "bool ResultTask::Init() {\n"
S = '    sprintf(path, "CsdFiles/Result%s", lbl_8051D528[lbl_8057D6A0.m_language]);\n'
B = "void ResultTask::ShowRank() {\n"
T1 = "            if (lbl_8057D6A0.m_total <= fn_800CBAA4()->m_stages[lbl_8057D6A0.m_stage].m_rankThresholds[type][rank]) {\n"
T2 = "            if (lbl_8057D6A0.m_total >= fn_800CBAA4()->m_stages[lbl_8057D6A0.m_stage].m_rankThresholds[type][rank]) {\n"
TH = "inline s32 Threshold(int type, int rank) {\n    return fn_800CBAA4()->m_stages[lbl_8057D6A0.m_stage].m_rankThresholds[type][rank];\n}\n\n"
TH2 = "inline s32 *Thresholds(int type) {\n    return fn_800CBAA4()->m_stages[lbl_8057D6A0.m_stage].m_rankThresholds[type];\n}\n\n"
REPL = {
 "i_lang": [(A, "inline s32 Language() {\n    return lbl_8057D6A0.m_language;\n}\n\n" + A), (S, '    sprintf(path, "CsdFiles/Result%s", lbl_8051D528[Language()]);\n')],
 "i_suffix": [(A, "inline const char *Suffix() {\n    return lbl_8051D528[lbl_8057D6A0.m_language];\n}\n\n" + A), (S, '    sprintf(path, "CsdFiles/Result%s", Suffix());\n')],
 "i_suffix_arg": [(A, "inline const char *Suffix(s32 language) {\n    return lbl_8051D528[language];\n}\n\n" + A), (S, '    sprintf(path, "CsdFiles/Result%s", Suffix(lbl_8057D6A0.m_language));\n')],
 "local": [(S, '    const char *suffix = lbl_8051D528[lbl_8057D6A0.m_language];\n    sprintf(path, "CsdFiles/Result%s", suffix);\n')],
 "fmt_local": [(S, '    const char *format = "CsdFiles/Result%s";\n    sprintf(path, format, lbl_8051D528[lbl_8057D6A0.m_language]);\n')],
 "r_th": [(B, TH + B), (T1, "            if (lbl_8057D6A0.m_total <= Threshold(type, rank)) {\n"), (T2, "            if (lbl_8057D6A0.m_total >= Threshold(type, rank)) {\n")],
 "r_th_lit": [(B, TH + B), (T1, "            if (lbl_8057D6A0.m_total <= Threshold(1, rank)) {\n"), (T2, "            if (lbl_8057D6A0.m_total >= Threshold(type, rank)) {\n")],
 "r_th2": [(B, TH2 + B), (T1, "            if (lbl_8057D6A0.m_total <= Thresholds(type)[rank]) {\n"), (T2, "            if (lbl_8057D6A0.m_total >= Thresholds(type)[rank]) {\n")],
 "r_th2_lit": [(B, TH2 + B), (T1, "            if (lbl_8057D6A0.m_total <= Thresholds(1)[rank]) {\n"), (T2, "            if (lbl_8057D6A0.m_total >= Thresholds(type)[rank]) {\n")],
 "r_type_out": [("    int rank;\n    for (rank = 0; rank < 4; rank++) {\n        int type;\n", "    int rank;\n    int type;\n    for (rank = 0; rank < 4; rank++) {\n")],
 "r_type_u": [("    for (rank = 0; rank < 4; rank++) {\n        int type;\n", "    for (rank = 0; rank < 4; rank++) {\n        u32 type;\n")],
}
SCORE = ["Init__10ResultTaskFv", "ShowRank__10ResultTaskFv"]
