"""Source shapes for Weapon::WeaponStickBase::fn_80067918 (wave 39): retail loads the bit (li r4,0)
before it forms the flags' address (addi r3,r3,60); GC/2.7 had the two the other way round."""
SCORE = ["fn_80067918__Q26Weapon15WeaponStickBaseFv"]
A = "namespace Weapon {\n\n/* The swing. Flag bit 0: a swing is held. */"
HELP = """struct ProbeBase1_ { bool test(unsigned long pos) const; u32 data_; };
struct ProbeBitsA_ : private ProbeBase1_ { bool test(unsigned long pos) const { return ProbeBase1_::test(pos); } };
struct ProbeBitsB_ { bool test(unsigned long pos) const { return base_.test(pos); } ProbeBase1_ base_; };
struct ProbeBitsC_ { bool test(unsigned long pos) const; u32 data_; };
"""
OLD = "    return m_swing->m_flags.Test(0);"
REPL = {
  "base_inline": [(A, HELP + A), (OLD, "    return reinterpret_cast<ProbeBitsA_ &>(m_swing->m_flags).test(0);")],
  "member_inline": [(A, HELP + A), (OLD, "    return reinterpret_cast<ProbeBitsB_ &>(m_swing->m_flags).test(0);")],
  "direct_member": [(A, HELP + A), (OLD, "    return reinterpret_cast<ProbeBitsC_ &>(m_swing->m_flags).test(0);")],
  "direct_c": [(OLD, "    return fn_80014E34(&m_swing->m_flags.m_bits, 0);")],
}
