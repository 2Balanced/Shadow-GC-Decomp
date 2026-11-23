"""JumpDash::vf_0x0C (wave 40): loops of one pass."""
O = "        return lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_canJumpDash;\n"
W = "for (int n = 0; n < 1; n++) {\n"
REPL = {
 "for_ret": [(O, "        " + W + "    " + O + "        }\n")],
 "for_kind": [(O, "        int kind;\n        " + W + "            kind = FieldsOf(m_player.get())->m_kind;\n        }\n        return lbl_804F1834[kind].m_canJumpDash;\n")],
 "for_res": [(O, "        bool result;\n        " + W + "            result = lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_canJumpDash;\n        }\n        return result;\n")],
 "for_all": [("    if (fn_80014E34(&m_player.get()->m_flags, 2)) {\n" + O + "    }\n    return false;\n", "    " + W + "        if (fn_80014E34(&m_player.get()->m_flags, 2)) {\n    " + O + "        }\n    }\n    return false;\n")],
 "for_player": [(O, "        PlayerBase *player;\n        " + W + "            player = m_player.get();\n        }\n        return lbl_804F1834[FieldsOf(player)->m_kind].m_canJumpDash;\n")],
}
SCORE = ["vf_0x0C__Q36Player8Behavior8JumpDashFiRCQ36Player8Behavior14InitializeArgs"]
