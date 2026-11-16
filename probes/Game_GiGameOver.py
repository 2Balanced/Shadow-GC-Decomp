"""GiGameOver's constructor (wave 39): experiments on what gives `this` r31 and the .rodata base
r30 (retail) instead of the reverse (GC/2.7 with the committed source)."""
SCORE = ["__ct__10GiGameOverFP4Task"]
NAME = '    m_name = "GiGameOver";\n'
REPL = {
  "E_no_name": [(NAME, "")],
  "E_name_late": [(NAME, ""), ("    s_instance = this;\n", "    s_instance = this;\n" + NAME)],
  "E_name_last": [(NAME, ""), ("    m_render = fn_8004E24C(", NAME + "    m_render = fn_8004E24C(")],
  "E_no_new_strings": [("        scene = fn_8036D660(m_project, \"Loop\");", "        scene = fn_8036D660(m_project, s_fileName);")],
  "name_first": [(NAME, ""), ("    m_timer = 0.0f;\n", NAME + "    m_timer = 0.0f;\n")],
  "name_static": [(NAME, "    m_name = s_className;\n"), ("GiGameOver::GiGameOver(Task *parent) : Task(parent) {", "static const char s_className[] = \"GiGameOver\";\nGiGameOver::GiGameOver(Task *parent) : Task(parent) {")],
}
