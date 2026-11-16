"""ShadowWeapon::GiveWeapon (wave 39, second pass): retail has the given weapon's type in r0 and the
held one's in r4, and passes the given one to fn_8005B9AC. The scores count the branch targets as
differences (8 rows of noise in this function)."""
OLD = """    int type = weapon->m_type;
    if (m_weapon) {
        if (type == m_weapon->m_type) {
            if (ammo < 0) {
                ammo = fn_8005B9AC(m_weapon->m_type);
            }
"""
def v(a, b, c):
    return [(OLD, OLD.replace("    int type = weapon->m_type;\n", a).replace("type == m_weapon->m_type", b).replace("fn_8005B9AC(m_weapon->m_type)", c))]
T = "    int type = weapon->m_type;\n"
TG = "    int type = weapon->TypeId();\n"
REPL = {
 "call_type": v(T, "type == m_weapon->m_type", "fn_8005B9AC(type)"),
 "g_call_type": v(TG, "type == m_weapon->TypeId()", "fn_8005B9AC(type)"),
 "g_call_held": v(TG, "type == m_weapon->TypeId()", "fn_8005B9AC(m_weapon->TypeId())"),
 "a": v(TG, "type == m_weapon->TypeId()", "fn_8005B9AC(weapon->TypeId())"),
 "b": v(TG, "type == m_weapon->m_type", "fn_8005B9AC(weapon->TypeId())"),
 "c": v(T, "type == m_weapon->TypeId()", "fn_8005B9AC(weapon->TypeId())"),
 "d": v(T, "type == m_weapon->TypeId()", "fn_8005B9AC(weapon->m_type)"),
 "e": v(T, "type == m_weapon->m_type", "fn_8005B9AC(weapon->m_type)"),
 "f": v(TG, "type == m_weapon->TypeId()", "fn_8005B9AC(weapon->m_type)"),
 "h": v("    const int type = weapon->TypeId();\n", "type == m_weapon->TypeId()", "fn_8005B9AC(type)"),
 "j": v("    int type = weapon->TypeId();\n    int held;\n", "type == (held = m_weapon->TypeId())", "fn_8005B9AC(held)"),
 "k": v("", "weapon->TypeId() == m_weapon->TypeId()", "fn_8005B9AC(m_weapon->TypeId())"),
}
SCORE = ["GiveWeapon__Q36Player6Shadow12ShadowWeaponFRCQ25boost33shared_ptr<Q26Weapon10WeaponBase>i"]
