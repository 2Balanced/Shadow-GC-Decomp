"""Round 3 for TPjsEventActionManager::FindScenes (wave 39): the type of the scene counter."""
SCORE = ["FindScenes__22TPjsEventActionManagerFv"]
LIT = '"event' + chr(92) * 2 + 'event%04d_scene%c.one"'
OLD = "    for (int i = 0; i < 12; i++) {\n        sprintf(fileName, " + LIT + ", lbl_805EF2A0, (char)('A' + i));\n"
def v(decl, arg): return [(OLD, "    for (" + decl + " = 0; i < 12; i++) {\n        sprintf(fileName, " + LIT + ", lbl_805EF2A0, " + arg + ");\n")]
REPL = {
  "long_i": v("s32 i", "(char)('A' + i)"),
  "ulong_i": v("u32 i", "(char)('A' + i)"),
  "uint_i": v("unsigned int i", "(char)('A' + i)"),
  "short_i": v("s16 i", "(char)('A' + i)"),
  "int_cast_long": v("int i", "(char)('A' + (s32)i)"),
  "long_cast_int": v("s32 i", "(char)('A' + (int)i)"),
  "char_i_A": v("int i", "(char)i + 'A'"),
  "uchar": v("int i", "(u8)('A' + i)"),
  "long_i_first": v("s32 i", "(char)(i + 'A')"),
}
