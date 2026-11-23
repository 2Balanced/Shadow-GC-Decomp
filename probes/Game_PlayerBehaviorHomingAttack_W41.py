"""Wave 41: HomingAttack::vf_0x0C (retail: player r4, table address r3). The one-pass loop of
JumpDash::vf_0x0C and the wave 40 lever (named copy + '#pragma opt_propagation off')."""
F = "vf_0x0C__Q36Player8Behavior12HomingAttackFiRCQ36Player8Behavior14InitializeArgs"
SCORE = [F]
H = "bool HomingAttack::vf_0x0C(int prevId, const InitializeArgs &args) {\n"
B = "    return lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_canHomingAttack;\n}\n"
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"
def v(body, prag=False):
    if prag:
        return [(H + B, P0 + H + body + P1)]
    return [(H + B, H + body)]
LOOP = """    int kind;
    for (int n = 0; n < 1; n++) {
        kind = FieldsOf(m_player.get())->m_kind;
    }
    return lbl_804F1834[kind].m_canHomingAttack;
}
"""
PL = "    PlayerBase *player = m_player.get();\n    return lbl_804F1834[FieldsOf(player)->m_kind].m_canHomingAttack;\n}\n"
TB = "    const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    return table[FieldsOf(m_player.get())->m_kind].m_canHomingAttack;\n}\n"
SELF = "    HomingAttack *self = this;\n    return lbl_804F1834[FieldsOf(self->m_player.get())->m_kind].m_canHomingAttack;\n}\n"
KIND = "    int kind = FieldsOf(m_player.get())->m_kind;\n    return lbl_804F1834[kind].m_canHomingAttack;\n}\n"
TBPL = "    const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    PlayerBase *player = m_player.get();\n    return table[FieldsOf(player)->m_kind].m_canHomingAttack;\n}\n"
PLTB = "    PlayerBase *player = m_player.get();\n    const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    return table[FieldsOf(player)->m_kind].m_canHomingAttack;\n}\n"
PADD = "    const UnkCharaParam_804F1834 *p = lbl_804F1834;\n    p += FieldsOf(m_player.get())->m_kind;\n    return p->m_canHomingAttack;\n}\n"
REPL = {
  "loop": v(LOOP),
  "prag": v(B, True),
  "prag_player": v(PL, True),
  "prag_table": v(TB, True),
  "prag_self": v(SELF, True),
  "prag_kind": v(KIND, True),
  "prag_table_player": v(TBPL, True),
  "prag_player_table": v(PLTB, True),
  "prag_padd": v(PADD, True),
}
