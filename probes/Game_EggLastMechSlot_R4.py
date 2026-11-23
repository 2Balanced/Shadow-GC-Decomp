"""Wave 40: fn_80313CC0 case 3 (shot type r0 / symbol r4 in retail): IsMatch's spellings (it is
inlined just before the lookup; fn_80314004 inlines it too)."""
SCORE = ["fn_80313CC0", "fn_80314004"]
M = """    s32 first = m_symbols[0];
    s32 second = m_symbols[1];
    if (first == second && second == m_symbols[2] && first != -1) {
"""
REPL = {
  "swap_decl": [(M, """    s32 second = m_symbols[1];
    s32 first = m_symbols[0];
    if (first == second && second == m_symbols[2] && first != -1) {
""")],
  "no_locals": [(M, """    if (m_symbols[0] == m_symbols[1] && m_symbols[1] == m_symbols[2] && m_symbols[0] != -1) {
""")],
  "first_only": [(M, """    s32 first = m_symbols[0];
    if (first == m_symbols[1] && m_symbols[1] == m_symbols[2] && first != -1) {
""")],
}
