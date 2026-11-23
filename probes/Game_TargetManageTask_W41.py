"""Wave 41: HasFlags/LacksFlags (retail: and r5,r0,r31, the target's flags first)."""
SCORE = ["HasFlags__FPCQ25boost17weak_ptr<6Target>Ul", "LacksFlags__FPCQ25boost17weak_ptr<6Target>Ul"]
H1 = "static bool HasFlags(TargetEntry target, u32 flags) {\n    return (target->lock()->m_flags & flags) != 0;\n}\n"
H2 = "static bool LacksFlags(TargetEntry target, u32 flags) {\n    bool has = (target->lock()->m_flags & flags) != 0;\n    return !has;\n}\n"
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"
REPL = {
  "swap": [(H1, H1.replace("(target->lock()->m_flags & flags)", "(flags & target->lock()->m_flags)")), (H2, H2.replace("(target->lock()->m_flags & flags)", "(flags & target->lock()->m_flags)"))],
  "prag": [(H1, P0 + H1 + P1), (H2, P0 + H2 + P1)],
  "prag_local": [(H1, P0 + "static bool HasFlags(TargetEntry target, u32 flags) {\n    u32 f = flags;\n    return (target->lock()->m_flags & f) != 0;\n}\n" + P1), (H2, P0 + "static bool LacksFlags(TargetEntry target, u32 flags) {\n    u32 f = flags;\n    bool has = (target->lock()->m_flags & f) != 0;\n    return !has;\n}\n" + P1)],
  "prag_masked": [(H1, P0 + "static bool HasFlags(TargetEntry target, u32 flags) {\n    u32 masked = target->lock()->m_flags & flags;\n    return masked != 0;\n}\n" + P1), (H2, P0 + "static bool LacksFlags(TargetEntry target, u32 flags) {\n    u32 masked = target->lock()->m_flags & flags;\n    bool has = masked != 0;\n    return !has;\n}\n" + P1)],
  "prag_word": [(H1, P0 + "static bool HasFlags(TargetEntry target, u32 flags) {\n    u32 word = target->lock()->m_flags;\n    return (word & flags) != 0;\n}\n" + P1), (H2, P0 + "static bool LacksFlags(TargetEntry target, u32 flags) {\n    u32 word = target->lock()->m_flags;\n    bool has = (word & flags) != 0;\n    return !has;\n}\n" + P1)],
}
