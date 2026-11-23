"""Wave 40: MotionEyeBlink's constructor with its first loop in an inline helper (in
EggLastMechSlot that renumbered the second loop as retail has it)."""
SCORE = ["__ct__Q36Player6Shadow14MotionEyeBlinkFRCQ25boost33shared_ptr<Q26Player10PlayerBase>PCQ26Player22UnkMotionNode_800AA760P21UnkClumpFade_80027D5CRC23UnkMotionFiles_800ADE78"]
LOOP1 = """    void *loader = fn_800122D4();
    for (int i = 0; i < 5; i++) {
        int index = files.m_set2[i].m_index;
        if (index >= 0) {
            m_clumps[i] = fn_8001277C(loader, files.m_set2[i].m_file, index);
        } else {
            m_clumps[i] = 0;
        }
    }
"""
CT = "MotionEyeBlink::MotionEyeBlink(const shared_ptr<PlayerBase> &player, const UnkMotionNode_800AA760 *node,"
H1 = """inline void LoadClumps_(MotionEyeBlink *self, void *loader, const UnkMotionFiles_800ADE78 &files) {
    for (int i = 0; i < 5; i++) {
        int index = files.m_set2[i].m_index;
        if (index >= 0) {
            self->m_clumps[i] = fn_8001277C(loader, files.m_set2[i].m_file, index);
        } else {
            self->m_clumps[i] = 0;
        }
    }
}
inline void LoadClumps2_(MotionEyeBlink *self, const UnkMotionFiles_800ADE78 &files) {
    void *loader = fn_800122D4();
    for (int i = 0; i < 5; i++) {
        int index = files.m_set2[i].m_index;
        if (index >= 0) {
            self->m_clumps[i] = fn_8001277C(loader, files.m_set2[i].m_file, index);
        } else {
            self->m_clumps[i] = 0;
        }
    }
}

"""
REPL = {
  "l1_helper": [(CT, H1 + CT), (LOOP1, "    LoadClumps_(this, fn_800122D4(), files);\n")],
  "l1_helper_loader": [(CT, H1 + CT), (LOOP1, "    void *loader = fn_800122D4();\n    LoadClumps_(this, loader, files);\n")],
  "l1_helper2": [(CT, H1 + CT), (LOOP1, "    LoadClumps2_(this, files);\n")],
}
