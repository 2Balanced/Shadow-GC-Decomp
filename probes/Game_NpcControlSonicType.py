"""NpcSonicType::UpdateAttack (wave 39, second pass): retail forms the character table's address in
r3 and the offset in r0."""
A = "inline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n    return lbl_804F1834[Fields(player)->m_kind];\n}\n"
C1 = "CharaParam(m_npc.get()).m_canHomingAttack"
def f(body, sig="inline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {"):
    return [(A, sig + "\n" + body + "}\n")]
REPL = {
 "u32": f("    return lbl_804F1834[(u32)Fields(player)->m_kind];\n"),
 "ptr_add": f("    return *(lbl_804F1834 + Fields(player)->m_kind);\n"),
 "addr": f("    return *&lbl_804F1834[Fields(player)->m_kind];\n"),
 "kind_local": f("    s32 kind = Fields(player)->m_kind;\n    return lbl_804F1834[kind];\n"),
 "ret_ptr": [(A, "inline const UnkCharaParam_804F1834 *CharaParam(PlayerBase *player) {\n    return &lbl_804F1834[Fields(player)->m_kind];\n}\n"), (C1, "CharaParam(m_npc.get())->m_canHomingAttack"), (C1, "CharaParam(m_npc.get())->m_canHomingAttack")],
 "by_kind": [(A, "inline const UnkCharaParam_804F1834 &CharaParam(s32 kind) {\n    return lbl_804F1834[kind];\n}\n"), (C1, "CharaParam(Fields(m_npc.get())->m_kind).m_canHomingAttack"), (C1, "CharaParam(Fields(m_npc.get())->m_kind).m_canHomingAttack")],
 "by_kind_u": [(A, "inline const UnkCharaParam_804F1834 &CharaParam(u32 kind) {\n    return lbl_804F1834[kind];\n}\n"), (C1, "CharaParam(Fields(m_npc.get())->m_kind).m_canHomingAttack"), (C1, "CharaParam(Fields(m_npc.get())->m_kind).m_canHomingAttack")],
 "direct": [(C1, "lbl_804F1834[Fields(m_npc.get())->m_kind].m_canHomingAttack"), (C1, "lbl_804F1834[Fields(m_npc.get())->m_kind].m_canHomingAttack")],
 "tbl_local": f("    const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    return table[Fields(player)->m_kind];\n"),
}
SCORE = ["UpdateAttack__Q233@unnamed@NpcControlSonicType_cpp@12NpcSonicTypeFf"]
REPL = {
 "p_plus": f("    const UnkCharaParam_804F1834 *p = lbl_804F1834;\n    p += Fields(player)->m_kind;\n    return *p;\n"),
 "idx_first": f("    return Fields(player)->m_kind[lbl_804F1834];\n"),
 "kind_plus": f("    return *(Fields(player)->m_kind + lbl_804F1834);\n"),
 "kind_getter": [(A, "inline s32 KindOf(PlayerBase *player) {\n    return Fields(player)->m_kind;\n}\ninline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n    return lbl_804F1834[KindOf(player)];\n}\n")],
 "tbl_getter": [(A, "inline const UnkCharaParam_804F1834 *CharaTable() {\n    return lbl_804F1834;\n}\ninline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n    return CharaTable()[Fields(player)->m_kind];\n}\n")],
 "both_getters": [(A, "inline s32 KindOf(PlayerBase *player) {\n    return Fields(player)->m_kind;\n}\ninline const UnkCharaParam_804F1834 &ParamOf(s32 kind) {\n    return lbl_804F1834[kind];\n}\ninline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n    return ParamOf(KindOf(player));\n}\n")],
}
