"""Wave 41: fn_8036F470, the key's stack slot between the find() result and the end() temporary
(retail): a key that is a temporary made at the call (a char * converted to the map's key).
Run against the combined Game/csdmProject.cpp before the wave 41 split (fn_8036F470 is in
Game/csdmSceneList.cpp now); all four kept 4 rows."""
SCORE = ["fn_8036F470"]
F = 'extern "C" RCPtr<CScene> fn_8036F470(CSceneList *list, const char *name) {'
REPL = {
    "char_name": [(F, 'extern "C" RCPtr<CScene> fn_8036F470(CSceneList *list, char *name) {')],
    "cast_key": [("    it = list->m_scenes.find(name);\n", "    it = list->m_scenes.find((const char *)name);\n")],
    "u8_name": [(F, 'extern "C" RCPtr<CScene> fn_8036F470(CSceneList *list, const u8 *name) {'),
                ("    it = list->m_scenes.find(name);\n", "    it = list->m_scenes.find((const char *)name);\n")],
    "void_name": [(F, 'extern "C" RCPtr<CScene> fn_8036F470(CSceneList *list, const void *name) {'),
                  ("    it = list->m_scenes.find(name);\n", "    it = list->m_scenes.find((const char *)name);\n")],
}
