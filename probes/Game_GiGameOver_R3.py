"""Round 3 for GiGameOver's constructor (wave 39): the name through a pointer of its own."""
SCORE = ["__ct__10GiGameOverFP4Task"]
NAME = '    m_name = "GiGameOver";\n'
C = "GiGameOver::GiGameOver(Task *parent) : Task(parent) {"
REPL = {
  "ns_const_ptr": [(C, 'static const char *const kClassName = "GiGameOver";\n' + C), (NAME, "    m_name = kClassName;\n")],
  "ns_array": [(C, 'static const char kClassName[] = "GiGameOver";\n' + C), (NAME, "    m_name = kClassName;\n")],
  "addr_index": [(NAME, '    m_name = &"GiGameOver"[0];\n')],
  "plus_zero": [(NAME, '    m_name = "GiGameOver" + 0;\n')],
  "cast": [(NAME, '    m_name = static_cast<const char *>("GiGameOver");\n')],
  "chain_local": [(NAME, '    const char *name;\n    m_name = name = "GiGameOver";\n')],
  "local_const": [(NAME, '    const char *const name = "GiGameOver";\n    m_name = name;\n')],
  "local_before_timer": [(NAME, ''), ("    m_timer = 0.0f;\n", '    m_name = "GiGameOver";\n    m_timer = 0.0f;\n')],
  "name_after_instance": [(NAME, ''), ("    s_instance = this;\n", '    s_instance = this;\n    m_name = "GiGameOver";\n')],
  "ctor_init_list": [(NAME, ''), (C, "GiGameOver::GiGameOver(Task *parent) : Task(parent), m_timer(0.0f) {")],
}
