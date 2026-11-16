"""Round 2 for fn_8036F470 of Game/csdmProject.cpp (wave 39): the lookup in an inline function, so
that the key is a local made while the call is evaluated."""
SCORE = ["fn_8036F470"]
F = 'extern "C" RCPtr<CScene> fn_8036F470(CSceneList *list, const char *name) {'
OLD = F + '\n    CSceneList::SceneMap::const_iterator it;\n    it = list->m_scenes.find(name);\n'
HELP = """inline CSceneList::SceneMap::iterator FindSceneA_(CSceneList *list, const char *name) { return list->m_scenes.find(name); }
inline CSceneList::SceneMap::iterator FindSceneB_(CSceneList::SceneMap &map, const char *name) { return map.find(name); }
inline CSceneList::SceneMap::iterator FindSceneC_(CSceneList *list, const char *name) { const char *key = name; return list->m_scenes.find(key); }
"""
REPL = {
  "A": [(OLD, HELP + F + '\n    CSceneList::SceneMap::const_iterator it;\n    it = FindSceneA_(list, name);\n')],
  "B": [(OLD, HELP + F + '\n    CSceneList::SceneMap::const_iterator it;\n    it = FindSceneB_(list->m_scenes, name);\n')],
  "C": [(OLD, HELP + F + '\n    CSceneList::SceneMap::const_iterator it;\n    it = FindSceneC_(list, name);\n')],
  "init": [(OLD, F + '\n    CSceneList::SceneMap::const_iterator it = list->m_scenes.find(name);\n')],
  "init_paren": [(OLD, F + '\n    CSceneList::SceneMap::const_iterator it(list->m_scenes.find(name));\n')],
}
