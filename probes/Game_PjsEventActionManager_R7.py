"""Wave 40: FindScenes, a last round (the inline-getter lever of AdvOption's UpdateList)."""
SCORE = ["FindScenes__22TPjsEventActionManagerFv"]
F = "void TPjsEventActionManager::FindScenes() {"
S = "        sprintf(fileName, \"event\\\\event%04d_scene%c.one\", lbl_805EF2A0, (char)('A' + i));\n"
L = "    for (int i = 0; i < 12; i++) {\n"
H = "inline int EventId_() { return lbl_805EF2A0; }\ninline const char *SceneFormat_() { return \"event\\\\event%04d_scene%c.one\"; }\ninline int FirstScene_() { return 0; }\n" + F
REPL = {
  "event_inl": [(F, H), (S, S.replace("lbl_805EF2A0", "EventId_()"))],
  "format_inl": [(F, H), (S, S.replace("\"event\\\\event%04d_scene%c.one\"", "SceneFormat_()"))],
  "both_inl": [(F, H), (S, S.replace("lbl_805EF2A0", "EventId_()").replace("\"event\\\\event%04d_scene%c.one\"", "SceneFormat_()"))],
  "first_inl": [(F, H), (L, "    for (int i = FirstScene_(); i < 12; i++) {\n")],
  "i_first": [("    char fileName[0x40];\n    m_sceneIndex = 0;", "    int i;\n    char fileName[0x40];\n    m_sceneIndex = 0;"), (L, "    for (i = 0; i < 12; i++) {\n")],
  "while": [(L, "    int i = 0;\n    while (i < 12) {\n"), ("            m_sceneCount++;\n        }\n    }", "            m_sceneCount++;\n        }\n        i++;\n    }")],
  "exists_local": [("        if (FileExists(fileName)) {", "        bool exists = FileExists(fileName);\n        if (exists) {")],
  "count_inl": [(F, "inline void AddScene_(TPjsEventActionManager *m, int i) { m->m_scenes[m->m_sceneCount] = i; m->m_sceneCount++; }\n" + F), ("            m_scenes[m_sceneCount] = i;\n            m_sceneCount++;\n", "            AddScene_(this, i);\n")],
  "u32_cast": [(S, S.replace("(char)('A' + i)", "(char)('A' + (u32)i)"))],
  "u32_s32": [(S, S.replace("(char)('A' + i)", "(char)('A' + (s32)(u32)i)"))],
  "ptr_idx": [(S, S.replace("(char)('A' + i)", "\"ABCDEFGHIJKL\"[i]"))],
  "scene_store_cast": [("            m_scenes[m_sceneCount] = i;", "            m_scenes[m_sceneCount] = (u32)i;")],
  "lt_u": [(L, "    for (int i = 0; (u32)i < 12; i++) {\n")],
  "ne": [(L, "    for (int i = 0; i != 12; i++) {\n")],
}
