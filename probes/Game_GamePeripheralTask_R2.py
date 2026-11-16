"""More shapes for the repeat-counter loop of UnkPads_80051734::Update (wave 39, second pass):
inline helpers, index types."""
OLD = """                s16 count = m_repeat[i][j];
                if (m_states[i].m_down & s_repeatMasks[j]) {
                    count++;
                } else {
                    count = 0;
                }
                m_repeat[i][j] = count;
"""
FOR = "            for (int j = 0; j < 8; j++) {\n"
ANCH = "void UnkPads_80051734::Update() {\n"
def v(s):
    return [(OLD, s)]
REPL = {
 "inl_next": [(ANCH, "inline s16 NextCount(s16 count, u32 held) {\n    if (held) {\n        count++;\n    } else {\n        count = 0;\n    }\n    return count;\n}\n\n"+ANCH),
              (OLD, "                m_repeat[i][j] = NextCount(m_repeat[i][j], m_states[i].m_down & s_repeatMasks[j]);\n")],
 "inl_next_b": [(ANCH, "inline s16 NextCount(s16 count, bool held) {\n    return held ? (s16)(count + 1) : 0;\n}\n\n"+ANCH),
              (OLD, "                m_repeat[i][j] = NextCount(m_repeat[i][j], (m_states[i].m_down & s_repeatMasks[j]) != 0);\n")],
 "inl_ref": [(ANCH, "inline void Count(s16 &count, u32 held) {\n    s16 c = count;\n    if (held) {\n        c++;\n    } else {\n        c = 0;\n    }\n    count = c;\n}\n\n"+ANCH),
              (OLD, "                Count(m_repeat[i][j], m_states[i].m_down & s_repeatMasks[j]);\n")],
 "uj": [(FOR, "            for (u32 j = 0; j < 8; j++) {\n")],
 "held_bool": v("""                bool held = (m_states[i].m_down & s_repeatMasks[j]) != 0;
                s16 count = m_repeat[i][j];
                if (held) {
                    count++;
                } else {
                    count = 0;
                }
                m_repeat[i][j] = count;
"""),
 "count_outer": [(FOR+OLD, """            s16 count;
            for (int j = 0; j < 8; j++) {
                count = m_repeat[i][j];
                if (m_states[i].m_down & s_repeatMasks[j]) {
                    count++;
                } else {
                    count = 0;
                }
                m_repeat[i][j] = count;
""")],
 "row_ref_inner": v("""                s16 *row = m_repeat[i];
                s16 count = row[j];
                if (m_states[i].m_down & s_repeatMasks[j]) {
                    count++;
                } else {
                    count = 0;
                }
                row[j] = count;
"""),
 "mul": v("""                s16 count = m_repeat[i][j];
                m_repeat[i][j] = (s16)(count + 1) & -((m_states[i].m_down & s_repeatMasks[j]) != 0);
"""),
 "down_local": v("""                u32 down2 = m_states[i].m_down;
                s16 count = m_repeat[i][j];
                if (down2 & s_repeatMasks[j]) {
                    count++;
                } else {
                    count = 0;
                }
                m_repeat[i][j] = count;
"""),
 "masks_first": v("""                s16 count = m_repeat[i][j];
                if (s_repeatMasks[j] & m_states[i].m_down) {
                    count++;
                } else {
                    count = 0;
                }
                m_repeat[i][j] = count;
"""),
 "pp": v("""                s16 count = m_repeat[i][j];
                if (m_states[i].m_down & s_repeatMasks[j]) {
                    ++count;
                } else {
                    count = 0;
                }
                m_repeat[i][j] = count;
"""),
 "tern_s16": v("""                s16 count = m_repeat[i][j];
                count = (m_states[i].m_down & s_repeatMasks[j]) ? (s16)(count + 1) : (s16)0;
                m_repeat[i][j] = count;
"""),
}
SCORE = ["Update__16UnkPads_80051734Fv"]
