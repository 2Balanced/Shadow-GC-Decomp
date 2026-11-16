"""ShadowWeapon::GiveWeapon (wave 39, second pass): the test against the held weapon as an inline
member taking the type by value. 8 rows of the score are branch-target noise."""
A = "void ShadowWeapon::GiveWeapon(const boost::shared_ptr<Weapon::WeaponBase> &weapon, int ammo) {\n"
OLD = """    int type = weapon->m_type;
    if (m_weapon) {
        if (type == m_weapon->m_type) {
            if (ammo < 0) {
                ammo = fn_8005B9AC(m_weapon->m_type);
            }
            AddAmmo(ammo);
            return;
        }
        RemoveWeapon();
    }
    SetWeapon(weapon, ammo);
"""
H = """inline bool StackAmmo(ShadowWeapon *self, int type, int ammo) {
    if (self->m_weapon) {
        if (type == self->m_weapon->m_type) {
            if (ammo < 0) {
                ammo = fn_8005B9AC(type);
            }
            self->AddAmmo(ammo);
            return true;
        }
        self->RemoveWeapon();
    }
    return false;
}

"""
H2 = H.replace("int type, int ammo", "int type, int &ammo")
REPL = {
 "inl_m": [(A, H + A), (OLD, "    if (StackAmmo(this, weapon->m_type, ammo)) {\n        return;\n    }\n    SetWeapon(weapon, ammo);\n")],
 "inl_g": [(A, H + A), (OLD, "    if (StackAmmo(this, weapon->TypeId(), ammo)) {\n        return;\n    }\n    SetWeapon(weapon, ammo);\n")],
 "inl_ref_m": [(A, H2 + A), (OLD, "    if (StackAmmo(this, weapon->m_type, ammo)) {\n        return;\n    }\n    SetWeapon(weapon, ammo);\n")],
 "inl_ref_g": [(A, H2 + A), (OLD, "    if (StackAmmo(this, weapon->TypeId(), ammo)) {\n        return;\n    }\n    SetWeapon(weapon, ammo);\n")],
 "inl_not": [(A, H + A), (OLD, "    if (!StackAmmo(this, weapon->m_type, ammo)) {\n        SetWeapon(weapon, ammo);\n    }\n")],
}
SCORE = ["GiveWeapon__Q36Player6Shadow12ShadowWeaponFRCQ25boost33shared_ptr<Q26Weapon10WeaponBase>i"]
