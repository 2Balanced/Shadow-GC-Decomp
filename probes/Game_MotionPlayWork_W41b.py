"""Wave 41: fn_8041FB4C, the layer list as a named local after the entry with copy propagation off
(what matched fn_8041FAD8)."""
SRC = open("src/Game/MotionPlayWork.cpp", encoding="utf-8").read()
SCORE = ["fn_8041FB4C"]
s = SRC.index('extern "C" void fn_8041FB4C(void *data) {')
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
NEW = FN.replace("    MotionChannelLayer *entry = args->m_entries;\n", "    MotionChannelLayer *entry = args->m_entries;\n    MotionLayerList *list = args->m_list;\n").replace("    entry += args->m_list->m_order[0];\n", "    entry += list->m_order[0];\n")
NEW2 = FN.replace("    MotionChannelLayer *entry = args->m_entries;\n    Vec3f *angles;\n    int x;\n    int y;\n    int z;\n", "    MotionChannelLayer *entry = args->m_entries;\n    Vec3f *angles;\n    int x;\n    int y;\n    int z;\n    MotionLayerList *list = args->m_list;\n").replace("    entry += args->m_list->m_order[0];\n", "    entry += list->m_order[0];\n")
REPL = {
  "prag_list_local": [(FN, "#pragma opt_propagation off\n" + NEW + "#pragma opt_propagation reset\n")],
  "list_local": [(FN, NEW)],
  "prag_list_last": [(FN, "#pragma opt_propagation off\n" + NEW2 + "#pragma opt_propagation reset\n")],
}
