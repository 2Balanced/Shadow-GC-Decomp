"""Round 4 for fn_801E7094 of Game/Target.cpp (wave 39): the type of the saved bit."""
SYMBOLS = {"fn_801E7094": "extern \"C\" bool fn_801E7094(const boost::weak_ptr<Target> *target) {"}
H = "    if (target->expired()) {\n        return false;\n    }\n"
def v(t, test="active == 0", cast=""):
    return H + f"    {t} active = {cast}target->lock()->IsActive();\n    if ({test}) {{\n        return false;\n    }}\n    return true;\n"
BODIES = {"fn_801E7094": {
    "u8": v("u8"), "u16": v("u16"), "s8": v("s8"), "bool_eq_false": v("bool", "active == false"),
    "bool_not": v("bool", "!active"), "ulong": v("unsigned long"), "ptr": v("void *", "active == 0", "(void *)"),
    "ptr_not": v("void *", "!active", "(void *)"), "size_t": v("size_t"),
    "u32_not": v("u32", "!active"), "u32_lt1": v("u32", "active < 1"),
    "bool_ptr": H + "    bool active = target->lock()->IsActive();\n    bool *p = &active;\n    if (*p == 0) {\n        return false;\n    }\n    return true;\n",
}}
