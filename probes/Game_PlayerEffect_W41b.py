"""Wave 41: Awake's constructor and Tick. Retail keeps the awakening effect's weak_ptr address in
the register below this (r28 in the constructor, r29 in Tick): a compiler temporary, not a named
reference. Shapes reaching the effect through the inline AwakeEffect at each use."""
C = "__ct__Q226@unnamed@PlayerEffect_cpp@5AwakeFP4TaskRCQ25boost33shared_ptr<Q26Player10PlayerBase>PPQ226@unnamed@PlayerEffect_cpp@5Awakei"
T = "Tick__Q226@unnamed@PlayerEffect_cpp@5AwakeFf"
SCORE = [C, T]
CT_OLD = """    if (PlayerMotion *motion = m_player->m_motion) {
        boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);
        if (!effect.expired()) {
            effect.lock()->Retire();
        }
        effect = fn_800A2854(reinterpret_cast<Player::Effect *>(m_player->field_0x23C), 1,
                             awake->m_startEffect);
"""
CT_NEW = """    if (PlayerMotion *motion = m_player->m_motion) {
        if (!AwakeEffect(motion).expired()) {
            AwakeEffect(motion).lock()->Retire();
        }
        AwakeEffect(motion) = fn_800A2854(reinterpret_cast<Player::Effect *>(m_player->field_0x23C), 1,
                                          awake->m_startEffect);
"""
TK_OLD = """            if (PlayerMotion *motion = m_player->m_motion) {
                boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);
                if (!effect.expired()) {
                    effect.lock()->Retire();
                }
                effect = fn_800A2854(reinterpret_cast<Player::Effect *>(m_player->field_0x23C), 1,
                                     s_awakeKinds[m_kind].m_effect);
"""
TK_NEW = """            if (PlayerMotion *motion = m_player->m_motion) {
                if (!AwakeEffect(motion).expired()) {
                    AwakeEffect(motion).lock()->Retire();
                }
                AwakeEffect(motion) = fn_800A2854(reinterpret_cast<Player::Effect *>(m_player->field_0x23C), 1,
                                                  s_awakeKinds[m_kind].m_effect);
"""
REPL = {
    "inl_each": [(CT_OLD, CT_NEW), (TK_OLD, TK_NEW)],
}
TOP = "    RpClump *clump;\n    _stMtnPlayWork *work = 0;\n"
CT_PTR = """    if (PlayerMotion *motion = m_player->m_motion) {
        effect = &AwakeEffect(motion);
        if (!effect->expired()) {
            effect->lock()->Retire();
        }
        *effect = fn_800A2854(reinterpret_cast<Player::Effect *>(m_player->field_0x23C), 1,
                              awake->m_startEffect);
"""
CT_PTR2 = CT_PTR.replace("        effect = &AwakeEffect(motion);", "        boost::weak_ptr<EffectElement> *effect = &AwakeEffect(motion);")
REPL.update({
    "ptr_top": [(TOP, "    boost::weak_ptr<EffectElement> *effect;\n" + TOP), (CT_OLD, CT_PTR)],
    "ptr_after_clump": [(TOP, "    RpClump *clump;\n    boost::weak_ptr<EffectElement> *effect;\n    _stMtnPlayWork *work = 0;\n"), (CT_OLD, CT_PTR)],
    "ptr_after_work": [(TOP, TOP + "    boost::weak_ptr<EffectElement> *effect;\n"), (CT_OLD, CT_PTR)],
    "ptr_inner": [(CT_OLD, CT_PTR2)],
    "clump_reuse": [(TOP, "    _stMtnPlayWork *work = 0;\n    RpClump *clump;\n"), ],
})
HELP = """/* Starts an effect of the player in the awakening slot of its motion, retiring the one there
 * (name inferred). */
inline void StartAwakeEffect(const boost::shared_ptr<PlayerBase> &player, PlayerMotion *motion, s32 id) {
    boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);
    if (!effect.expired()) {
        effect.lock()->Retire();
    }
    effect = fn_800A2854(reinterpret_cast<Player::Effect *>(player->field_0x23C), 1, id);
}

"""
HELP2 = """inline void StartAwakeEffect(boost::weak_ptr<EffectElement> &effect, const boost::shared_ptr<PlayerBase> &player, s32 id) {
    if (!effect.expired()) {
        effect.lock()->Retire();
    }
    effect = fn_800A2854(reinterpret_cast<Player::Effect *>(player->field_0x23C), 1, id);
}

"""
ANCH = "Awake::Awake(Task *parent, const boost::shared_ptr<PlayerBase> &player, Awake **slot, int kind)\n"
CT_H = "    if (PlayerMotion *motion = m_player->m_motion) {\n        StartAwakeEffect(m_player, motion, awake->m_startEffect);\n"
TK_H = "            if (PlayerMotion *motion = m_player->m_motion) {\n                StartAwakeEffect(m_player, motion, s_awakeKinds[m_kind].m_effect);\n"
CT_H2 = "    if (PlayerMotion *motion = m_player->m_motion) {\n        StartAwakeEffect(AwakeEffect(motion), m_player, awake->m_startEffect);\n"
TK_H2 = "            if (PlayerMotion *motion = m_player->m_motion) {\n                StartAwakeEffect(AwakeEffect(motion), m_player, s_awakeKinds[m_kind].m_effect);\n"
REPL.update({
    "helper": [(ANCH, HELP + ANCH), (CT_OLD, CT_H), (TK_OLD, TK_H)],
    "helper2": [(ANCH, HELP2 + ANCH), (CT_OLD, CT_H2), (TK_OLD, TK_H2)],
})
REPL.update({
    "nomotion": [(CT_OLD, CT_OLD.replace("    if (PlayerMotion *motion = m_player->m_motion) {\n        boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);",
                                         "    if (m_player->m_motion != 0) {\n        boost::weak_ptr<EffectElement> &effect = AwakeEffect(m_player->m_motion);")),
                 (TK_OLD, TK_OLD.replace("            if (PlayerMotion *motion = m_player->m_motion) {\n                boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);",
                                         "            if (m_player->m_motion != 0) {\n                boost::weak_ptr<EffectElement> &effect = AwakeEffect(m_player->m_motion);"))],
    "nomotion_inl": [(CT_OLD, CT_NEW.replace("    if (PlayerMotion *motion = m_player->m_motion) {", "    if (m_player->m_motion != 0) {").replace("AwakeEffect(motion)", "AwakeEffect(m_player->m_motion)")),
                     (TK_OLD, TK_NEW.replace("            if (PlayerMotion *motion = m_player->m_motion) {", "            if (m_player->m_motion != 0) {").replace("AwakeEffect(motion)", "AwakeEffect(m_player->m_motion)"))],
})
# Round 3 (after Show() moved inside the motion block in Tick): copy propagation off, a typed slot.
ANCH_CT = "Awake::Awake(Task *parent, const boost::shared_ptr<PlayerBase> &player, Awake **slot, int kind)\n"
ANCH_DT = "Awake::~Awake() {\n"
TK_OLD2 = TK_OLD
REPL.update({
    "ct_off": [(ANCH_CT, "#pragma opt_propagation off\n" + ANCH_CT), (ANCH_DT, "#pragma opt_propagation reset\n" + ANCH_DT)],
    "ct_off_effptr": [(ANCH_CT, "#pragma opt_propagation off\n" + ANCH_CT), (ANCH_DT, "#pragma opt_propagation reset\n" + ANCH_DT), (CT_OLD, CT_PTR2)],
    "ct_awake_first": [("    RpClump *clump;\n    _stMtnPlayWork *work = 0;\n", "    const AwakeKind *awake;\n    RpClump *clump;\n    _stMtnPlayWork *work = 0;\n"), ("    const AwakeKind *awake = &s_awakeKinds[m_kind];\n    void *loader", "    awake = &s_awakeKinds[m_kind];\n    void *loader")],
    "ct_loader_top": [("    RpClump *clump;\n    _stMtnPlayWork *work = 0;\n", "    void *loader;\n    RpClump *clump;\n    _stMtnPlayWork *work = 0;\n"), ("    void *loader = fn_800122D4();", "    loader = fn_800122D4();")],
})
OFF_CT = [(ANCH_CT, "#pragma opt_propagation off\n" + ANCH_CT), (ANCH_DT, "#pragma opt_propagation reset\n" + ANCH_DT)]
DIRECT = "        boost::weak_ptr<EffectElement> &effect = *reinterpret_cast<boost::weak_ptr<EffectElement> *>(motion->field_0x28 + (0x48 - 0x28));"
REPL.update({
    "ct_off_direct": OFF_CT + [(CT_OLD, CT_OLD.replace("        boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);", DIRECT))],
    "ct_direct": [(CT_OLD, CT_OLD.replace("        boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);", DIRECT))],
})
TK_CUR = TK_OLD
DIRECT_T = "                boost::weak_ptr<EffectElement> &effect = *reinterpret_cast<boost::weak_ptr<EffectElement> *>(motion->field_0x28 + (0x48 - 0x28));"
REPL.update({
    "both_direct": [(CT_OLD, CT_OLD.replace("        boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);", DIRECT)),
                    (TK_CUR, TK_CUR.replace("                boost::weak_ptr<EffectElement> &effect = AwakeEffect(motion);", DIRECT_T))],
})
