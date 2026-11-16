"""Round 2 for TPjsEventActionManager::FindScenes (wave 39): forms of the scene letter."""
SCORE = ["FindScenes__22TPjsEventActionManagerFv"]
LIT = '"event' + chr(92) * 2 + 'event%04d_scene%c.one"'
OLD = "        sprintf(fileName, " + LIT + ", lbl_805EF2A0, (char)('A' + i));\n"
def sp(arg, pre=""): return [(OLD, pre + "        sprintf(fileName, " + LIT + ", lbl_805EF2A0, " + arg + ");\n")]
REPL = {
  "i_plus_A": sp("(char)(i + 'A')"),
  "letter_char": sp("letter", "        char letter = (char)('A' + i);\n"),
  "letter_char2": sp("letter", "        char letter = 'A' + i;\n"),
  "letter_int": sp("(char)letter", "        int letter = 'A' + i;\n"),
  "s8_cast": sp("(s8)('A' + i)"),
  "static_cast": sp("static_cast<char>('A' + i)"),
  "plus_65": sp("(char)(65 + i)"),
  "i_plus_65": sp("(char)(i + 65)"),
  "char_i_plus": sp("(char)((char)i + 'A')"),
  "char_ctor": sp("char('A' + i)"),
  "char_ctor_i": sp("char(i + 'A')"),
  "loop_char": [(OLD, "        sprintf(fileName, " + LIT + ", lbl_805EF2A0, (char)('A' + i));\n".replace("(char)('A' + i)", "(char)(c + i)")), ("    for (int i = 0; i < 12; i++) {", "    const int c = 'A';\n    for (int i = 0; i < 12; i++) {")],
}
