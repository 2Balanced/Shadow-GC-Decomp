"""Round 5 for TPjsEventActionManager::FindScenes (wave 39): the scene letter from an inline
function (its argument is a temporary, not the loop counter)."""
SCORE = ["FindScenes__22TPjsEventActionManagerFv"]
LIT = '"event' + chr(92) * 2 + 'event%04d_scene%c.one"'
OLD = "        sprintf(fileName, " + LIT + ", lbl_805EF2A0, (char)('A' + i));\n"
F = "void TPjsEventActionManager::FindScenes() {"
HELP = """inline char SceneLetterA_(int scene) { return 'A' + scene; }
inline char SceneLetterB_(int scene) { return (char)('A' + scene); }
inline int SceneLetterC_(int scene) { return (char)('A' + scene); }
inline char SceneLetterD_(char scene) { return 'A' + scene; }
inline void SceneFileName_(char *out, int event, int scene) { sprintf(out, """ + LIT + """, event, (char)('A' + scene)); }
"""
def sp(arg): return [(F, HELP + F), (OLD, "        sprintf(fileName, " + LIT + ", lbl_805EF2A0, " + arg + ");\n")]
REPL = {
  "A": sp("SceneLetterA_(i)"), "B": sp("SceneLetterB_(i)"), "C": sp("SceneLetterC_(i)"), "D": sp("SceneLetterD_(i)"),
  "file_fn": [(F, HELP + F), (OLD, "        SceneFileName_(fileName, lbl_805EF2A0, i);\n")],
}
