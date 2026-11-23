"""Wave 41: PlayerShadowMotionHand. Attach with the clump put in its world by an inline helper
(as SetClumpWorld in Game/BossCtrlUnit.cpp), so that the helper's parameters are the first
coloured locals; copy propagation off for Update."""
SRC = open("src/Game/PlayerShadowMotionHand.cpp", encoding="utf-8").read()
SCORE = ["Attach__Q46Player6Shadow14MotionHandImpl4HandFv"]
OLD = SRC[SRC.index("void MotionHandImpl::Hand::Attach() {"):SRC.index("MotionHandImpl::MotionHandImpl(const shared_ptr")]
HELPER = """/* Puts clump in world (name inferred; as in Game/BossCtrlUnit.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

"""
NEW1 = HELPER + """void MotionHandImpl::Hand::Attach() {
    if (m_clump != 0) {
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        void *world = lbl_8051D208[area].m_world;
        if (world != 0) {
            SetClumpWorld(m_clump, world);
        }
    }
}

"""
REPL = {"helper": [(OLD, NEW1)],
        "helper_prag": [(OLD, "#pragma opt_propagation off\n" + NEW1.replace("\n\n\nvoid", "\n\nvoid").rstrip("\n") + "\n#pragma opt_propagation reset\n\n")]}
