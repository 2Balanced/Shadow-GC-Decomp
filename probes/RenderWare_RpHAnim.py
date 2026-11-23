"""Wave 41, RenderWare/RpHAnim.c: fn_804474D8 (retail copies the zeroed counter into the
strength-reduced offset: the loop in an inline function), fn_804472AC, fn_80446780."""
SCORE = ["fn_804474D8", "fn_804472AC", "fn_80446780"]
NL = chr(10)
FHEAD = "/* Attaches the hierarchy to its parent frame's tree: points every node at the frame with its ID. */" + NL
BODY = """    void *frame;
    int i;
    frame = hierarchy->m_parentFrame;
    for (i = 0; i < hierarchy->m_nodeCount; i++) {
        if (hierarchy->m_nodes[i].m_id == fn_8044758C(frame)) {
            hierarchy->m_nodes[i].m_frame = frame;
        }
    }
    fn_8048722C(frame);
"""
INL = """static inline void SetNodeFrames(R1Hierarchy *hierarchy, void *frame) {
    int i;
    for (i = 0; i < hierarchy->m_nodeCount; i++) {
        if (hierarchy->m_nodes[i].m_id == fn_8044758C(frame)) {
            hierarchy->m_nodes[i].m_frame = frame;
        }
    }
}

"""
REPL = {
    "base": [],
    "inl": [(BODY, "    void *frame;" + NL + "    frame = hierarchy->m_parentFrame;" + NL + "    SetNodeFrames(hierarchy, frame);" + NL + "    fn_8048722C(frame);" + NL),
            (FHEAD, INL + FHEAD)],
    "inl_direct": [(BODY, "    void *frame = hierarchy->m_parentFrame;" + NL + "    SetNodeFrames(hierarchy, frame);" + NL + "    fn_8048722C(frame);" + NL),
            (FHEAD, INL + FHEAD)],
}

INL_ALL = """static inline void AttachHierarchy(R1Hierarchy *hierarchy) {
    void *frame;
    int i;
    frame = hierarchy->m_parentFrame;
    for (i = 0; i < hierarchy->m_nodeCount; i++) {
        if (hierarchy->m_nodes[i].m_id == fn_8044758C(frame)) {
            hierarchy->m_nodes[i].m_frame = frame;
        }
    }
    fn_8048722C(frame);
    fn_8048752C(frame, (void *)fn_80446A04, hierarchy);
}

"""
INL_RET = """static inline void *SetNodeFrames(R1Hierarchy *hierarchy) {
    void *frame;
    int i;
    frame = hierarchy->m_parentFrame;
    for (i = 0; i < hierarchy->m_nodeCount; i++) {
        if (hierarchy->m_nodes[i].m_id == fn_8044758C(frame)) {
            hierarchy->m_nodes[i].m_frame = frame;
        }
    }
    return frame;
}

"""
CALLTAIL = "    fn_8048752C(frame, (void *)fn_80446A04, hierarchy);" + NL
REPL.update({
    "inl_all": [(BODY + CALLTAIL, "    AttachHierarchy(hierarchy);" + NL), (FHEAD, INL_ALL + FHEAD)],
    "inl_ret": [(BODY, "    void *frame;" + NL + "    frame = SetNodeFrames(hierarchy);" + NL + "    fn_8048722C(frame);" + NL), (FHEAD, INL_RET + FHEAD)],
})
