"""Mission::OnFailed (wave 39, after the unit was split): the registers around the sprintf."""
SCORE = ["OnFailed__Q27Mission7MissionFv"]
T = "extern const char *const lbl_8051D528[];"
G = "extern UnkGameState_8057D6A0 lbl_8057D6A0;"
O = '            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[lbl_8057D6A0.m_language]);\n'
HV = {
 "t_nc": [(T, "extern const char *lbl_8051D528[];")],
 "t_char": [(T, "extern char *lbl_8051D528[];")],
 "g_const": [(G, "extern const UnkGameState_8057D6A0 lbl_8057D6A0;")],
 "both": [(T, "extern const char *lbl_8051D528[];"), (G, "extern const UnkGameState_8057D6A0 lbl_8057D6A0;")],
 "t_ptr": [(T, "extern const char **lbl_8051D528_p;\n#define lbl_8051D528 lbl_8051D528_p")],
}
REPL = {k: [] for k in HV}
HDR = {k: {"Game/MissionManager.h": v} for k, v in HV.items()}
REPL["fmt_static"] = [(O, '            static const char format[] = "CsdFiles/MissionFailed%s";\n            sprintf(path, format, lbl_8051D528[lbl_8057D6A0.m_language]);\n')]
REPL["fmt_local"] = [(O, '            const char *format = "CsdFiles/MissionFailed%s";\n            sprintf(path, format, lbl_8051D528[lbl_8057D6A0.m_language]);\n')]
REPL["suffix_first"] = [(O, '            const char *suffix = lbl_8051D528[lbl_8057D6A0.m_language];\n            const char *format = "CsdFiles/MissionFailed%s";\n            sprintf(path, format, suffix);\n')]
REPL["lang_ptr"] = [(O, '            const s32 *language = &lbl_8057D6A0.m_language;\n            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[*language]);\n')]
REPL["table_ptr"] = [(O, '            const char *const *table = lbl_8051D528;\n            sprintf(path, "CsdFiles/MissionFailed%s", table[lbl_8057D6A0.m_language]);\n')]
REPL["ptr_arith"] = [(O, '            sprintf(path, "CsdFiles/MissionFailed%s", *(lbl_8051D528 + lbl_8057D6A0.m_language));\n')]
