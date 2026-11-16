"""Impl::Update of Game/EffBelt.cpp, loop forms (wave 39, second pass)."""
L1 = """    int i;
    for (i = 1; i < count; i++) {
        point = m_ring.Get(i);
        point->m_alpha = (int)alpha;
    }
"""
L2 = """    int fade = 0;
    for (i = count - 1; i >= count - 9; i--) {
        point = m_ring.Get(i);
        point->m_alpha = fade;
        fade += 0x20;
        if (fade > m_color.m_a) {
            break;
        }
    }
"""
CNT = "    s32 count = m_ring.m_count;\n"
REPL = {
 "const_count": [(CNT, "    const s32 count = m_ring.m_count;\n")],
 "int_count": [(CNT, "    int count = m_ring.m_count;\n")],
 "end_local": [(L2, L2.replace("    int fade = 0;\n    for (i = count - 1; i >= count - 9; i--) {", "    int fade = 0;\n    int end = count - 9;\n    for (i = count - 1; i >= end; i--) {"))],
 "fade_in_for": [(L2, L2.replace("    int fade = 0;\n    for (i = count - 1;", "    int fade;\n    for (i = count - 1, fade = 0;"))],
 "fade_in_for2": [(L2, L2.replace("    int fade = 0;\n    for (i = count - 1;", "    int fade;\n    for (fade = 0, i = count - 1;"))],
 "nopoint1": [(L1, "    int i;\n    for (i = 1; i < count; i++) {\n        m_ring.Get(i)->m_alpha = (int)alpha;\n    }\n")],
 "nopoint": [(L1, "    int i;\n    for (i = 1; i < count; i++) {\n        m_ring.Get(i)->m_alpha = (int)alpha;\n    }\n"), (L2, L2.replace("        point = m_ring.Get(i);\n        point->m_alpha = fade;", "        m_ring.Get(i)->m_alpha = fade;")), ("    BeltPoint *point = m_ring.Get(0);\n    point->m_alpha = 0.0f;\n", "    m_ring.Get(0)->m_alpha = 0.0f;\n")],
 "points_scoped": [(L1, "    int i;\n    for (i = 1; i < count; i++) {\n        BeltPoint *p = m_ring.Get(i);\n        p->m_alpha = (int)alpha;\n    }\n"), (L2, L2.replace("        point = m_ring.Get(i);\n        point->m_alpha = fade;", "        BeltPoint *p = m_ring.Get(i);\n        p->m_alpha = fade;"))],
 "last_local": [(L2, L2.replace("    for (i = count - 1;", "    i = count - 1;\n    for (;"))],
 "while2": [(L2, "    int fade = 0;\n    i = count - 1;\n    while (i >= count - 9) {\n        point = m_ring.Get(i);\n        point->m_alpha = fade;\n        fade += 0x20;\n        if (fade > m_color.m_a) {\n            break;\n        }\n        i--;\n    }\n")],
 "count_m1": [(L2, L2.replace("for (i = count - 1; i >= count - 9; i--)", "for (i = count - 1; i > count - 10; i--)"))],
 "i_short": [(L1, L1.replace("    int i;\n", "    s32 i;\n"))],
}
SCORE = ["Update__Q34Test7EffBelt4ImplFv"]
