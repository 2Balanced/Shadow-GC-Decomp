"""Wave 41: Ground::SetSpeedRange (retail: table address r3, player r4) with
'#pragma opt_propagation off' scoped to it (and the one-pass loop of JumpDash::vf_0x0C)."""
F = "SetSpeedRange__Q36Player8Behavior6GroundFv"
SCORE = [F]
H = "void Ground::SetSpeedRange() {\n"
B = """    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
        m_minSpeed = 1000.0f;
        m_maxSpeed = 3000.0f;
    } else {
        m_minSpeed = 1000.0f;
        m_maxSpeed = 15000.0f * lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_runSpeed;
    }
}
"""
LOOP = B.replace("        m_maxSpeed = 15000.0f * lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_runSpeed;\n",
 "        int kind;\n        for (int n = 0; n < 1; n++) {\n            kind = FieldsOf(m_player.get())->m_kind;\n        }\n        m_maxSpeed = 15000.0f * lbl_804F1834[kind].m_runSpeed;\n")
REPL = {
  "prag": [(H + B, "#pragma opt_propagation off\n" + H + B + "#pragma opt_propagation reset\n")],
  "loop": [(H + B, H + LOOP)],
}
