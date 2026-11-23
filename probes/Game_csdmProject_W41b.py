"""Wave 41: fn_8036F470 with copy propagation off (and map::find by value, MSL_MAP_FIND_BY_VALUE, a
temporary <map> option removed afterwards). Run against the combined Game/csdmProject.cpp before the
wave 41 split; a record only (best: prag_key and key_block, 5 rows)."""
SCORE = ["fn_8036F470"]
F = 'extern "C" RCPtr<CScene> fn_8036F470(CSceneList *list, const char *name) {'
N = "void CSceneList::Add(const RCPtr<CScene> &scene) {"
PRAG = [(F, "#pragma opt_propagation off\n" + F), (N, "#pragma opt_propagation reset\n" + N)]
NOBV = [("#define MSL_MAP_FIND_BY_VALUE\n", "")]
REPL = {
    "prag_bv": PRAG,
    "prag": PRAG + NOBV,
    "prag_key": PRAG + NOBV + [("    it = list->m_scenes.find(name);\n", "    const char *key = name;\n    it = list->m_scenes.find(key);\n")],
    "prag_bv_key": PRAG + [("    it = list->m_scenes.find(name);\n", "    const char *key = name;\n    it = list->m_scenes.find(key);\n")],
}
OLDB = "    CSceneList::SceneMap::const_iterator it;\n    it = list->m_scenes.find(name);\n"
REPL.update({
    "found_key": PRAG + NOBV + [(OLDB, "    CSceneList::SceneMap::const_iterator it;\n    CSceneList::SceneMap::iterator found;\n    const char *key = name;\n    found = list->m_scenes.find(key);\n    it = found;\n")],
    "found_key_np": NOBV + [(OLDB, "    CSceneList::SceneMap::const_iterator it;\n    CSceneList::SceneMap::iterator found;\n    const char *key = name;\n    found = list->m_scenes.find(key);\n    it = found;\n")],
    "key_block": PRAG + NOBV + [(OLDB, "    CSceneList::SceneMap::const_iterator it;\n    {\n        const char *key = name;\n        it = list->m_scenes.find(key);\n    }\n")],
    "key_after_it_init": PRAG + NOBV + [(OLDB, "    const char *key = name;\n    CSceneList::SceneMap::const_iterator it;\n    it = list->m_scenes.find(key);\n")],
})
