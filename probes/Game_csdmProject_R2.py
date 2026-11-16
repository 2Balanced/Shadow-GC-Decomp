"""fn_8036F470 of Game/csdmProject.cpp (wave 39): retail gives the key a stack slot between find's
result and the end() temporary - a temporary made at the call, as a conversion of the name to the
map's key type would be - where GC/2.7 put the parameter's own slot last."""
SCORE = ["fn_8036F470"]
OLD = 'extern "C" RCPtr<CScene> fn_8036F470(CSceneList *list, const char *name) {\n    CSceneList::SceneMap::const_iterator it;\n    it = list->m_scenes.find(name);\n'
REPL = {
  "char_param": [(OLD, OLD.replace("const char *name", "char *name"))],
  "void_param": [(OLD, OLD.replace("const char *name", "const void *name").replace("find(name)", "find((const char *)name)"))],
  "cast_temp": [(OLD, OLD.replace("find(name)", "find((const char *)name)"))],
  "key_local": [(OLD, OLD.replace("    it = list->m_scenes.find(name);\n", "    const char *key = name;\n    it = list->m_scenes.find(key);\n"))],
  "key_type_temp": [(OLD, OLD.replace("find(name)", "find(CSceneList::SceneMap::key_type(name))"))],
}
