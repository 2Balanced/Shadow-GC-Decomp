"""Round 5 for GiGameOver's constructor (wave 39): the name through a local gives `this` r31 but
takes r4 from the zero; shapes that might leave the three scratch registers as retail has them."""
SCORE = ["__ct__10GiGameOverFP4Task"]
NAME = '    m_name = "GiGameOver";\n'
C = "GiGameOver::GiGameOver(Task *parent) : Task(parent) {"
HELP = """inline const char *ClassName_() { return "GiGameOver"; }
inline void SetTaskName_(Task *task, const char *name) { task->m_name = name; }
inline void SetTaskName2_(Task *task, const char *name) { const char *n = name; task->m_name = n; }
inline const char *Same_(const char *s) { return s; }
"""
B = '    m_timer = 0.0f;\n    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n    m_name = "GiGameOver";\n'
REPL = {
  "local_from_getter": [(C, HELP + C), (NAME, '    const char *name = ClassName_();\n    m_name = name;\n')],
  "same_fn": [(C, HELP + C), (NAME, '    m_name = Same_("GiGameOver");\n')],
  "setter2": [(C, HELP + C), (NAME, '    SetTaskName2_(this, "GiGameOver");\n')],
  "block_local": [(NAME, '    {\n        const char *name = "GiGameOver";\n        m_name = name;\n    }\n')],
  "locals_all": [(B, '    float timer = 0.0f;\n    u8 yes = 0;\n    u8 decided = 0;\n    u8 waiting = 1;\n    const char *name = "GiGameOver";\n    m_timer = timer;\n    m_yes = yes;\n    m_decided = decided;\n    m_waiting = waiting;\n    m_name = name;\n')],
  "stores_then_local": [(B, '    const char *name;\n    m_timer = 0.0f;\n    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n    name = "GiGameOver";\n    m_name = name;\n')],
  "local_reassign": [(NAME, '    const char *name = 0;\n    name = "GiGameOver";\n    m_name = name;\n')],
  "local_ptrptr": [(NAME, '    const char **slot = &m_name;\n    *slot = "GiGameOver";\n')],
  "local_ref": [(NAME, '    const char *&slot = m_name;\n    slot = "GiGameOver";\n')],
}
