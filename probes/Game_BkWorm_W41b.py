"""BkWormBase::CreateLockons (wave 41, round 2): a grid of weapon forms x declaration orders x copy
propagation, over the source that has the pragma already (counter r28 / weapon r27 left)."""
SRC = open("src/Game/BkWorm.cpp", encoding="utf-8").read()
SCORE = ["CreateLockons__10BkWormBaseFv"]
s = SRC.index("#pragma opt_propagation off\nvoid BkWormBase::CreateLockons() {")
e = SRC.index("#pragma opt_propagation reset\n", s) + len("#pragma opt_propagation reset\n")
FN = SRC[s:e]
LOOPBODY = FN[FN.index("        m_lockons[i] = "):FN.rindex("    }\n}\n")]
HELP = {
  "ref": ("", "    const WormWeapon &weapon = lbl_804C97C0[m_params.WeaponType];\n", "weapon.count"),
  "ptr": ("", "    const WormWeapon *weapon = &lbl_804C97C0[m_params.WeaponType];\n", "weapon->count"),
  "iref": ("inline const WormWeapon &WeaponOf_(const BkWormBase *worm) {\n    return lbl_804C97C0[worm->m_params.WeaponType];\n}\n\n",
           "    const WormWeapon &weapon = WeaponOf_(this);\n", "weapon.count"),
  "iptr": ("inline const WormWeapon *WeaponOf_(const BkWormBase *worm) {\n    return &lbl_804C97C0[worm->m_params.WeaponType];\n}\n\n",
           "    const WormWeapon *weapon = WeaponOf_(this);\n", "weapon->count"),
  "type": ("", "    s32 type = m_params.WeaponType;\n    const WormWeapon &weapon = lbl_804C97C0[type];\n", "weapon.count"),
}
REPL = {}
for hk, (pre, decl, cnt) in HELP.items():
    for order in ("iw", "wi", "forw"):
        for prag in (0, 1):
            if order == "iw":
                head = "    int i;\n" + decl
                loop = "    for (i = 0; i < " + cnt + "; i++) {\n"
            elif order == "wi":
                head = decl + "    int i;\n"
                loop = "    for (i = 0; i < " + cnt + "; i++) {\n"
            else:
                head = decl
                loop = "    for (int i = 0; i < " + cnt + "; i++) {\n"
            body = "void BkWormBase::CreateLockons() {\n" + head + loop + LOOPBODY + "    }\n}\n"
            if prag:
                body = "#pragma opt_propagation off\n" + body + "#pragma opt_propagation reset\n"
            REPL[hk + "_" + order + ("_p" if prag else "")] = [(FN, pre + body)]
