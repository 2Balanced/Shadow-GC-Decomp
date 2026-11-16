"""Round 2 for fn_801E7094 of Game/Target.cpp (wave 39): an int result (as CTexListRW::Load, the
one matched function with the same branches, has)."""
SCORE = ["fn_801E7094"]
OLD = '''extern "C" bool fn_801E7094(const boost::weak_ptr<Target> *target) {
    if (target->expired()) {
        return false;
    }
    /* Retail tests the flag with a branch; GC/2.7 turns the same source into a branchless
     * conversion (addic/subfe). Every form tried gives the latter. */
    u32 active = target->lock()->IsActive();
    if (active == 0) {
        return false;
    }
    return true;
}'''
H = 'extern "C" int fn_801E7094(const boost::weak_ptr<Target> *target) {\n    if (target->expired()) {\n        return false;\n    }\n'
REPL = {
  "int_local": [(OLD, H + "    u32 active = target->lock()->IsActive();\n    if (active == 0) {\n        return false;\n    }\n    return true;\n}")],
  "int_if_not": [(OLD, H + "    if (!target->lock()->IsActive()) {\n        return false;\n    }\n    return true;\n}")],
  "int_if": [(OLD, H + "    if (target->lock()->IsActive()) {\n        return true;\n    }\n    return false;\n}")],
}
