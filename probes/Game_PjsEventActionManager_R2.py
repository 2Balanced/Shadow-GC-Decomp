"""Source shapes for TPjsEventActionManager::FindScenes (wave 39): retail keeps the format string
in r31, the scene number in r30 and this in r29 - the order GC/2.7 gives declared locals (first
declared, highest register) with this after them."""
SCORE = ["FindScenes__22TPjsEventActionManagerFv"]
LIT = '"event' + chr(92) * 2 + 'event%04d_scene%c.one"'
OLD = "    m_sceneCount = 0;\n    m_nextScene = 0;\n    for (int i = 0; i < 12; i++) {\n        sprintf(fileName, " + LIT + ", lbl_805EF2A0, (char)('A' + i));\n"
FMT = "    const char *format = " + LIT + ";\n"
SP = "        sprintf(fileName, format, lbl_805EF2A0, (char)('A' + i));\n"
REPL = {
  "fmt_before_loop": [(OLD, "    m_sceneCount = 0;\n    m_nextScene = 0;\n" + FMT + "    for (int i = 0; i < 12; i++) {\n" + SP)],
  "fmt_between": [(OLD, "    m_sceneCount = 0;\n" + FMT + "    m_nextScene = 0;\n    for (int i = 0; i < 12; i++) {\n" + SP)],
  "fmt_before_stores": [(OLD, FMT + "    m_sceneCount = 0;\n    m_nextScene = 0;\n    for (int i = 0; i < 12; i++) {\n" + SP)],
  "fmt_top": [("    char fileName[0x40];\n    m_sceneIndex = 0;", "    char fileName[0x40];\n" + FMT + "    m_sceneIndex = 0;"), (OLD, "    m_sceneCount = 0;\n    m_nextScene = 0;\n    for (int i = 0; i < 12; i++) {\n" + SP)],
  "fmt_i_decl": [(OLD, "    m_sceneCount = 0;\n    m_nextScene = 0;\n    const char *format;\n    int i;\n    for (format = " + LIT + ", i = 0; i < 12; i++) {\n" + SP)],
}
