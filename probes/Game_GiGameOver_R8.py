"""Round 8 for GiGameOver's constructor (wave 39): the name as an expression temporary."""
SCORE = ["__ct__10GiGameOverFP4Task"]
NAME = '    m_name = "GiGameOver";\n'
C = "GiGameOver::GiGameOver(Task *parent) : Task(parent) {"
HELP = """struct NameHolder_ { const char *m; NameHolder_(const char *s) : m(s) {} operator const char *() const { return m; } };
inline const char *PassName_(const char *s) { const char *t = s; return t; }
template <class T> inline T Pass_(T x) { return x; }
"""
REPL = {
  "comma": [(NAME, '    m_name = (0, "GiGameOver");\n')],
  "int_cast": [(NAME, '    m_name = (const char *)(u32)"GiGameOver";\n')],
  "holder": [(C, HELP + C), (NAME, '    m_name = NameHolder_("GiGameOver");\n')],
  "pass_name": [(C, HELP + C), (NAME, '    m_name = PassName_("GiGameOver");\n')],
  "pass_tmpl": [(C, HELP + C), (NAME, '    m_name = Pass_("GiGameOver");\n')],
  "pass_tmpl2": [(C, HELP + C), (NAME, '    m_name = Pass_<const char *>("GiGameOver");\n')],
  "ternary_this": [(NAME, '    m_name = this ? "GiGameOver" : 0;\n')],
  "array_ref": [(NAME, '    const char (&name)[11] = "GiGameOver";\n    m_name = name;\n')],
}
