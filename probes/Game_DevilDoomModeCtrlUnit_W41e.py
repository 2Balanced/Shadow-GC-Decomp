"""Wave 41: BELOW_HALF's constructor: where moveCamera is declared."""
SCORE = ["__ct__32DevilDoomModeCtrlUnit_BELOW_HALFFP17DevilDoomVarParam"]
D = """        bool moveCamera = false;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 30.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xD);
        if (m_owner->m_eyeOpen[1]) {
"""
L1 = "        fn_804165D8(m_owner->m_motion, 0, -1, -1);\n"
L2 = "        fn_80416148(m_owner->m_motion, 0, 30.0f);\n"
L3 = "        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xD);\n"
M = "        bool moveCamera = false;\n"
REPL = {
  "after1": [(D, L1 + M + L2 + L3 + "        if (m_owner->m_eyeOpen[1]) {\n")],
  "after2": [(D, L1 + L2 + M + L3 + "        if (m_owner->m_eyeOpen[1]) {\n")],
  "after3": [(D, L1 + L2 + L3 + M + "        if (m_owner->m_eyeOpen[1]) {\n")],
  "decl_assign_after3": [(D, "        bool moveCamera;\n" + L1 + L2 + L3 + "        moveCamera = false;\n        if (m_owner->m_eyeOpen[1]) {\n")],
  "block": [(D, L1 + L2 + L3 + "        {\n" + M + "        if (m_owner->m_eyeOpen[1]) {\n"), ("        m_owner->m_active = 0;\n        if (m_owner->m_eyeOpen[0]) {", "        }\n        m_owner->m_active = 0;\n        if (m_owner->m_eyeOpen[0]) {")],
}
