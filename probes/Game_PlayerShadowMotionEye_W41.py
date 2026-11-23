"""Wave 41: MotionEyeBlink's constructor, second loop (retail: clump offset r26, world r28), with
copy propagation off for the function and named copies of this, the fade and the clump."""
SCORE = ["__ct__Q36Player6Shadow14MotionEyeBlinkFRCQ25boost33shared_ptr<Q26Player10PlayerBase>PCQ26Player22UnkMotionNode_800AA760P21UnkClumpFade_80027D5CRC23UnkMotionFiles_800ADE78"]
CT = "MotionEyeBlink::MotionEyeBlink(const shared_ptr<PlayerBase> &player, const UnkMotionNode_800AA760 *node,"
END = "void MotionEyeBlink::Update(float dt) {"
OFF = [(CT, "#pragma opt_propagation off\n" + CT), (END, "#pragma opt_propagation reset\n" + END)]
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
def L2(new): return [(LOOP2, new)]
REPL = {
  "off": OFF,
  "off_self": OFF + L2(LOOP2.replace("    UnkStage_8003A840 *stage", "    MotionEyeBlink *self = this;\n    UnkStage_8003A840 *stage").replace("m_clumps[i]", "self->m_clumps[i]")),
  "off_self_first": OFF + [("    : m_state(0), m_step(0), m_time(0.0f), m_current(0), m_hidden(0) {\n", "    : m_state(0), m_step(0), m_time(0.0f), m_current(0), m_hidden(0) {\n    MotionEyeBlink *self = this;\n")] + L2(LOOP2.replace("m_clumps[i]", "self->m_clumps[i]")),
  "off_fade": OFF + L2(LOOP2.replace("    UnkStage_8003A840 *stage", "    UnkClumpFade_80027D5C *f = fade;\n    UnkStage_8003A840 *stage").replace(", fade);", ", f);")),
  "off_world_top": OFF + L2(LOOP2.replace("    UnkStage_8003A840 *stage", "    void *world;\n    UnkStage_8003A840 *stage").replace("void *world = ", "world = ")),
  "off_world_after": OFF + L2(LOOP2.replace("    UnkStage_8003A840 *stage = fn_8003A840();\n", "    UnkStage_8003A840 *stage = fn_8003A840();\n    void *world;\n").replace("void *world = ", "world = ")),
  "off_clump": OFF + L2("""    UnkStage_8003A840 *stage = fn_8003A840();
    for (int i = 0; i < 5; i++) {
        if (m_clumps[i] != 0) {
            m_clumps[i] = fn_80457090(m_clumps[i]);
            RpClump *clump = m_clumps[i];
            void *world = stage->GetAreas()[fn_80169350(fn_80169364(clump))].m_world;
            if (world != 0) {
                SetClumpWorld(m_clumps[i], world);
            }
            fn_80038FC0(m_clumps[i]);
            fn_80038198(m_clumps[i], fade);
        }
    }
"""),
  "off_slot": OFF + L2("""    UnkStage_8003A840 *stage = fn_8003A840();
    for (int i = 0; i < 5; i++) {
        RpClump **slot = &m_clumps[i];
        if (*slot != 0) {
            *slot = fn_80457090(*slot);
            void *world = stage->GetAreas()[fn_80169350(fn_80169364(*slot))].m_world;
            if (world != 0) {
                SetClumpWorld(*slot, world);
            }
            fn_80038FC0(*slot);
            fn_80038198(*slot, fade);
        }
    }
"""),
  "slot": L2("""    UnkStage_8003A840 *stage = fn_8003A840();
    for (int i = 0; i < 5; i++) {
        RpClump **slot = &m_clumps[i];
        if (*slot != 0) {
            *slot = fn_80457090(*slot);
            void *world = stage->GetAreas()[fn_80169350(fn_80169364(*slot))].m_world;
            if (world != 0) {
                SetClumpWorld(*slot, world);
            }
            fn_80038FC0(*slot);
            fn_80038198(*slot, fade);
        }
    }
"""),
  "off_stage_copy": OFF + L2(LOOP2.replace("    for (int i = 0; i < 5; i++) {\n        if (m_clumps", "    UnkStage_8003A840 *st = stage;\n    for (int i = 0; i < 5; i++) {\n        if (m_clumps").replace("stage->GetAreas()", "st->GetAreas()")),
}
