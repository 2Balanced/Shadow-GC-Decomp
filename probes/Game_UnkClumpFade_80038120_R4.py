"""Wave 40: FindNodeFrame's loop (fn_80038DD8 and fn_80038D24): retail numbers the offset r4 and
the search id r5, GC/2.7 the other way round."""
SCORE = ["fn_80038DD8", "fn_80038D24"]
L = """        RwFrame *nodeFrame;
        for (int i = 0; i < hierarchy->numNodes; i++) {
            nodeFrame = hierarchy->pNodeInfo[i].pFrame;
            if (nodeFrame != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID) {
                search->m_frame = nodeFrame;
                return frame;
            }
        }
"""
REPL = {
  "i_first": [(L, L.replace("        RwFrame *nodeFrame;\n        for (int i = 0;", "        int i;\n        RwFrame *nodeFrame;\n        for (i = 0;"))],
  "nf_in_loop": [(L, L.replace("        RwFrame *nodeFrame;\n", "").replace("            nodeFrame =", "            RwFrame *nodeFrame ="))],
  "swap_cmp": [(L, L.replace("search->m_id == hierarchy->pNodeInfo[i].nodeID", "hierarchy->pNodeInfo[i].nodeID == search->m_id"))],
  "node_ptr": [(L, """        RwFrame *nodeFrame;
        for (int i = 0; i < hierarchy->numNodes; i++) {
            RpHAnimNodeInfo *info = &hierarchy->pNodeInfo[i];
            nodeFrame = info->pFrame;
            if (nodeFrame != 0 && search->m_id == info->nodeID) {
                search->m_frame = nodeFrame;
                return frame;
            }
        }
""")],
  "id_local_in": [(L, """        RwFrame *nodeFrame;
        for (int i = 0; i < hierarchy->numNodes; i++) {
            nodeFrame = hierarchy->pNodeInfo[i].pFrame;
            if (nodeFrame != 0) {
                s32 id = search->m_id;
                if (id == hierarchy->pNodeInfo[i].nodeID) {
                    search->m_frame = nodeFrame;
                    return frame;
                }
            }
        }
""")],
  "nf_after_i": [(L, L.replace("        RwFrame *nodeFrame;\n        for (int i = 0;", "        int i;\n        for (i = 0;").replace("            nodeFrame =", "            RwFrame *nodeFrame;\n            nodeFrame ="))],
}
