"""Source shapes for fn_801E7094 of Game/Target.cpp (wave 39): retail tests the active bit with a
branch after the locked pointer is released; GC/2.7 made it branchless (addic/subfe)."""
SYMBOLS = {"fn_801E7094": "extern \"C\" bool fn_801E7094(const boost::weak_ptr<Target> *target) {"}
XTRA = {"fn_801E7094": """inline u32 ActiveBit_(Target *t) { return t->m_flags & 1; }
inline bool ActiveBool_(Target *t) { return t->m_flags & 1; }
inline bool ActiveBranch_(Target *t) { if (t->m_flags & 1) { return true; } return false; }
"""}
H = "    if (target->expired()) {\n        return false;\n    }\n"
BODIES = {"fn_801E7094": {
    "if_true": H + "    if (target->lock()->IsActive()) {\n        return true;\n    }\n    return false;\n",
    "if_not": H + "    if (!target->lock()->IsActive()) {\n        return false;\n    }\n    return true;\n",
    "ternary": H + "    return target->lock()->IsActive() ? true : false;\n",
    "ret_ne": H + "    return target->lock()->IsActive() != 0;\n",
    "ret_direct": H + "    return target->lock()->IsActive();\n",
    "bit_if": H + "    if (ActiveBit_(target->lock().get())) {\n        return true;\n    }\n    return false;\n",
    "bool_if": H + "    if (ActiveBool_(target->lock().get())) {\n        return true;\n    }\n    return false;\n",
    "branch_fn": H + "    return ActiveBranch_(target->lock().get());\n",
    "bool_local": H + "    bool active = target->lock()->IsActive();\n    if (active) {\n        return true;\n    }\n    return false;\n",
    "scope_if": H + "    {\n        boost::shared_ptr<Target> t = target->lock();\n        if (t->IsActive()) {\n            return true;\n        }\n    }\n    return false;\n",
    "local_ret": H + "    boost::shared_ptr<Target> t = target->lock();\n    if (t->IsActive()) {\n        return true;\n    }\n    return false;\n",
    "flags_and": H + "    if (target->lock()->m_flags & 1) {\n        return true;\n    }\n    return false;\n",
    "else_form": H + "    if (target->lock()->IsActive()) {\n        return true;\n    } else {\n        return false;\n    }\n",
}}
