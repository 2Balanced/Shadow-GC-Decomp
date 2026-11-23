"""MotionHand::GetNodeMatrix (wave 40): the loop inlined from a function (retail `li r29,0` /
`mr r31,r29`: a loop inlined from a function copies the zeroed counter into its offset)."""
SCORE = ["GetNodeMatrix__Q36Player6Shadow10MotionHandFi", "UpdatePose__Q36Player6Shadow14MotionHandImplFv", "Update__Q36Player6Shadow14MotionHandImplFv"]
O = """Mtx34f *MotionHand::GetNodeMatrix(int node) {
    MotionHandImpl *impl = m_impl.get();
    for (u32 i = 0; i < 5; i++) {
        if (impl->m_anims[i] != 0) {
            Mtx34f *matrix = fn_80415E70(impl->m_anims[i], node);
            if (matrix != 0) {
                return matrix;
            }
        }
    }
    return 0;
}
"""
H = """/* The first hand animation that has node (name inferred). */
inline Mtx34f *FindNodeMatrix(MotionHandImpl *impl, int node) {
    for (u32 i = 0; i < 5; i++) {
        if (impl->m_anims[i] != 0) {
            Mtx34f *matrix = fn_80415E70(impl->m_anims[i], node);
            if (matrix != 0) {
                return matrix;
            }
        }
    }
    return 0;
}

"""
REPL = {
  "inl_get": [(O, H + "Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    return FindNodeMatrix(m_impl.get(), node);\n}\n")],
  "inl_local": [(O, H + "Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    MotionHandImpl *impl = m_impl.get();\n    return FindNodeMatrix(impl, node);\n}\n")],
  "inl_px": [(O, H + "Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    return FindNodeMatrix(m_impl.px, node);\n}\n")],
}
H2 = H.replace("inline Mtx34f *FindNodeMatrix(MotionHandImpl *impl, int node) {", "inline Mtx34f *FindNodeMatrix(int node, MotionHandImpl *impl) {")
REPL["inl_rev"] = [(O, H2 + "Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    return FindNodeMatrix(node, m_impl.get());\n}\n")]
MEMDECL = "    /* 0x110 */ _stMtnPlayWork *m_anims[5];\n"
MEMBODY = """    /* The first hand animation that has node (name inferred). */
    Mtx34f *FindNodeMatrix(int node) {
        for (u32 i = 0; i < 5; i++) {
            if (m_anims[i] != 0) {
                Mtx34f *matrix = fn_80415E70(m_anims[i], node);
                if (matrix != 0) {
                    return matrix;
                }
            }
        }
        return 0;
    }

"""
REPL["inl_member"] = [(MEMDECL, MEMBODY + MEMDECL), (O, "Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    return m_impl.get()->FindNodeMatrix(node);\n}\n")]
REPL["inl_member_arrow"] = [(MEMDECL, MEMBODY + MEMDECL), (O, "Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    return m_impl->FindNodeMatrix(node);\n}\n")]
H3 = """/* The first hand animation that has node (name inferred). */
inline Mtx34f *FindNodeMatrix(_stMtnPlayWork *const *anims, int node) {
    for (u32 i = 0; i < 5; i++) {
        if (anims[i] != 0) {
            Mtx34f *matrix = fn_80415E70(anims[i], node);
            if (matrix != 0) {
                return matrix;
            }
        }
    }
    return 0;
}

"""
REPL["inl_anims"] = [(O, H3 + "Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    return FindNodeMatrix(m_impl.get()->m_anims, node);\n}\n")]
H4 = H.replace("    for (u32 i = 0; i < 5; i++) {\n", "    u32 i;\n    for (i = 0; i < 5; i++) {\n")
REPL["inl_i_top"] = [(O, H4 + "Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    return FindNodeMatrix(m_impl.get(), node);\n}\n")]
H5 = H.replace("inline Mtx34f *FindNodeMatrix(MotionHandImpl *impl, int node) {\n", "inline Mtx34f *FindNodeMatrix(MotionHandImpl *impl, int node) {\n    int key = node;\n").replace("fn_80415E70(impl->m_anims[i], node)", "fn_80415E70(impl->m_anims[i], key)")
REPL["inl_key"] = [(O, H5 + "Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    return FindNodeMatrix(m_impl.get(), node);\n}\n")]
# UpdatePose: retail adds the motion base first (`add r4,r0,r4`), subtracts 1 into r0, narrows.
UP = "                fn_80417568(hand->m_anim, motion < 0 ? 0xFFFF : motion - 1 + m_motionBase);\n"
SCORE2 = ["UpdatePose__Q36Player6Shadow14MotionHandImplFv"]
REPL["up_base_first"] = [(UP, UP.replace("motion - 1 + m_motionBase", "m_motionBase + motion - 1"))]
REPL["up_base_paren"] = [(UP, UP.replace("motion - 1 + m_motionBase", "m_motionBase + (motion - 1)"))]
REPL["up_sum_paren"] = [(UP, UP.replace("motion - 1 + m_motionBase", "(motion + m_motionBase) - 1"))]
REPL["up_minus_first"] = [(UP, UP.replace("motion - 1 + m_motionBase", "-1 + motion + m_motionBase"))]
REPL["up_u16_local"] = [(UP, "                u16 id = motion < 0 ? 0xFFFF : motion - 1 + m_motionBase;\n                fn_80417568(hand->m_anim, id);\n")]
REPL["up_int_local"] = [(UP, "                int id = motion < 0 ? 0xFFFF : motion - 1 + m_motionBase;\n                fn_80417568(hand->m_anim, id);\n")]
REPL["up_ifelse"] = [(UP, "                u16 id;\n                if (motion < 0) {\n                    id = 0xFFFF;\n                } else {\n                    id = motion - 1 + m_motionBase;\n                }\n                fn_80417568(hand->m_anim, id);\n")]
REPL["up_cast"] = [(UP, "                fn_80417568(hand->m_anim, motion < 0 ? (u16)0xFFFF : (u16)(motion - 1 + m_motionBase));\n")]
REPL["up_cast_base"] = [(UP, "                fn_80417568(hand->m_anim, motion < 0 ? (u16)0xFFFF : (u16)(m_motionBase + motion - 1));\n")]
REPL["up_cast_base2"] = [(UP, "                fn_80417568(hand->m_anim, motion < 0 ? (u16)0xFFFF : (u16)(m_motionBase + (motion - 1)));\n")]
REPL["up_ifelse_base"] = [(UP, "                u16 id;\n                if (motion < 0) {\n                    id = 0xFFFF;\n                } else {\n                    id = m_motionBase + motion - 1;\n                }\n                fn_80417568(hand->m_anim, id);\n")]
REPL["up_cast_only_else"] = [(UP, "                fn_80417568(hand->m_anim, motion < 0 ? 0xFFFF : (u16)(m_motionBase + motion - 1));\n")]
for nm, ex in (("c1", "(m_motionBase + motion) - 1"), ("c2", "-1 + (m_motionBase + motion)"), ("c3", "m_motionBase - 1 + motion"), ("c4", "motion + (m_motionBase - 1)"), ("c5", "(motion - 1) + m_motionBase"), ("c6", "motion + m_motionBase - 1")):
    REPL["up_" + nm] = [(UP, "                fn_80417568(hand->m_anim, motion < 0 ? (u16)0xFFFF : (u16)(%s));\n" % ex)]
UPK = REPL["up_c3"]
U0 = """    if (m_current != 0 && m_player->m_motion != 0) {
        PlayerMotion *motion = m_player->m_motion;
        bool done = false;
        const UnkMotionNode_800AA760 *nodes[2];
        nodes[0] = fn_800AA760(motion, 5);
        nodes[1] = fn_800AA760(motion, 3);
"""
REPL["upd_direct_first"] = UPK + [(U0, """    if (m_current != 0 && m_player->m_motion != 0) {
        PlayerMotion *motion = m_player->m_motion;
        bool done = false;
        const UnkMotionNode_800AA760 *nodes[2];
        nodes[0] = fn_800AA760(m_player->m_motion, 5);
        nodes[1] = fn_800AA760(motion, 3);
""")]
REPL["upd_done_first"] = UPK + [(U0, """    if (m_current != 0 && m_player->m_motion != 0) {
        bool done = false;
        PlayerMotion *motion = m_player->m_motion;
        const UnkMotionNode_800AA760 *nodes[2];
        nodes[0] = fn_800AA760(motion, 5);
        nodes[1] = fn_800AA760(motion, 3);
""")]
REPL["upd_nodes_first"] = UPK + [(U0, """    if (m_current != 0 && m_player->m_motion != 0) {
        const UnkMotionNode_800AA760 *nodes[2];
        bool done = false;
        PlayerMotion *motion = m_player->m_motion;
        nodes[0] = fn_800AA760(motion, 5);
        nodes[1] = fn_800AA760(motion, 3);
""")]
REPL["upd_motion_outer"] = UPK + [(U0, """    PlayerMotion *motion;
    if (m_current != 0 && m_player->m_motion != 0) {
        motion = m_player->m_motion;
        bool done = false;
        const UnkMotionNode_800AA760 *nodes[2];
        nodes[0] = fn_800AA760(motion, 5);
        nodes[1] = fn_800AA760(motion, 3);
""")]
GETM = "inline PlayerMotion *MotionOf_(const shared_ptr<PlayerBase> &player) {\n    return player->m_motion;\n}\n\nvoid MotionHandImpl::Update() {\n"
UA = "void MotionHandImpl::Update() {\n"
def upd(cond, decl, a0, a1):
    return UPK + [(UA, GETM), (U0, """    if (m_current != 0 && %s != 0) {
        PlayerMotion *motion = %s;
        bool done = false;
        const UnkMotionNode_800AA760 *nodes[2];
        nodes[0] = fn_800AA760(%s, 5);
        nodes[1] = fn_800AA760(%s, 3);
""" % (cond, decl, a0, a1))]
M = "m_player->m_motion"; G = "MotionOf_(m_player)"
REPL["upd_g1"] = upd(G, M, "motion", "motion")
REPL["upd_g2"] = upd(M, G, "motion", "motion")
REPL["upd_g3"] = upd(G, G, "motion", "motion")
REPL["upd_g4"] = upd(G, M, G, "motion")
REPL["upd_g5"] = upd(M, M, G, "motion")
