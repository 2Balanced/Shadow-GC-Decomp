"""Wave 40: fn_80015570's loop in an inline helper (the lever that gives `mr` for the
strength-reduced offset in TGiHdGage and EffectShimmerRW3)."""
SYMBOLS = {"fn_80015570": "RwFrame *fn_80015570(RwFrame *frame, void *data) {"}
LOOP = """    for (int i = 0; i < hierarchy->numNodes; i++) {
        if (hierarchy->nodeInfo[i].frame != 0) {
            Mtx34f *node = fn_80415E70(updater->m_motion, hierarchy->nodeInfo[i].nodeID);
            if (node != 0) {
                if (updater->m_head == 0) {
                    updater->m_head = updater->m_cursor = new UnkClumpUpdaterNode_80015A7C;
                } else {
                    updater->m_cursor->m_next = new UnkClumpUpdaterNode_80015A7C;
                    updater->m_cursor = updater->m_cursor->m_next;
                }
                if (updater->m_cursor != 0) {
                    updater->m_cursor->m_frame = hierarchy->nodeInfo[i].frame;
                    updater->m_cursor->m_id = hierarchy->nodeInfo[i].nodeID;
                    updater->m_cursor->m_node = node;
                    updater->m_cursor->m_parent = 0;
                    updater->m_cursor->m_next = 0;
                }
            }
        }
    }
"""
XTRA = {"fn_80015570": (
    "inline void ListA_(UnkClumpUpdater_80015A7C *updater, RpHAnimHierarchy *hierarchy) {\n" + LOOP + "}\n"
    "inline void ListB_(RpHAnimHierarchy *hierarchy, UnkClumpUpdater_80015A7C *updater) {\n" + LOOP + "}\n"
    "inline void ListC_(UnkClumpUpdater_80015A7C *updater, RwFrame *frame) {\n"
    "    RpHAnimHierarchy *hierarchy = fn_8044757C(frame);\n    if (hierarchy != 0) {\n" + LOOP + "    }\n}\n"
    "inline void ListD_(RwFrame *frame, UnkClumpUpdater_80015A7C *updater) {\n"
    "    if (updater->m_motion != 0) {\n    RpHAnimHierarchy *hierarchy = fn_8044757C(frame);\n    if (hierarchy != 0) {\n" + LOOP + "    }\n    }\n}\n"
    "inline void ListE_(RpHAnimHierarchy *hierarchy, void *data) {\n"
    "    UnkClumpUpdater_80015A7C *updater = static_cast<UnkClumpUpdater_80015A7C *>(data);\n" + LOOP + "}\n"
)}
HEAD = """    UnkClumpUpdater_80015A7C *updater = static_cast<UnkClumpUpdater_80015A7C *>(data);
    if (updater->m_motion != 0) {
        RpHAnimHierarchy *hierarchy = fn_8044757C(frame);
        if (hierarchy != 0) {
"""
TAIL = """        }
    }
    fn_8048752C(frame, fn_80015570, data);
    return frame;
"""
BODIES = {"fn_80015570": {
    "helperA": HEAD + "            ListA_(updater, hierarchy);\n" + TAIL,
    "helperB": HEAD + "            ListB_(hierarchy, updater);\n" + TAIL,
    "helperC": """    UnkClumpUpdater_80015A7C *updater = static_cast<UnkClumpUpdater_80015A7C *>(data);
    if (updater->m_motion != 0) {
        ListC_(updater, frame);
    }
    fn_8048752C(frame, fn_80015570, data);
    return frame;
""",
    "helperD": """    ListD_(frame, static_cast<UnkClumpUpdater_80015A7C *>(data));
    fn_8048752C(frame, fn_80015570, data);
    return frame;
""",
    "helperE": """    if (static_cast<UnkClumpUpdater_80015A7C *>(data)->m_motion != 0) {
        RpHAnimHierarchy *hierarchy = fn_8044757C(frame);
        if (hierarchy != 0) {
            ListE_(hierarchy, data);
        }
    }
    fn_8048752C(frame, fn_80015570, data);
    return frame;
""",
}}
