"""Wave 41: SetMotion: retail reads the right motion first (an inline function's arguments, right to
left) and its loop copies the zeroed counter (the loop in an inline function)."""
SRC = open("src/Game/PlayerShadowMotionHand.cpp", encoding="utf-8").read()
SCORE = ["SetMotion__Q36Player6Shadow14MotionHandImplFPC22UnkHandMotion_800B07FC"]
s = SRC.index("void MotionHandImpl::SetMotion(const UnkHandMotion_800B07FC *motion) {")
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
LOOPF = """inline void RestartHands_(MotionHandImpl *impl) {
    for (int i = 0; i < 2; i++) {
        MotionHandImpl::Hand *hand = &impl->m_hands[0][i];
        if (hand->m_anim != 0) {
            fn_80416F28(hand->m_anim, 0, 0, 0);
            fn_80416F28(hand->m_anim, 0, 1, 0);
        }
    }
}

inline void SetHandMotions_(MotionHandImpl *impl, u16 left, u16 right) {
    fn_80417568(impl->m_hands[0][0].m_anim, left);
    fn_80417568(impl->m_hands[0][1].m_anim, right);
}

"""
BODY_A = """void MotionHandImpl::SetMotion(const UnkHandMotion_800B07FC *motion) {
    m_pose = 0xE;
    m_request = 0xE;
    m_shownPose = m_pose;
    SetHandMotions_(this, motion->m_left, motion->m_right);
    fn_80416F28(m_hands[0][0].m_anim, 0, 0xB, 0);
    fn_80416F28(m_hands[0][1].m_anim, 0, 0xB, 0);
    RestartHands_(this);
}
"""
BODY_B = """void MotionHandImpl::SetMotion(const UnkHandMotion_800B07FC *motion) {
    m_pose = 0xE;
    m_request = 0xE;
    m_shownPose = m_pose;
    fn_80417568(m_hands[0][0].m_anim, motion->m_left);
    fn_80417568(m_hands[0][1].m_anim, motion->m_right);
    fn_80416F28(m_hands[0][0].m_anim, 0, 0xB, 0);
    fn_80416F28(m_hands[0][1].m_anim, 0, 0xB, 0);
    RestartHands_(this);
}
"""
REPL = {
  "args_loop": [(FN, LOOPF + BODY_A)],
  "loop_only": [(FN, LOOPF + BODY_B)],
  "prag_args_loop": [(FN, LOOPF + "#pragma opt_propagation off\n" + BODY_A + "#pragma opt_propagation reset\n")],
}
ALL = """inline void SetMotions_(MotionHandImpl *impl, u16 left, u16 right) {
    impl->m_pose = 0xE;
    impl->m_request = 0xE;
    impl->m_shownPose = impl->m_pose;
    fn_80417568(impl->m_hands[0][0].m_anim, left);
    fn_80417568(impl->m_hands[0][1].m_anim, right);
    fn_80416F28(impl->m_hands[0][0].m_anim, 0, 0xB, 0);
    fn_80416F28(impl->m_hands[0][1].m_anim, 0, 0xB, 0);
    RestartHands_(impl);
}

void MotionHandImpl::SetMotion(const UnkHandMotion_800B07FC *motion) {
    SetMotions_(this, motion->m_left, motion->m_right);
}
"""
REPL["all_inline"] = [(FN, LOOPF + ALL)]
ALL2 = ALL.replace("inline void SetMotions_(MotionHandImpl *impl, u16 left, u16 right) {", "inline void SetMotions_(MotionHandImpl *impl, s32 left, s32 right) {")
REPL["all_inline_s32"] = [(FN, LOOPF + ALL2)]
B3 = """void MotionHandImpl::SetMotion(const UnkHandMotion_800B07FC *motion) {
    u16 right = motion->m_right;
    u16 left = motion->m_left;
    m_pose = 0xE;
    m_request = 0xE;
    m_shownPose = m_pose;
    fn_80417568(m_hands[0][0].m_anim, left);
    fn_80417568(m_hands[0][1].m_anim, right);
    fn_80416F28(m_hands[0][0].m_anim, 0, 0xB, 0);
    fn_80416F28(m_hands[0][1].m_anim, 0, 0xB, 0);
    RestartHands_(this);
}
"""
REPL["locals_rl"] = [(FN, LOOPF + B3)]
REPL["prag_locals_rl"] = [(FN, LOOPF + "#pragma opt_propagation off\n" + B3 + "#pragma opt_propagation reset\n")]
