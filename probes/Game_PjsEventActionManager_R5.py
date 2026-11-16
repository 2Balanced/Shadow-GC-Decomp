"""Round 4 for TPjsEventActionManager::FindScenes (wave 39): `(char)i + 'A'` (an int argument)
gives retail's registers but extends before the add; int-typed forms that add first."""
SCORE = ["FindScenes__22TPjsEventActionManagerFv"]
LIT = '"event' + chr(92) * 2 + 'event%04d_scene%c.one"'
OLD = "        sprintf(fileName, " + LIT + ", lbl_805EF2A0, (char)('A' + i));\n"
def sp(arg, pre=""): return [(OLD, pre + "        sprintf(fileName, " + LIT + ", lbl_805EF2A0, " + arg + ");\n")]
REPL = {
  "int_of_char": sp("(int)(char)('A' + i)"),
  "plus_zero": sp("(char)('A' + i) + 0"),
  "unary_plus": sp("+(char)('A' + i)"),
  "int_local": sp("letter", "        int letter = (char)('A' + i);\n"),
  "s32_of_char": sp("(s32)(char)('A' + i)"),
  "int_of_s8": sp("(int)(s8)('A' + i)"),
  "int_of_char_iA": sp("(int)(char)(i + 'A')"),
}
