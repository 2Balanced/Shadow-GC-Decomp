"""BkWormBase::CreateLockons (wave 41): the wave 40 table's shapes with copy propagation off for
the function (the named-copy lever)."""
import importlib.util, os
spec = importlib.util.spec_from_file_location("w40", os.path.join("probes", "Game_BkWorm_W40.py"))
w40 = importlib.util.module_from_spec(spec); spec.loader.exec_module(w40)
SCORE = w40.SCORE
OLD = w40.OLD
REPL = {}
for k, pairs in w40.REPL.items():
    (old, new), = pairs[:1] if len(pairs) == 1 else (pairs[0],)
    REPL["prag_" + k] = [(old, "#pragma opt_propagation off\n" + new + "#pragma opt_propagation reset\n")]
W = w40.W
BODY = w40.BODY
def v(head):
    return [(OLD, "#pragma opt_propagation off\nvoid BkWormBase::CreateLockons() {\n" + head + BODY + "#pragma opt_propagation reset\n")]
REPL["prag_self_w_i"] = v("    BkWormBase *self = this;\n" + W + "    int i;\n")
REPL["prag_w_self_i"] = v(W.replace("self->", "this->") + "    BkWormBase *self = this;\n    int i;\n")
REPL["prag_self_w_i_const"] = v("    BkWormBase *const self = this;\n" + W + "    int i;\n")
REPL["prag_plain"] = [(OLD, "#pragma opt_propagation off\n" + OLD + "#pragma opt_propagation reset\n")]
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"
WP = "    const WormWeapon &weapon = lbl_804C97C0[m_params.WeaponType];\n"
LOOP = OLD[OLD.index("    for (i = 0;"):]
REPL["p_w_i"] = [(OLD, P0 + "void BkWormBase::CreateLockons() {\n" + WP + "    int i;\n" + LOOP + P1)]
REPL["p_w_fori"] = [(OLD, P0 + "void BkWormBase::CreateLockons() {\n" + WP + LOOP.replace("for (i = 0;", "for (int i = 0;") + P1)]
REPL["p_wptr_i"] = [(OLD, P0 + "void BkWormBase::CreateLockons() {\n" + "    const WormWeapon *weapon = &lbl_804C97C0[m_params.WeaponType];\n    int i;\n" + LOOP.replace("weapon.count", "weapon->count") + P1)]
REPL["p_i_wptr"] = [(OLD, P0 + "void BkWormBase::CreateLockons() {\n" + "    int i;\n    const WormWeapon *weapon = &lbl_804C97C0[m_params.WeaponType];\n" + LOOP.replace("weapon.count", "weapon->count") + P1)]
REPL["p_i_wcount"] = [(OLD, P0 + "void BkWormBase::CreateLockons() {\n" + "    int i;\n" + WP + LOOP + P1)]
FN = "void BkWormBase::CreateLockons() {\n"
LOOPB = LOOP[LOOP.index("{\n") + 2:]  # body after 'for (...) {'
FORHEAD = LOOP[:LOOP.index("{\n") + 2]
GETW = "inline const WormWeapon &WeaponOf_(const BkWormBase *worm) {\n    return lbl_804C97C0[worm->m_params.WeaponType];\n}\n\n"
def pv(text):
    return [(OLD, P0 + text + P1)]
REPL["q_inlw_i"] = [(OLD, GETW + P0 + FN + "    int i;\n    const WormWeapon &weapon = WeaponOf_(this);\n" + LOOP + P1)]
REPL["q_i_inlw_direct"] = [(OLD, GETW + P0 + FN + "    int i;\n" + LOOP.replace("weapon.count", "WeaponOf_(this).count") + P1)]
REPL["q_w_i_later"] = pv(FN + "    const WormWeapon *weapon;\n    int i;\n    weapon = &lbl_804C97C0[m_params.WeaponType];\n" + LOOP.replace("weapon.count", "weapon->count"))
REPL["q_i_w_later"] = pv(FN + "    int i;\n    const WormWeapon *weapon;\n    weapon = &lbl_804C97C0[m_params.WeaponType];\n" + LOOP.replace("weapon.count", "weapon->count"))
REPL["q_i0_w"] = pv(FN + "    int i = 0;\n" + WP + LOOP.replace("for (i = 0;", "for (;"))
REPL["q_w_i0"] = pv(FN + WP + "    int i = 0;\n" + LOOP.replace("for (i = 0;", "for (;"))
REPL["q_while"] = pv(FN + "    int i;\n" + WP + "    i = 0;\n    while (i < weapon.count) {\n" + LOOPB.replace("\n    }\n}\n", "\n        i++;\n    }\n}\n", 1))
REPL["q_self_i_w"] = pv(FN + "    BkWormBase *self = this;\n    int i;\n    const WormWeapon &weapon = lbl_804C97C0[self->m_params.WeaponType];\n" + LOOP.replace("m_lockons", "self->m_lockons"))
REPL["q_nonconst_w"] = pv(FN + "    int i;\n    WormWeapon &weapon = (WormWeapon &)lbl_804C97C0[m_params.WeaponType];\n" + LOOP)
REPL["q_idx_w"] = pv(FN + "    int i;\n    int type = m_params.WeaponType;\n    const WormWeapon &weapon = lbl_804C97C0[type];\n" + LOOP)
REPL["r_cond_rev"] = pv(FN + "    int i;\n" + WP + LOOP.replace("i < weapon.count", "weapon.count > i"))
REPL["r_countref"] = pv(FN + "    int i;\n" + WP + "    const s32 &count = weapon.count;\n" + LOOP.replace("i < weapon.count", "i < count"))
REPL["r_wptr_nc"] = pv(FN + "    int i;\n    const WormWeapon *weapon = lbl_804C97C0 + m_params.WeaponType;\n" + LOOP.replace("weapon.count", "weapon->count"))
REPL["r_static_i"] = pv(FN + WP + "    for (int i = 0; i < weapon.count; i++) {\n" + LOOPB)
REPL["r_w_in_for"] = pv(FN + "    int i;\n    for (const WormWeapon *weapon = &lbl_804C97C0[m_params.WeaponType], *dummy = (i = 0, weapon); i < weapon->count; i++) {\n" + LOOPB)
CNT = "inline s32 WeaponCount_(const WormWeapon &weapon) {\n    return weapon.count;\n}\n\n"
CNTP = "inline s32 WeaponCount_(const WormWeapon *weapon) {\n    return weapon->count;\n}\n\n"
REPL["s_cnt"] = [(OLD, CNT + P0 + FN + "    int i;\n" + WP + LOOP.replace("i < weapon.count", "i < WeaponCount_(weapon)") + P1)]
REPL["s_cnt_nop"] = [(OLD, CNT + FN + "    int i;\n" + WP + LOOP.replace("i < weapon.count", "i < WeaponCount_(weapon)"))]
REPL["s_cntp"] = [(OLD, CNTP + P0 + FN + "    int i;\n    const WormWeapon *weapon = &lbl_804C97C0[m_params.WeaponType];\n" + LOOP.replace("i < weapon.count", "i < WeaponCount_(weapon)") + P1)]
REPL["s_cnt_wfirst"] = [(OLD, CNT + P0 + FN + WP + "    int i;\n" + LOOP.replace("i < weapon.count", "i < WeaponCount_(weapon)") + P1)]
