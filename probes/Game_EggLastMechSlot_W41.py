"""Wave 41: fn_80313CC0 case 3's result lookup (retail: shot type r0, symbol r4)."""
F = "fn_80313CC0"
SCORE = [F]
C3 = """                    const s32 *results = s_results[slot->m_line.m_shotType];
                    slot->m_result = results[slot->m_line.m_symbols[0]];
"""
FN = 'extern "C" void fn_80313CC0(UnkSlot_803142C0 *slot) {\n'
NEXTF = "/* Stops reel on the next symbol of shot type 0 (name inferred). */\n"
OFF = [(FN, "#pragma opt_propagation off\n" + FN), (NEXTF, "#pragma opt_propagation reset\n" + NEXTF)]
HELP = """inline s32 LineResult_(const UnkSlotLine_803141C4 &line) {
    return s_results[line.m_shotType][line.m_symbols[0]];
}
inline s32 LineResultP_(const UnkSlotLine_803141C4 *line) {
    const s32 *results = s_results[line->m_shotType];
    return results[line->m_symbols[0]];
}
inline const s32 *ResultsOf2_(const UnkSlotLine_803141C4 &line) {
    return s_results[line.m_shotType];
}

"""
def v(new, extra=None):
    r = [(FN, HELP + FN), (C3, new)]
    return r + (extra or [])
REPL = {
    "line_ref": v("                    slot->m_result = LineResult_(slot->m_line);\n"),
    "line_ptr": v("                    slot->m_result = LineResultP_(&slot->m_line);\n"),
    "line_local": v("                    UnkSlotLine_803141C4 *line = &slot->m_line;\n                    slot->m_result = s_results[line->m_shotType][line->m_symbols[0]];\n"),
    "resof2": v("                    slot->m_result = ResultsOf2_(slot->m_line)[slot->m_line.m_symbols[0]];\n"),
    "off_two": v("                    s32 shotType = slot->m_line.m_shotType;\n                    s32 symbol = slot->m_line.m_symbols[0];\n                    slot->m_result = s_results[shotType][symbol];\n", OFF),
    "off_two_rev": v("                    s32 symbol = slot->m_line.m_symbols[0];\n                    s32 shotType = slot->m_line.m_shotType;\n                    slot->m_result = s_results[shotType][symbol];\n", OFF),
    "off_base": v(C3, OFF),
    "uidx": v("                    slot->m_result = s_results[slot->m_line.m_shotType][(u32)slot->m_line.m_symbols[0]];\n"),
}
