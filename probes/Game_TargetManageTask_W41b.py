"""Wave 41: HasFlags/LacksFlags: the and's operand order (retail: and r5,r0,r31)."""
SCORE = ["HasFlags__FPCQ25boost17weak_ptr<6Target>Ul"]
H1 = "static bool HasFlags(TargetEntry target, u32 flags) {\n    return (target->lock()->m_flags & flags) != 0;\n}\n"
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"
def v(body, prag=False):
    f = "static bool HasFlags(TargetEntry target, u32 flags) {\n" + body + "}\n"
    return [(H1, (P0 + f + P1) if prag else f)]
REPL = {
  "named": v("    boost::shared_ptr<Target> locked(target->lock());\n    return (locked->m_flags & flags) != 0;\n"),
  "named_prag": v("    boost::shared_ptr<Target> locked(target->lock());\n    return (locked->m_flags & flags) != 0;\n", True),
  "named_late_copy_prag": v("    boost::shared_ptr<Target> locked(target->lock());\n    u32 word = locked->m_flags;\n    u32 f = flags;\n    return (word & f) != 0;\n", True),
  "ternary": v("    return (target->lock()->m_flags & flags) ? true : false;\n"),
  "bool_cast": v("    return (bool)(target->lock()->m_flags & flags);\n"),
  "notnot": v("    return !!(target->lock()->m_flags & flags);\n"),
  "flags_first_eq": v("    return 0 != (target->lock()->m_flags & flags);\n"),
}
