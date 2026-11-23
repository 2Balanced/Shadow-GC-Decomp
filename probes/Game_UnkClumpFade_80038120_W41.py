"""Wave 41: FindNodeFrame's loop registers (retail: offset r4, id r5) through inline getters and with
copy propagation off."""
SCORE = ["fn_80038DD8", "fn_80038D24"]
F = "inline RwFrame *FindNodeFrame(RwFrame *frame, UnkNodeSearch_80038D24 *search) {\n"
N = "extern \"C\" RwFrame *fn_80038CC8(RwFrame *frame, void *data) {\n"
PRAG = [(F, "#pragma opt_propagation off\n" + F), (N, "#pragma opt_propagation reset\n" + N)]
CMP = "            if (nodeFrame != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID) {\n"
GID = "inline s32 SearchId_(UnkNodeSearch_80038D24 *s) {\n    return s->m_id;\n}\n\n"
NID = "inline s32 NodeId_(RpHAnimHierarchy *h, int i) {\n    return h->pNodeInfo[i].nodeID;\n}\n\n"


def v(*extra, prag=False):
    return list(extra) + (PRAG if prag else [])


REPL = {
    "prag": v(prag=True),
    "gid": v((F, GID + F), (CMP, "            if (nodeFrame != 0 && SearchId_(search) == hierarchy->pNodeInfo[i].nodeID) {\n")),
    "gid_prag": v((F, GID + F), (CMP, "            if (nodeFrame != 0 && SearchId_(search) == hierarchy->pNodeInfo[i].nodeID) {\n"), prag=True),
    "nid": v((F, NID + F), (CMP, "            if (nodeFrame != 0 && search->m_id == NodeId_(hierarchy, i)) {\n")),
    "nid_prag": v((F, NID + F), (CMP, "            if (nodeFrame != 0 && search->m_id == NodeId_(hierarchy, i)) {\n"), prag=True),
    "both": v((F, GID + NID + F), (CMP, "            if (nodeFrame != 0 && SearchId_(search) == NodeId_(hierarchy, i)) {\n")),
    "both_rev": v((F, GID + NID + F), (CMP, "            if (nodeFrame != 0 && NodeId_(hierarchy, i) == SearchId_(search)) {\n")),
    "both_prag": v((F, GID + NID + F), (CMP, "            if (nodeFrame != 0 && SearchId_(search) == NodeId_(hierarchy, i)) {\n"), prag=True),
}
