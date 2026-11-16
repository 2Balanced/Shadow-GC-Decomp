"""Shapes for the repeat-counter loop of UnkPads_80051734::Update (wave 39, second pass)."""
OLD = """                s16 count = m_repeat[i][j];
                if (m_states[i].m_down & s_repeatMasks[j]) {
                    count++;
                } else {
                    count = 0;
                }
                m_repeat[i][j] = count;
"""
def v(s):
    return [(OLD, s)]
REPL = {
 "ref": v("""                s16 &count = m_repeat[i][j];
                if (m_states[i].m_down & s_repeatMasks[j]) {
                    count++;
                } else {
                    count = 0;
                }
"""),
 "ptr": v("""                s16 *count = &m_repeat[i][j];
                if (m_states[i].m_down & s_repeatMasks[j]) {
                    (*count)++;
                } else {
                    *count = 0;
                }
"""),
 "ptr_tern": v("""                s16 *p = &m_repeat[i][j];
                *p = (m_states[i].m_down & s_repeatMasks[j]) ? (s16)(*p + 1) : 0;
"""),
 "mask_first": v("""                u32 held = m_states[i].m_down & s_repeatMasks[j];
                s16 count = m_repeat[i][j];
                if (held) {
                    count++;
                } else {
                    count = 0;
                }
                m_repeat[i][j] = count;
"""),
 "next_local": v("""                s16 count = m_repeat[i][j];
                s16 next = count + 1;
                if (!(m_states[i].m_down & s_repeatMasks[j])) {
                    next = 0;
                }
                m_repeat[i][j] = next;
"""),
 "direct": v("""                if (m_states[i].m_down & s_repeatMasks[j]) {
                    m_repeat[i][j]++;
                } else {
                    m_repeat[i][j] = 0;
                }
"""),
 "tern_direct": v("""                m_repeat[i][j] = (m_states[i].m_down & s_repeatMasks[j]) ? m_repeat[i][j] + 1 : 0;
"""),
 "inv": v("""                s16 count = m_repeat[i][j];
                if (!(m_states[i].m_down & s_repeatMasks[j])) {
                    count = 0;
                } else {
                    count++;
                }
                m_repeat[i][j] = count;
"""),
 "pre_inc": v("""                s16 count = m_repeat[i][j] + 1;
                if (!(m_states[i].m_down & s_repeatMasks[j])) {
                    count = 0;
                }
                m_repeat[i][j] = count;
"""),
 "row_outer": [("            for (int j = 0; j < 8; j++) {\n"+OLD, """            s16 *row = m_repeat[i];
            for (int j = 0; j < 8; j++) {
                s16 count = row[j];
                if (m_states[i].m_down & s_repeatMasks[j]) {
                    count++;
                } else {
                    count = 0;
                }
                row[j] = count;
""")],
 "mask_local": v("""                u32 mask = s_repeatMasks[j];
                s16 count = m_repeat[i][j];
                if (m_states[i].m_down & mask) {
                    count++;
                } else {
                    count = 0;
                }
                m_repeat[i][j] = count;
"""),
 "int_tern": v("""                int count = m_repeat[i][j];
                m_repeat[i][j] = (m_states[i].m_down & s_repeatMasks[j]) ? count + 1 : 0;
"""),
}
SCORE = ["Update__16UnkPads_80051734Fv"]
