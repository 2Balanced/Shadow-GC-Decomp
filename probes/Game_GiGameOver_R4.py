"""Round 4 for GiGameOver's constructor (wave 39): other named locals."""
SCORE = ["__ct__10GiGameOverFP4Task"]
REPL = {
  "self_local": [("    s_instance = this;\n", "    GiGameOver *self = this;\n    s_instance = self;\n")],
  "zero_f": [("    m_timer = 0.0f;\n", "    float zero = 0.0f;\n    m_timer = zero;\n")],
  "off_local": [("    m_yes = 0;\n    m_decided = 0;\n", "    u8 off = 0;\n    m_yes = off;\n    m_decided = off;\n")],
  "on_local": [("    m_waiting = 1;\n", "    u8 on = 1;\n    m_waiting = on;\n")],
  "off_on": [("    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n", "    u8 off = 0;\n    u8 on = 1;\n    m_yes = off;\n    m_decided = off;\n    m_waiting = on;\n")],
  "chain_zero": [("    m_yes = 0;\n    m_decided = 0;\n", "    m_yes = m_decided = 0;\n")],
  "chain_zero2": [("    m_yes = 0;\n    m_decided = 0;\n", "    m_decided = m_yes = 0;\n")],
  "bool_false": [("    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n", "    m_yes = false;\n    m_decided = false;\n    m_waiting = true;\n")],
  "owner_local": [("    m_render = fn_8004E24C(fn_80046178()->field_0x3C);", "    UnkRenderOwner_80046178 *owner = fn_80046178();\n    m_render = fn_8004E24C(owner->field_0x3C);")],
  "project_local": [("    m_project = new Chao::CSD::CProject();\n", "    Chao::CSD::CProject *project = new Chao::CSD::CProject();\n    m_project = project;\n")],
  "name_local_off_on": [('    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n    m_name = "GiGameOver";\n', '    u8 off = 0;\n    u8 on = 1;\n    const char *name = "GiGameOver";\n    m_yes = off;\n    m_decided = off;\n    m_waiting = on;\n    m_name = name;\n')],
  "name_local_first": [('    m_timer = 0.0f;\n    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n    m_name = "GiGameOver";\n', '    const char *name = "GiGameOver";\n    m_timer = 0.0f;\n    m_yes = 0;\n    m_decided = 0;\n    m_waiting = 1;\n    m_name = name;\n')],
}
