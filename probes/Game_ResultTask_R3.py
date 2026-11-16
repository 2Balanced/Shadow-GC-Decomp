"""ResultTask::Init (wave 39, second pass): the path formatted by an inline function, so that the
format string's address is formed before the game state's."""
A = "bool ResultTask::Init() {\n"
S = '    sprintf(path, "CsdFiles/Result%s", lbl_8051D528[lbl_8057D6A0.m_language]);\n'
REPL = {
 "fmt_param": [(A, "inline void FormatLanguage(char *path, const char *format) {\n    sprintf(path, format, lbl_8051D528[lbl_8057D6A0.m_language]);\n}\n\n" + A), (S, '    FormatLanguage(path, "CsdFiles/Result%s");\n')],
 "fmt_only": [(A, "inline void FormatLanguage(char *path, const char *format, const char *suffix) {\n    sprintf(path, format, suffix);\n}\n\n" + A), (S, '    FormatLanguage(path, "CsdFiles/Result%s", lbl_8051D528[lbl_8057D6A0.m_language]);\n')],
 "fmt_lang": [(A, "inline void FormatLanguage(char *path, const char *format, s32 language) {\n    sprintf(path, format, lbl_8051D528[language]);\n}\n\n" + A), (S, '    FormatLanguage(path, "CsdFiles/Result%s", lbl_8057D6A0.m_language);\n')],
 "fmt_first": [(A, "inline void FormatLanguage(const char *format, char *path) {\n    sprintf(path, format, lbl_8051D528[lbl_8057D6A0.m_language]);\n}\n\n" + A), (S, '    FormatLanguage("CsdFiles/Result%s", path);\n')],
 "suffix_getter": [(A, "inline const char *Suffix() {\n    return lbl_8051D528[lbl_8057D6A0.m_language];\n}\ninline void FormatLanguage(char *path, const char *format) {\n    sprintf(path, format, Suffix());\n}\n\n" + A), (S, '    FormatLanguage(path, "CsdFiles/Result%s");\n')],
}
SCORE = ["Init__10ResultTaskFv"]
