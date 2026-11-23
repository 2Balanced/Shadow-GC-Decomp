"""Wave 40: FindNodeFrame's loop (retail: offset r4, the search id r5, the node's frame r6)."""
SCORE = ["fn_80038DD8", "fn_80038D24"]
L = """        for (int i = 0; i < hierarchy->numNodes; i++) {
            if (hierarchy->pNodeInfo[i].pFrame != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID) {
                search->m_frame = hierarchy->pNodeInfo[i].pFrame;
                return frame;
            }
        }
"""
def v(s): return [(L, s)]
REPL = {
  "nf": v("""        for (int i = 0; i < hierarchy->numNodes; i++) {
            RwFrame *nodeFrame = hierarchy->pNodeInfo[i].pFrame;
            if (nodeFrame != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID) {
                search->m_frame = nodeFrame;
                return frame;
            }
        }
"""),
  "info": v("""        for (int i = 0; i < hierarchy->numNodes; i++) {
            RpHAnimNodeInfo *info = &hierarchy->pNodeInfo[i];
            if (info->pFrame != 0 && search->m_id == info->nodeID) {
                search->m_frame = info->pFrame;
                return frame;
            }
        }
"""),
  "info_nf": v("""        for (int i = 0; i < hierarchy->numNodes; i++) {
            RpHAnimNodeInfo *info = &hierarchy->pNodeInfo[i];
            RwFrame *nodeFrame = info->pFrame;
            if (nodeFrame != 0 && search->m_id == info->nodeID) {
                search->m_frame = nodeFrame;
                return frame;
            }
        }
"""),
  "i_out": v("""        int i;
        for (i = 0; i < hierarchy->numNodes; i++) {
            if (hierarchy->pNodeInfo[i].pFrame != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID) {
                search->m_frame = hierarchy->pNodeInfo[i].pFrame;
                return frame;
            }
        }
"""),
  "nf_out": v("""        RwFrame *nodeFrame;
        for (int i = 0; i < hierarchy->numNodes; i++) {
            nodeFrame = hierarchy->pNodeInfo[i].pFrame;
            if (nodeFrame != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID) {
                search->m_frame = nodeFrame;
                return frame;
            }
        }
"""),
  "i_nf_out": v("""        int i;
        RwFrame *nodeFrame;
        for (i = 0; i < hierarchy->numNodes; i++) {
            nodeFrame = hierarchy->pNodeInfo[i].pFrame;
            if (nodeFrame != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID) {
                search->m_frame = nodeFrame;
                return frame;
            }
        }
"""),
  "nested_if": v("""        for (int i = 0; i < hierarchy->numNodes; i++) {
            if (hierarchy->pNodeInfo[i].pFrame != 0) {
                if (search->m_id == hierarchy->pNodeInfo[i].nodeID) {
                    search->m_frame = hierarchy->pNodeInfo[i].pFrame;
                    return frame;
                }
            }
        }
"""),
  "id_rev": v("""        for (int i = 0; i < hierarchy->numNodes; i++) {
            if (hierarchy->pNodeInfo[i].pFrame != 0 && hierarchy->pNodeInfo[i].nodeID == search->m_id) {
                search->m_frame = hierarchy->pNodeInfo[i].pFrame;
                return frame;
            }
        }
"""),
}

def loop(decl, idx="int i = 0", idl=False, pre=""):
    body = """            nodeFrame = hierarchy->pNodeInfo[i].pFrame;
            if (nodeFrame != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID) {
                search->m_frame = nodeFrame;
                return frame;
            }
"""
    if idl:
        body = """            nodeFrame = hierarchy->pNodeInfo[i].pFrame;
            if (nodeFrame != 0) {
                id = search->m_id;
                if (id == hierarchy->pNodeInfo[i].nodeID) {
                    search->m_frame = nodeFrame;
                    return frame;
                }
            }
"""
    return v(decl + "        for (" + idx + "; i < hierarchy->numNodes; i++) {\n" + body + "        }\n")
T = "        "
REPL = {
  "A_nf_i": loop(T + "RwFrame *nodeFrame;\n" + T + "int i;\n", "i = 0"),
  "E_nf_id": loop(T + "RwFrame *nodeFrame;\n" + T + "s32 id;\n", idl=True),
  "F_id_nf": loop(T + "s32 id;\n" + T + "RwFrame *nodeFrame;\n", idl=True),
  "G_id_nf_i": loop(T + "s32 id;\n" + T + "RwFrame *nodeFrame;\n" + T + "int i;\n", "i = 0", idl=True),
  "H_i_id_nf": loop(T + "int i;\n" + T + "s32 id;\n" + T + "RwFrame *nodeFrame;\n", "i = 0", idl=True),
  "I_nf_id_i": loop(T + "RwFrame *nodeFrame;\n" + T + "s32 id;\n" + T + "int i;\n", "i = 0", idl=True),
  "J_i_nf_id": loop(T + "int i;\n" + T + "RwFrame *nodeFrame;\n" + T + "s32 id;\n", "i = 0", idl=True),
  "K_nf_i_id": loop(T + "RwFrame *nodeFrame;\n" + T + "int i;\n" + T + "s32 id;\n", "i = 0", idl=True),
  "L_id_i_nf": loop(T + "s32 id;\n" + T + "int i;\n" + T + "RwFrame *nodeFrame;\n", "i = 0", idl=True),
}

HEAD = """    if (search->m_frame != 0) {
        return frame;
    }
    RpHAnimHierarchy *hierarchy = fn_8044757C(frame);
"""
def top(decl_before, decl_after, idx):
    return [(HEAD, decl_before + HEAD + decl_after)] + loop("", idx)
REPL = {
  "T_nf": top("    RwFrame *nodeFrame;\n", "", "int i = 0"),
  "T_nf_i": top("    RwFrame *nodeFrame;\n    int i;\n", "", "i = 0"),
  "T_i_nf": top("    int i;\n    RwFrame *nodeFrame;\n", "", "i = 0"),
  "T_i__nf": top("    int i;\n", "    RwFrame *nodeFrame;\n", "i = 0"),
  "T_nf__i": top("    RwFrame *nodeFrame;\n", "    int i;\n", "i = 0"),
  "T__nf_i": top("", "    RwFrame *nodeFrame;\n    int i;\n", "i = 0"),
  "T__i_nf": top("", "    int i;\n    RwFrame *nodeFrame;\n", "i = 0"),
  "T__nf": top("", "    RwFrame *nodeFrame;\n", "int i = 0"),
}

FN = "inline RwFrame *FindNodeFrame(RwFrame *frame, UnkNodeSearch_80038D24 *search) {"
HELP = """inline s32 SearchId_(UnkNodeSearch_80038D24 *s) { return s->m_id; }
inline s32 SearchIdC_(const UnkNodeSearch_80038D24 *s) { return s->m_id; }
inline RwFrame *NodeFrame_(RpHAnimHierarchy *h, int i) { return h->pNodeInfo[i].pFrame; }
inline s32 NodeId_(RpHAnimHierarchy *h, int i) { return h->pNodeInfo[i].nodeID; }
inline bool IsNode_(UnkNodeSearch_80038D24 *s, RpHAnimHierarchy *h, int i) { return s->m_id == h->pNodeInfo[i].nodeID; }
"""
H = (FN, HELP + FN)
def lp(decl, cond, st="nodeFrame = hierarchy->pNodeInfo[i].pFrame;", val="nodeFrame"):
    return [H] + v(decl + "        for (int i = 0; i < hierarchy->numNodes; i++) {\n            " + st + "\n            if (" + cond + ") {\n                search->m_frame = " + val + ";\n                return frame;\n            }\n        }\n")
NF = "        RwFrame *nodeFrame;\n"
REPL = {
  "M1": lp(NF, "nodeFrame != 0 && SearchId_(search) == hierarchy->pNodeInfo[i].nodeID"),
  "M1c": lp(NF, "nodeFrame != 0 && SearchIdC_(search) == hierarchy->pNodeInfo[i].nodeID"),
  "M2": lp("", "NodeFrame_(hierarchy, i) != 0 && SearchId_(search) == NodeId_(hierarchy, i)", "", "NodeFrame_(hierarchy, i)"),
  "M3": lp(NF, "nodeFrame != 0 && IsNode_(search, hierarchy, i)"),
  "M4": lp(NF, "nodeFrame != 0 && search->m_id == NodeId_(hierarchy, i)"),
  "M5": lp(NF, "nodeFrame != 0 && SearchId_(search) == NodeId_(hierarchy, i)"),
  "M6": lp(NF, "nodeFrame != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID", "nodeFrame = NodeFrame_(hierarchy, i);"),
  "M7": lp("", "NodeFrame_(hierarchy, i) != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID", "", "NodeFrame_(hierarchy, i)"),
  "M8": lp("", "hierarchy->pNodeInfo[i].pFrame != 0 && SearchId_(search) == hierarchy->pNodeInfo[i].nodeID", "", "hierarchy->pNodeInfo[i].pFrame"),
}

REPL = {"nf_out": v("""        RwFrame *nodeFrame;
        for (int i = 0; i < hierarchy->numNodes; i++) {
            nodeFrame = hierarchy->pNodeInfo[i].pFrame;
            if (nodeFrame != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID) {
                search->m_frame = nodeFrame;
                return frame;
            }
        }
""")}
