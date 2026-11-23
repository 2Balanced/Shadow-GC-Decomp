"""Wave 40: MotionEyeBlink's constructor, second loop (retail: the clump's offset r26, the world
r28). The loop in an inline helper, the world through an inline lookup."""
SCORE = ["__ct__Q36Player6Shadow14MotionEyeBlinkFRCQ25boost33shared_ptr<Q26Player10PlayerBase>PCQ26Player22UnkMotionNode_800AA760P21UnkClumpFade_80027D5CRC23UnkMotionFiles_800ADE78"]
LOOP2 = """    UnkStage_8003A840 *stage = fn_8003A840();
    for (int i = 0; i < 5; i++) {
        if (m_clumps[i] != 0) {
            m_clumps[i] = fn_80457090(m_clumps[i]);
            void *world = stage->GetAreas()[fn_80169350(fn_80169364(m_clumps[i]))].m_world;
            if (world != 0) {
                SetClumpWorld(m_clumps[i], world);
            }
            fn_80038FC0(m_clumps[i]);
            fn_80038198(m_clumps[i], fade);
        }
    }
"""
CT = "MotionEyeBlink::MotionEyeBlink(const shared_ptr<PlayerBase> &player, const UnkMotionNode_800AA760 *node,"
H1 = """inline void PlaceClumps_(MotionEyeBlink *self, UnkClumpFade_80027D5C *fade) {
    for (int i = 0; i < 5; i++) {
        if (self->m_clumps[i] != 0) {
            self->m_clumps[i] = fn_80457090(self->m_clumps[i]);
            void *world = lbl_8051D208[fn_80169350(fn_80169364(self->m_clumps[i]))].m_world;
            if (world != 0) {
                RpClump *clump = self->m_clumps[i];
                void *current = fn_8046256C(clump);
                if (current != world) {
                    if (current != 0) {
                        fn_804623EC(current, clump);
                    }
                    fn_804622FC(world, clump);
                }
            }
            fn_80038FC0(self->m_clumps[i]);
            fn_80038198(self->m_clumps[i], fade);
        }
    }
}

"""
H2 = """inline void PlaceClumps2_(MotionEyeBlink *self, UnkStage_8003A840 *stage, UnkClumpFade_80027D5C *fade) {
    for (int i = 0; i < 5; i++) {
        if (self->m_clumps[i] != 0) {
            self->m_clumps[i] = fn_80457090(self->m_clumps[i]);
            void *world = stage->GetAreas()[fn_80169350(fn_80169364(self->m_clumps[i]))].m_world;
            if (world != 0) {
                SetClumpWorld(self->m_clumps[i], world);
            }
            fn_80038FC0(self->m_clumps[i]);
            fn_80038198(self->m_clumps[i], fade);
        }
    }
}

"""
H4 = """inline void *WorldOf_(UnkStage_8003A840 *stage, RpClump *clump) {
    return stage->GetAreas()[fn_80169350(fn_80169364(clump))].m_world;
}
inline void *WorldOf2_(RpClump *clump) {
    return lbl_8051D208[fn_80169350(fn_80169364(clump))].m_world;
}

"""
REPL = {
  "helper_self": [(CT, H1 + CT), (LOOP2, "    fn_8003A840();\n    PlaceClumps_(this, fade);\n")],
  "helper_stage": [(CT, H2 + CT), (LOOP2, "    PlaceClumps2_(this, fn_8003A840(), fade);\n")],
  "world_inl": [(CT, H4 + CT), (LOOP2, LOOP2.replace("stage->GetAreas()[fn_80169350(fn_80169364(m_clumps[i]))].m_world", "WorldOf_(stage, m_clumps[i])"))],
  "world_inl2": [(CT, H4 + CT), (LOOP2, LOOP2.replace("stage->GetAreas()[fn_80169350(fn_80169364(m_clumps[i]))].m_world", "WorldOf2_(m_clumps[i])"))],
}
