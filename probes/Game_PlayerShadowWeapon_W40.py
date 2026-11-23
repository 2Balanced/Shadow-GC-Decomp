"""ShadowWeapon::GiveWeapon (wave 40): the types read in loops of one pass."""
OLD = """    int type = weapon->m_type;
    if (m_weapon) {
        if (type == m_weapon->m_type) {
            if (ammo < 0) {
                ammo = fn_8005B9AC(m_weapon->m_type);
            }
"""
W = "for (int n = 0; n < 1; n++) {\n"
def v(a, cond, arg, pre=""):
    return [(OLD, a + "    if (m_weapon) {\n" + pre + "        if (" + cond + ") {\n            if (ammo < 0) {\n                ammo = fn_8005B9AC(" + arg + ");\n            }\n")]
T = "    int type;\n    " + W + "        type = weapon->m_type;\n    }\n"
H = "        int held;\n        " + W + "            held = m_weapon->m_type;\n        }\n"
REPL = {
 "t_type": v(T, "type == m_weapon->m_type", "type"),
 "t_held": v(T, "type == m_weapon->m_type", "m_weapon->m_type"),
 "h_type": v("    int type = weapon->m_type;\n", "type == held", "type", H),
 "h_held": v("    int type = weapon->m_type;\n", "type == held", "held", H),
 "th_type": v(T, "type == held", "type", H),
 "th_held": v(T, "type == held", "held", H),
 "h_only_type": v("", "weapon->m_type == held", "weapon->m_type", H),
}
SCORE = ["GiveWeapon__Q36Player6Shadow12ShadowWeaponFRCQ25boost33shared_ptr<Q26Weapon10WeaponBase>i"]
