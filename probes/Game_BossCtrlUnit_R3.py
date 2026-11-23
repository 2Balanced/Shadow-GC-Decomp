"""Wave 40: BossCtrlUnit_StageObject::Tick's clump loop in an inline helper (the loop-in-a-helper
lever that renumbered EggLastMechSlot's registers)."""
SCORE = ["Tick__24BossCtrlUnit_StageObjectFf"]
LOOP = """    void *areaWorld;
    for (int i = 0; i < 2; i++) {
        if (m_clumps[i] != 0) {
            int area = fn_80169350(fn_80169364(m_clumps[i]));
            fn_8003A840();
            areaWorld = lbl_8051D208[area].m_world;
            void *current = fn_8046256C(m_clumps[i]);
            if (alpha < 1.0f) {
                if (area < 8 && areaWorld == current) {
                    fn_804623EC(current, m_clumps[i]);
                    RpClump *clump = m_clumps[i];
                    SetClumpWorld(clump, fn_8003A840()->GetWorld(9));
                }
            } else if (areaWorld != current) {
                fn_804623EC(current, m_clumps[i]);
                RpClump *clump = m_clumps[i];
                SetClumpWorld(clump, areaWorld);
            }
        }
    }
"""
F = "int BossCtrlUnit_StageObject::Tick(float dt) {"
H = """inline void UpdateClumps_(BossCtrlUnit_StageObject *self, float alpha) {
    void *areaWorld;
    for (int i = 0; i < 2; i++) {
        if (self->m_clumps[i] != 0) {
            int area = fn_80169350(fn_80169364(self->m_clumps[i]));
            fn_8003A840();
            areaWorld = lbl_8051D208[area].m_world;
            void *current = fn_8046256C(self->m_clumps[i]);
            if (alpha < 1.0f) {
                if (area < 8 && areaWorld == current) {
                    fn_804623EC(current, self->m_clumps[i]);
                    RpClump *clump = self->m_clumps[i];
                    void *world = fn_8003A840()->GetWorld(9);
                    void *now = fn_8046256C(clump);
                    if (now != world) {
                        if (now != 0) {
                            fn_804623EC(now, clump);
                        }
                        fn_804622FC(world, clump);
                    }
                }
            } else if (areaWorld != current) {
                fn_804623EC(current, self->m_clumps[i]);
                RpClump *clump = self->m_clumps[i];
                void *now = fn_8046256C(clump);
                if (now != areaWorld) {
                    if (now != 0) {
                        fn_804623EC(now, clump);
                    }
                    fn_804622FC(areaWorld, clump);
                }
            }
        }
    }
}

"""
REPL = {
  "loop_helper": [(F, H + F), (LOOP, "    UpdateClumps_(this, alpha);\n")],
}
