"""Source shapes for fn_80015570 of Game/ClumpUpdater.cpp (wave 39): retail copies the zeroed
counter into the byte offset (mr r27,r30); GC/2.7 emits li r27,0."""
SYMBOLS = {"fn_80015570": "RwFrame *fn_80015570(RwFrame *frame, void *data) {"}
XTRA = {"fn_80015570": """inline RpHAnimNodeInfo &NodeAt_(RpHAnimHierarchy *h, int i) { return h->nodeInfo[i]; }
inline RwFrame *NodeFrame_(RpHAnimHierarchy *h, int i) { return h->nodeInfo[i].frame; }
inline int NodeId_(RpHAnimHierarchy *h, int i) { return h->nodeInfo[i].nodeID; }
inline int NumNodes_(RpHAnimHierarchy *h) { return h->numNodes; }
"""}
HEAD = """    UnkClumpUpdater_80015A7C *updater = static_cast<UnkClumpUpdater_80015A7C *>(data);
    if (updater->m_motion != 0) {
        RpHAnimHierarchy *hierarchy = fn_8044757C(frame);
        if (hierarchy != 0) {
"""
def body(loop, frame="hierarchy->nodeInfo[i].frame", nid="hierarchy->nodeInfo[i].nodeID", pre="", post=""):
    return HEAD + pre + f"""            {loop} {{
                if ({frame} != 0) {{
                    Mtx34f *node = fn_80415E70(updater->m_motion, {nid});
                    if (node != 0) {{
                        if (updater->m_head == 0) {{
                            updater->m_head = updater->m_cursor = new UnkClumpUpdaterNode_80015A7C;
                        }} else {{
                            updater->m_cursor->m_next = new UnkClumpUpdaterNode_80015A7C;
                            updater->m_cursor = updater->m_cursor->m_next;
                        }}
                        if (updater->m_cursor != 0) {{
                            updater->m_cursor->m_frame = {frame};
                            updater->m_cursor->m_id = {nid};
                            updater->m_cursor->m_node = node;
                            updater->m_cursor->m_parent = 0;
                            updater->m_cursor->m_next = 0;
                        }}
                    }}
                }}
{post}            }}
        }}
    }}
    fn_8048752C(frame, fn_80015570, data);
    return frame;
"""
F = "for (int i = 0; i < hierarchy->numNodes; i++)"
BODIES = {"fn_80015570": {
    "outer_decl": body("for (i = 0; i < hierarchy->numNodes; i++)", pre="            int i;\n"),
    "long_i": body("for (s32 i = 0; i < hierarchy->numNodes; i++)"),
    "while_loop": body("while (i < hierarchy->numNodes)", pre="            int i = 0;\n", post="                i++;\n"),
    "node_at": body(F, "NodeAt_(hierarchy, i).frame", "NodeAt_(hierarchy, i).nodeID"),
    "node_fns": body(F, "NodeFrame_(hierarchy, i)", "NodeId_(hierarchy, i)"),
    "num_fn": body("for (int i = 0; i < NumNodes_(hierarchy); i++)"),
    "num_fn_at": body("for (int i = 0; i < NumNodes_(hierarchy); i++)", "NodeAt_(hierarchy, i).frame", "NodeAt_(hierarchy, i).nodeID"),
    "preinc": body("for (int i = 0; i < hierarchy->numNodes; ++i)"),
    "ptr_plus": body(F, "(hierarchy->nodeInfo + i)->frame", "(hierarchy->nodeInfo + i)->nodeID"),
    "do_while": body("if (0 < hierarchy->numNodes) do", pre="            int i = 0;\n", post="                i++;\n").replace("            }\n        }\n    }\n    fn_8048752C", "            } while (i < hierarchy->numNodes);\n        }\n    }\n    fn_8048752C"),
}}
