"""Wave 40: Ground::SetSpeedRange. Retail forms the parameter table's address (r3) before it loads
the player (r4); GC/2.7 the other way round."""
F = "SetSpeedRange__Q36Player8Behavior6GroundFv"
SYMBOLS = {F: "void Ground::SetSpeedRange() {"}
def b(expr, pre=""):
    return """    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
        m_minSpeed = 1000.0f;
        m_maxSpeed = 3000.0f;
    } else {
        m_minSpeed = 1000.0f;
""" + pre + "        m_maxSpeed = 15000.0f * " + expr + ";\n    }\n"
BODIES = {F: {
    "base": b("lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_runSpeed"),
    "px": b("lbl_804F1834[FieldsOf(m_player.px)->m_kind].m_runSpeed"),
    "kind_local": b("lbl_804F1834[kind].m_runSpeed", "        int kind = FieldsOf(m_player.get())->m_kind;\n"),
    "row_local": b("row->m_runSpeed", "        const PlayerCharaParam *row = &lbl_804F1834[FieldsOf(m_player.get())->m_kind];\n"),
    "ptr_arith": b("(lbl_804F1834 + FieldsOf(m_player.get())->m_kind)->m_runSpeed"),
    "ptr_arith_px": b("(lbl_804F1834 + FieldsOf(m_player.px)->m_kind)->m_runSpeed"),
    "table_local": b("table[FieldsOf(m_player.get())->m_kind].m_runSpeed", "        const PlayerCharaParam *table = lbl_804F1834;\n"),
    "table_local_px": b("table[FieldsOf(m_player.px)->m_kind].m_runSpeed", "        const PlayerCharaParam *table = lbl_804F1834;\n"),
    "arrow": b("lbl_804F1834[FieldsOf(m_player.operator->())->m_kind].m_runSpeed"),
    "rev_idx": b("(FieldsOf(m_player.get())->m_kind)[lbl_804F1834].m_runSpeed"),
}}
