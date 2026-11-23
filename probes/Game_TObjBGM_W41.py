"""Wave 41: fn_801D78C4, retail keeps the sign-extended priority for both CanInterrupt calls."""
SCORE = ["fn_801D78C4"]
F = "extern \"C\" int fn_801D78C4(int owner, s8 priority) {\n"
N = "/* Plays voice for owner with priority; returns the player used, or -1 (name inferred). */\n"
PRAG = [(F, "#pragma opt_propagation off\n" + F), (N, "#pragma opt_propagation reset\n" + N)]
CI = """inline bool CanInterrupt(s8 current, s8 incoming) {
    if (current == 3) {
        return false;
    }
    if (current <= 0) {
        return true;
    }
    return current < incoming;
}
"""
L2A = "            if (CanInterrupt(serif->m_priority, priority) == true) {\n"
L2B = "                } else if (CanInterrupt(serif->m_priority, s_serif[found]->m_priority) == true) {\n"
L1 = "                if (CanInterrupt(serif->m_priority, priority)) {\n"


def v(*extra, prag=True):
    return list(extra) + (PRAG if prag else [])


CI_COPY = CI.replace("    if (current == 3) {", "    s8 c = current;\n    if (c == 3) {").replace("    if (current <= 0) {", "    if (c <= 0) {").replace("    return current < incoming;", "    return c < incoming;")
SV = "    /* Stops the voice (name inferred). */\n"
MEMB = (SV, "    bool CanBeInterrupted(s8 incoming) {\n        if (m_priority == 3) {\n            return false;\n        }\n        if (m_priority <= 0) {\n            return true;\n        }\n        return m_priority < incoming;\n    }\n\n" + SV)
REPL = {
    "prag": v(),
    "prag_cicopy": v((CI, CI_COPY)),
    "cicopy": v((CI, CI_COPY), prag=False),
    "prag_cur": v((L2A, "            s8 cur = serif->m_priority;\n            if (CanInterrupt(cur, priority) == true) {\n"),
                  (L2B, "                } else if (CanInterrupt(cur, s_serif[found]->m_priority) == true) {\n"),
                  (L1, "                s8 cur = serif->m_priority;\n                if (CanInterrupt(cur, priority)) {\n")),
    "prag_int_cur": v((L2A, "            int cur = serif->m_priority;\n            if (CanInterrupt(cur, priority) == true) {\n"),
                      (L2B, "                } else if (CanInterrupt(cur, s_serif[found]->m_priority) == true) {\n"),
                      (L1, "                int cur = serif->m_priority;\n                if (CanInterrupt(cur, priority)) {\n")),
    "member": v(MEMB, (L2A, "            if (serif->CanBeInterrupted(priority) == true) {\n"),
                (L2B, "                } else if (serif->CanBeInterrupted(s_serif[found]->m_priority) == true) {\n"),
                (L1, "                if (serif->CanBeInterrupted(priority)) {\n"), prag=False),
    "prag_member": v(MEMB, (L2A, "            if (serif->CanBeInterrupted(priority) == true) {\n"),
                     (L2B, "                } else if (serif->CanBeInterrupted(s_serif[found]->m_priority) == true) {\n"),
                     (L1, "                if (serif->CanBeInterrupted(priority)) {\n")),
    "ci_int_prag": v((CI, CI.replace("inline bool CanInterrupt(s8 current, s8 incoming) {", "inline bool CanInterrupt(int current, int incoming) {"))),
}
