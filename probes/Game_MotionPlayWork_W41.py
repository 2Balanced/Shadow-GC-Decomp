"""Wave 41: fn_8041FAD8 / fn_8041FB4C: retail gives the entry array r4 and the layer list r5."""
SRC = open("src/Game/MotionPlayWork.cpp", encoding="utf-8").read()
SCORE = ["fn_8041FAD8"]
s = SRC.index('extern "C" void fn_8041FAD8(void *data) {')
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
HEAD = 'extern "C" void fn_8041FAD8(void *data) {\n'
TAIL = """    if (1.0f != scale->x || 1.0f != scale->y || 1.0f != scale->z) {
        fn_8040B790(TopMatrix(), scale);
    }
}
"""
V = {}
V["list_local"] = HEAD + """    MotionChannelArgs *args = (MotionChannelArgs *)data;
    MotionChannelLayer *entry = args->m_entries;
    MotionLayerList *list = args->m_list;
    Vec3f *scale;

    entry += list->m_order[0];
    scale = (Vec3f *)entry->m_data;
""" + TAIL
V["list_first"] = HEAD + """    MotionChannelArgs *args = (MotionChannelArgs *)data;
    MotionLayerList *list = args->m_list;
    MotionChannelLayer *entry = args->m_entries;
    Vec3f *scale;

    entry += list->m_order[0];
    scale = (Vec3f *)entry->m_data;
""" + TAIL
V["index"] = HEAD + """    MotionChannelArgs *args = (MotionChannelArgs *)data;
    MotionChannelLayer *entry = args->m_entries;
    int index = args->m_list->m_order[0];
    Vec3f *scale;

    scale = (Vec3f *)entry[index].m_data;
""" + TAIL
V["subscript"] = HEAD + """    MotionChannelArgs *args = (MotionChannelArgs *)data;
    MotionChannelLayer *entry = args->m_entries;
    Vec3f *scale = (Vec3f *)entry[args->m_list->m_order[0]].m_data;
""" + TAIL
V["direct"] = HEAD + """    MotionChannelArgs *args = (MotionChannelArgs *)data;
    Vec3f *scale = (Vec3f *)args->m_entries[args->m_list->m_order[0]].m_data;
""" + TAIL
REPL = {}
for k, v in V.items():
    REPL[k] = [(FN, v)]
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + v + "#pragma opt_propagation reset\n")]
