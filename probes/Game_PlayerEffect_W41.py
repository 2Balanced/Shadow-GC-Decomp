"""Wave 41: Jet::UpdateVisibility. Retail starts the hiding loop's offset with mr r31,r30 (a loop
inlined from a function) and the showing loop with two li, storing false from the counter's zero."""
U = "UpdateVisibility__Q226@unnamed@PlayerEffect_cpp@3JetFv"
SCORE = [U]
OLD = """void Jet::UpdateVisibility() {
    int i;
    if (m_player->IsWeaponHidden()) {
        if (!m_hidden) {
            m_hidden = true;
            for (i = 0; i < 4; i++) {
                if (m_clumps[i] != 0) {
                    fn_80038FC0(m_clumps[i]);
                }
            }
        }
    } else if (m_hidden) {
        m_hidden = false;
        for (i = 0; i < 4; i++) {
            if (m_clumps[i] != 0) {
                fn_80038F94(m_clumps[i]);
            }
        }
    }
}
"""
HIDE = """inline void HideJetClumps(Jet *jet) {
    for (int i = 0; i < 4; i++) {
        if (jet->m_clumps[i] != 0) {
            fn_80038FC0(jet->m_clumps[i]);
        }
    }
}

"""
SHOW = """inline void ShowJetClumps(Jet *jet) {
    for (int i = 0; i < 4; i++) {
        if (jet->m_clumps[i] != 0) {
            fn_80038F94(jet->m_clumps[i]);
        }
    }
}

"""
SHOW_LOOP = """        for (i = 0; i < 4; i++) {
            if (m_clumps[i] != 0) {
                fn_80038F94(m_clumps[i]);
            }
        }
"""
HIDE_LOOP = """            for (i = 0; i < 4; i++) {
                if (m_clumps[i] != 0) {
                    fn_80038FC0(m_clumps[i]);
                }
            }
"""
REPL = {
    "hide_inl": [(OLD, HIDE + OLD.replace(HIDE_LOOP, "            HideJetClumps(this);\n"))],
    "both_inl": [(OLD, HIDE + SHOW + OLD.replace(HIDE_LOOP, "            HideJetClumps(this);\n").replace(SHOW_LOOP, "        ShowJetClumps(this);\n"))],
    "show_inl": [(OLD, SHOW + OLD.replace(SHOW_LOOP, "        ShowJetClumps(this);\n"))],
    "hide_inl_idecl": [(OLD, HIDE + OLD.replace(HIDE_LOOP, "            HideJetClumps(this);\n").replace("    int i;\n", "").replace("        for (i = 0;", "        for (int i = 0;"))],
}

# Jet::UpdateModels: retail frame r31, the row base (this + 4i) r30, counter r29, this r28.
M = "UpdateModels__Q226@unnamed@PlayerEffect_cpp@3JetFv"
SYMBOLS = {M: "void Jet::UpdateModels() {"}
MB = """    for (i = 0; i < 4; i++) {
        if (m_clumps[i] != 0) {
            float color[4];
            color[3] = m_alpha;
            color[0] = -1.0f;
            color[1] = -1.0f;
            color[2] = -1.0f;
            fn_80039114(m_clumps[i], color);
            m_angles[i] += fn_8040EC80(40.0f * (lbl_805FAD38 / 180.0f),
                                       50.0f * (lbl_805FAD38 / 180.0f));
            UnkMotionNode_800AA760 *node = fn_800AA760(m_player->m_motion, s_jetNodes[i]);
            frame = m_frames[i];
            frame->modelling = fn_800591F0(
                &(const Mtx34f &)fn_8001E044(node->m_matrix, &(const Mtx34f &)fn_8040B138(m_angles[i])));
            fn_8048722C(m_frames[i]);
        }
    }
"""
BODIES = {M: {
    "m_frame_first": "    RwFrame *frame;\n    int i;\n" + MB,
    "m_frame_inner": "    int i;\n" + MB.replace("            frame = m_frames[i];", "            RwFrame *frame = m_frames[i];"),
    "m_frame_inner_top": "    int i;\n" + MB.replace("            frame = m_frames[i];", "            RwFrame *frame = m_frames[i];").replace("            UnkMotionNode_800AA760 *node", "            RwFrame *frame0;\n            UnkMotionNode_800AA760 *node"),
    "m_noframe": "    int i;\n" + MB.replace("            frame = m_frames[i];\n            frame->modelling", "            m_frames[i]->modelling"),
    "m_idecl": "    RwFrame *frame;\n" + MB.replace("    for (i = 0;", "    for (int i = 0;"),
    "m_frame_first_inner_i": "    RwFrame *frame;\n" + MB.replace("    for (i = 0;", "    for (int i = 0;"),
}}
FF = "    RwFrame *frame;\n    int i;\n"
OFFM = [("void Jet::UpdateModels() {", "#pragma opt_propagation off\nvoid Jet::UpdateModels() {"), ("void Jet::UpdateVisibility() {", "#pragma opt_propagation reset\nvoid Jet::UpdateVisibility() {")]
BODIES[M].update({
    "m_ff_self": "    RwFrame *frame;\n    int i;\n    Jet *self = this;\n" + MB.replace("m_", "self->m_"),
    "m_ff_ref": FF + MB.replace("            frame = m_frames[i];\n            frame->modelling = fn_800591F0(", "            frame = m_frames[i];\n            RwMatrix &mod = frame->modelling;\n            mod = fn_800591F0("),
    "m_ff_frames": FF + MB.replace("            frame = m_frames[i];", "            RwFrame **frames = m_frames;\n            frame = frames[i];"),
    "m_ff_later": FF + MB.replace("            UnkMotionNode_800AA760 *node = fn_800AA760(m_player->m_motion, s_jetNodes[i]);\n            frame = m_frames[i];", "            frame = m_frames[i];\n            UnkMotionNode_800AA760 *node = fn_800AA760(m_player->m_motion, s_jetNodes[i]);"),
    "m_ff_slot": FF + MB.replace("        if (m_clumps[i] != 0) {", "        RpClump **slot = &m_clumps[i];\n        if (*slot != 0) {").replace("fn_80039114(m_clumps[i], color)", "fn_80039114(*slot, color)"),
})
XTRA = {M: "inline RwMatrix *JetFrameMatrix_(RwFrame *frame) { return &frame->modelling; }\n\n"}
BODIES[M].update({
    "m_ff_inl": FF + MB.replace("            frame->modelling = fn_800591F0(", "            *JetFrameMatrix_(frame) = fn_800591F0("),
    "m_ff_ptr": FF + MB.replace("            frame = m_frames[i];\n            frame->modelling = fn_800591F0(", "            frame = m_frames[i];\n            RwMatrix *mod = &frame->modelling;\n            *mod = fn_800591F0("),
    "m_ff_addr": FF + MB.replace("            frame->modelling = fn_800591F0(", "            *(&frame->modelling) = fn_800591F0("),
})
