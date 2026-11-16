"""Round 2 for Weapon::WeaponStickBase::fn_80067918 (wave 39): the test inside an inline member of
the swing, so that the flags' address is formed at the call."""
SCORE = ["fn_80067918__Q26Weapon15WeaponStickBaseFv"]
A = """    int HitCount() {
        return m_hits;
    }
"""
OLD = "    return m_swing->m_flags.Test(0);"
REPL = {
  "impl_direct": [(A, A + "    bool IsHeld() {\n        return fn_80014E34(&m_flags.m_bits, 0);\n    }\n"), (OLD, "    return m_swing->IsHeld();")],
  "impl_test": [(A, A + "    bool IsHeld() {\n        return m_flags.Test(0);\n    }\n"), (OLD, "    return m_swing->IsHeld();")],
}
