"""Wave 41: NpcSonicType::UpdateTargets (kind's li of 0 after the flags load in retail) and
UpdateAttack, with copy propagation off and named copies (the wave-40 lever)."""
SCORE = ["UpdateTargets__Q233@unnamed@NpcControlSonicType_cpp@12NpcSonicTypeFf"]
F = "void NpcSonicType::UpdateTargets(float dt) {"
END = "void NpcSonicType::DecayTimers(float dt) {"
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
TOP = "    int kind = 0;\n    shared_ptr<Target> target = Fields(m_npc.get())->m_aim->vf_0x28().lock();\n"
IFB = "    if (target && m_state != 3) {\n        if (target->m_flags & 0x18) {\n"
def late(decl_top=True, where="if"):
    pairs = [(TOP, ("    int kind;\n" if decl_top else "") + "    shared_ptr<Target> target = Fields(m_npc.get())->m_aim->vf_0x28().lock();\n")]
    k = "        int kind = 0;\n" if not decl_top else "        kind = 0;\n"
    pairs.append((IFB, "    if (target && m_state != 3) {\n" + k + "        if (target->m_flags & 0x18) {\n"))
    return pairs
REPL = {
  "prag": P,
  "late": late(True),
  "late_decl": late(False),
  "prag_late": P + late(True),
  "prag_late_decl": P + late(False),
  "prag_dt": P + [(TOP, "    float time = dt;\n" + TOP), ("                timer -= dt;\n                timer = fn_80070494(timer, s_zero);\n            }\n        }\n        if (kind != 0) {\n            float &timer = m_timers[kind];\n            timer += dt;", "                timer -= time;\n                timer = fn_80070494(timer, s_zero);\n            }\n        }\n        if (kind != 0) {\n            float &timer = m_timers[kind];\n            timer += time;")],
  "prag_self": P + [(TOP, TOP + "    NpcSonicType *self = this;\n")],
}

SA = "UpdateAttack__Q233@unnamed@NpcControlSonicType_cpp@12NpcSonicTypeFf"
FA = "void NpcSonicType::UpdateAttack(float dt) {"
EA = "void NpcSonicType::UpdateFollow(float dt) {"
PA = [(FA, "#pragma opt_propagation off\n" + FA), (EA, "#pragma opt_propagation reset\n" + EA)]
CP = "inline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n    return lbl_804F1834[Fields(player)->m_kind];\n}\n"
C1 = "                CharaParam(m_npc.get()).m_canHomingAttack) {"
C2 = "            if (horizontal < 70.0f && CharaParam(m_npc.get()).m_canHomingAttack) {"
REPL.update({
  "a_prag": PA,
  "a_prag_cp_copy": PA + [(CP, "inline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n    PlayerBase *p = player;\n    return lbl_804F1834[Fields(p)->m_kind];\n}\n")],
  "a_prag_cp_kind": PA + [(CP, "inline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n    s32 kind = Fields(player)->m_kind;\n    return lbl_804F1834[kind];\n}\n")],
  "a_prag_npc": PA + [(C1, "                CharaParam(Npc_(m_npc)).m_canHomingAttack) {"), (C2, "            if (horizontal < 70.0f && CharaParam(Npc_(m_npc)).m_canHomingAttack) {"), (CP, CP + "inline PlayerBase *Npc_(const shared_ptr<Player::Npc::Npc> &n) { PlayerBase *p = n.get(); return p; }\n")],
})
SC = {k: [SA] for k in ["a_prag", "a_prag_cp_copy", "a_prag_cp_kind", "a_prag_npc"]}
SCORE = ["UpdateTargets__Q233@unnamed@NpcControlSonicType_cpp@12NpcSonicTypeFf", SA]

def cpv(body):
    return [(CP, "inline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n" + body + "}\n")]
V = {
  "ptradd": "    return *(lbl_804F1834 + Fields(player)->m_kind);\n",
  "idxswap": "    return (Fields(player)->m_kind)[lbl_804F1834];\n",
  "tablelocal": "    const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    return table[Fields(player)->m_kind];\n",
  "tablelocal2": "    const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    s32 kind = Fields(player)->m_kind;\n    return table[kind];\n",
  "kindfirst": "    s32 kind = Fields(player)->m_kind;\n    const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    return table[kind];\n",
  "tableadd": "    const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    table += Fields(player)->m_kind;\n    return *table;\n",
  "copy_player": "    PlayerBase *p = player;\n    return lbl_804F1834[Fields(p)->m_kind];\n",
}
for k, b in V.items():
    REPL["c_" + k] = cpv(b)
    REPL["cp_" + k] = PA + cpv(b)

V2 = {
  "ctable": "    const UnkCharaParam_804F1834 *const table = lbl_804F1834;\n    return table[Fields(player)->m_kind];\n",
  "ctable_kind": "    const UnkCharaParam_804F1834 *const table = lbl_804F1834;\n    s32 kind = Fields(player)->m_kind;\n    return table[kind];\n",
  "kind_ctable": "    s32 kind = Fields(player)->m_kind;\n    const UnkCharaParam_804F1834 *const table = lbl_804F1834;\n    return table[kind];\n",
  "ckind": "    const s32 kind = Fields(player)->m_kind;\n    return lbl_804F1834[kind];\n",
}
for k, b in V2.items():
    REPL["c_" + k] = cpv(b)

WJ = "                new WatchJump(m_npc, weak_ptr<PlayerBase>(), 0.0f);\n"
TL = cpv(V["tablelocal"])
REPL.update({
  "w_delay": PA + TL + [(WJ, "                float delay = 0.0f;\n                new WatchJump(m_npc, weak_ptr<PlayerBase>(), delay);\n")],
  "w_npcref": PA + TL + [(WJ, "                const shared_ptr<Npc> &npc = m_npc;\n                new WatchJump(npc, weak_ptr<PlayerBase>(), 0.0f);\n")],
  "w_ptr": PA + TL + [(WJ, "                WatchJump *jump = new WatchJump(m_npc, weak_ptr<PlayerBase>(), 0.0f);\n")],
  "w_cast": PA + TL + [(WJ, "                new WatchJump(m_npc, (const weak_ptr<PlayerBase> &)weak_ptr<PlayerBase>(), 0.0f);\n")],
})

HELP = "inline s32 KindOf_(PlayerBase *p) { return Fields(p)->m_kind; }\ninline const UnkCharaParam_804F1834 *Table_() { return lbl_804F1834; }\n"
def cpvh(body):
    return [(CP, HELP + "inline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n" + body + "}\n")]
V3 = {
  "regtable": "    register const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    return table[Fields(player)->m_kind];\n",
  "kindof": "    return lbl_804F1834[KindOf_(player)];\n",
  "tablefn": "    return Table_()[Fields(player)->m_kind];\n",
  "tablefn_kindof": "    return Table_()[KindOf_(player)];\n",
  "tablelocal_kindof": "    const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    return table[KindOf_(player)];\n",
}
for k, b in V3.items():
    REPL["h_" + k] = cpvh(b)

V4 = {
  "reftable": "    const UnkCharaParam_804F1834 *const &table = lbl_804F1834;\n    return table[Fields(player)->m_kind];\n",
  "arrref": "    const UnkCharaParam_804F1834 (&table)[1] = *(const UnkCharaParam_804F1834 (*)[1])lbl_804F1834;\n    return table[Fields(player)->m_kind];\n",
  "elem_ptr": "    const UnkCharaParam_804F1834 *param = lbl_804F1834 + Fields(player)->m_kind;\n    return *param;\n",
  "elem_ref": "    const UnkCharaParam_804F1834 &param = lbl_804F1834[Fields(player)->m_kind];\n    return param;\n",
}
for k, b in V4.items():
    REPL["c_" + k] = cpv(b)
    REPL["cp_" + k] = PA + cpv(b)

CPN = "/* The parameters of a player's character (the table through a local: see UpdateAttack). */\ninline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n    const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    return table[Fields(player)->m_kind];\n}\n"
UA = "/* NonMatching (2 instructions)"
def cpn(body, prag=False):
    pairs = [(CPN, "inline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n" + body + "}\n")]
    if not prag:
        pairs += [("#pragma opt_propagation off\nvoid NpcSonicType::UpdateAttack(float dt) {", "void NpcSonicType::UpdateAttack(float dt) {")]
    return pairs
V5 = {
  "bytes": "    return *(const UnkCharaParam_804F1834 *)((const u8 *)lbl_804F1834 + Fields(player)->m_kind * sizeof(UnkCharaParam_804F1834));\n",
  "bytes2": "    const u8 *table = (const u8 *)lbl_804F1834;\n    return *(const UnkCharaParam_804F1834 *)(table + Fields(player)->m_kind * sizeof(UnkCharaParam_804F1834));\n",
  "bytes3": "    u32 offset = Fields(player)->m_kind * sizeof(UnkCharaParam_804F1834);\n    return *(const UnkCharaParam_804F1834 *)((const u8 *)lbl_804F1834 + offset);\n",
  "bytes4": "    return *(const UnkCharaParam_804F1834 *)(Fields(player)->m_kind * sizeof(UnkCharaParam_804F1834) + (const u8 *)lbl_804F1834);\n",
}
for k, b in V5.items():
    REPL["n_" + k] = cpn(b)
    REPL["np_" + k] = cpn(b, True)

C1N = "                CharaParam(m_npc.get()).m_canHomingAttack) {"
C2N = "            if (horizontal < 70.0f && CharaParam(m_npc.get()).m_canHomingAttack) {"
NOP = ("#pragma opt_propagation off\nvoid NpcSonicType::UpdateAttack(float dt) {", "void NpcSonicType::UpdateAttack(float dt) {")
def kv(helper, call):
    return [NOP, (CPN, CPN + helper), (C1N, "                " + call + ".m_canHomingAttack) {"), (C2N, "            if (horizontal < 70.0f && " + call + ".m_canHomingAttack) {")]
H1 = "inline const UnkCharaParam_804F1834 &CharaParamOf(s32 kind) {\n    return lbl_804F1834[kind];\n}\n"
H2 = "inline const UnkCharaParam_804F1834 &CharaParamOf(s32 kind) {\n    const UnkCharaParam_804F1834 *table = lbl_804F1834;\n    return table[kind];\n}\n"
H3 = "inline const UnkCharaParam_804F1834 &CharaParamOf(const UnkCharaParam_804F1834 *table, s32 kind) {\n    return table[kind];\n}\n"
REPL.update({
  "k_of": kv(H1, "CharaParamOf(Fields(m_npc.get())->m_kind)"),
  "k_of2": kv(H2, "CharaParamOf(Fields(m_npc.get())->m_kind)"),
  "k_of3": kv(H3, "CharaParamOf(lbl_804F1834, Fields(m_npc.get())->m_kind)"),
})

WD = "    WatchJump(const shared_ptr<Npc> &npc, const weak_ptr<PlayerBase> &leader, float delay);"
WJ2 = "                new WatchJump(m_npc, weak_ptr<PlayerBase>(), 0.0f);\n"
REPL.update({
  "d_both": [(WD, "    WatchJump(const shared_ptr<Npc> &npc, const weak_ptr<PlayerBase> &leader = weak_ptr<PlayerBase>(), float delay = 0.0f);"), (WJ2, "                new WatchJump(m_npc);\n")],
  "d_delay": [(WD, "    WatchJump(const shared_ptr<Npc> &npc, const weak_ptr<PlayerBase> &leader, float delay = 0.0f);"), (WJ2, "                new WatchJump(m_npc, weak_ptr<PlayerBase>());\n")],
})

REPL["sz_np"] = [NOP, (CPN, "inline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n    return lbl_804F1834[Fields(player)->m_kind];\n}\n")]
REPL["sz_p"] = [(CPN, "inline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {\n    return lbl_804F1834[Fields(player)->m_kind];\n}\n")]
REPL["sz_np_tl"] = [NOP]
HDR = {k: {"Game/PlayerCharaParam.h": [("extern const UnkCharaParam_804F1834 lbl_804F1834[];", "extern const UnkCharaParam_804F1834 lbl_804F1834[12];")]} for k in ("sz_np", "sz_p", "sz_np_tl")}
