"""Impl::Update of Game/EffBelt.cpp (wave 39, second pass): retail has alpha r30, count r29, i r28."""
OLD = """    s32 count = m_ring.m_count;
    if (count == 0) {
        return true;
    }
    BeltPoint *point = m_ring.Get(0);
    point->m_alpha = 0.0f;
    int i;
    for (i = 1; i < count; i++) {
        point = m_ring.Get(i);
        point->m_alpha = (int)alpha;
    }
    int fade = 0;
    for (i = count - 1; i >= count - 9; i--) {
        point = m_ring.Get(i);
        point->m_alpha = fade;
        fade += 0x20;
        if (fade > m_color.m_a) {
            break;
        }
    }
"""
GET = ("    /* 0x0 */ boost::scoped_array<BeltPoint> m_points;\n", "    int Count() const {\n        return m_count;\n    }\n    /* 0x0 */ boost::scoped_array<BeltPoint> m_points;\n")
def mk(decl, l1="for (i = 1;", l2="for (i = count - 1;", fade="    int fade = 0;\n", cnt="m_ring.m_count"):
    s = OLD.replace("    int i;\n", "").replace("    s32 count = m_ring.m_count;\n", decl.replace("CNT", cnt)).replace("for (i = 1;", l1).replace("for (i = count - 1;", l2).replace("    int fade = 0;\n", fade)
    return [(OLD, s), GET]
REPL = {
 "two_i": mk("    s32 count = CNT;\n", "for (int i = 1;", "for (int i = count - 1;"),
 "i_before": mk("    int i;\n    s32 count = CNT;\n"),
 "i_after": mk("    s32 count = CNT;\n    int i;\n"),
 "g_two_i": mk("    s32 count = CNT;\n", "for (int i = 1;", "for (int i = count - 1;", cnt="m_ring.Count()"),
 "g_i_before": mk("    int i;\n    s32 count = CNT;\n", cnt="m_ring.Count()"),
 "g_i_after": mk("    s32 count = CNT;\n    int i;\n", cnt="m_ring.Count()"),
 "second_j": mk("    s32 count = CNT;\n    int i;\n", l2="for (int j = count - 1;"),
 "fade_first": mk("    int fade;\n    s32 count = CNT;\n    int i;\n", fade="    fade = 0;\n"),
 "ifc": mk("    int i;\n    int fade;\n    s32 count = CNT;\n", fade="    fade = 0;\n"),
 "cfi": mk("    s32 count = CNT;\n    int fade;\n    int i;\n", fade="    fade = 0;\n"),
}
REPL["second_j"][0] = (OLD, REPL["second_j"][0][1].replace("for (int j = count - 1; i >= count - 9; i--) {\n        point = m_ring.Get(i);", "for (int j = count - 1; j >= count - 9; j--) {\n        point = m_ring.Get(j);"))
SCORE = ["Update__Q34Test7EffBelt4ImplFv"]
