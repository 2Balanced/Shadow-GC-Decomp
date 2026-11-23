"""Wave 41: Screen::UpdateMap: more parameter copies under propagation off."""
SCORE = ["UpdateMap__Q37AdvReal10SelectMode6ScreenFb"]
H = "void Screen::UpdateMap(bool keys) {\n"
P0 = "#pragma opt_propagation off\n"
B0 = "    m_text.m_mode = 3;\n"
K = [("ShowStage(index, info->m_id, chosen, keys);", "ShowStage(index, info->m_id, chosen, k);"), ("ShowStage2(index, info->m_id, chosen, keys);", "ShowStage2(index, info->m_id, chosen, k);"), ("s_cursor.m_values.m_bossList && i == s_cursor.m_values.m_boss, keys);", "s_cursor.m_values.m_bossList && i == s_cursor.m_values.m_boss, k);")]
REPL = {
  "prag_self": [(H, P0 + H), (B0, "    Screen *self = this;\n    self->m_text.m_mode = 3;\n")],
  "prag_keys_after_mode": [(H, P0 + H), (B0, B0 + "    bool k = keys;\n")] + K,
  "prag_keys_u8": [(H, P0 + H), (B0, "    u8 k = keys;\n" + B0)] + K,
  "prag_keys_const": [(H, P0 + H), (B0, "    const bool k = keys;\n" + B0)] + K,
  "prag_keys_xy_first": [(H, P0 + H), (B0, "    int x;\n    int y;\n    bool k = keys;\n" + B0), ("    for (int x = 0; x < 6; x++) {\n        for (int y = 0; y < 5; y++) {", "    for (x = 0; x < 6; x++) {\n        for (y = 0; y < 5; y++) {")] + K,
}
