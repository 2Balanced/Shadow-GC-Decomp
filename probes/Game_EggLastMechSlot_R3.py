"""Wave 40: fn_80313CC0 with the first loop in an inline helper (R2 l1_helper, 6 rows): case 3's
result lookup (retail: shot type r0, symbol r4)."""
SCORE = ["fn_80313CC0"]
L1 = """        for (int i = 0; i < 2; i++) {
            u32 mask = slot->m_symbolMask & slot->m_lineMasks[i];
            if (mask == 0) {
                mask = slot->m_lineMasks[i];
            }
            slot->m_nextSymbols[i] = PickSymbol(mask);
        }
"""
F = 'extern "C" void fn_80313CC0(UnkSlot_803142C0 *slot) {'
H1 = """inline void PickNext_(UnkSlot_803142C0 *slot) {
    for (int i = 0; i < 2; i++) {
        u32 mask = slot->m_symbolMask & slot->m_lineMasks[i];
        if (mask == 0) {
            mask = slot->m_lineMasks[i];
        }
        s32 symbols[5];
        int count = 0;
        for (int k = 0; k < 5; k++) {
            if (mask & (1 << k)) {
                symbols[count++] = k;
            }
        }
        slot->m_nextSymbols[i] = symbols[fn_8040EA90(count)];
    }
}
inline const s32 *ResultsOf_(int shotType) { return s_results[shotType]; }
inline s32 ResultOf2_(int symbol, int shotType) { return s_results[shotType][symbol]; }
inline s32 Lookup_(const s32 *results, int symbol) { return results[symbol]; }

"""
BASE = [(F, H1 + F), (L1, "        PickNext_(slot);\n")]
C3 = """                    const s32 *results = s_results[slot->m_line.m_shotType];
                    slot->m_result = results[slot->m_line.m_symbols[0]];
"""
def v(new): return BASE + [(C3, new)]
REPL = {
  "base": BASE,
  "c3_2d": v("                    slot->m_result = s_results[slot->m_line.m_shotType][slot->m_line.m_symbols[0]];\n"),
  "c3_inl": v("                    slot->m_result = GetResult(slot->m_line.m_shotType, slot->m_line.m_symbols[0]);\n"),
  "c3_inl2": v("                    slot->m_result = ResultOf2_(slot->m_line.m_symbols[0], slot->m_line.m_shotType);\n"),
  "c3_resof": v("                    slot->m_result = ResultsOf_(slot->m_line.m_shotType)[slot->m_line.m_symbols[0]];\n"),
  "c3_lookup": v("                    slot->m_result = Lookup_(s_results[slot->m_line.m_shotType], slot->m_line.m_symbols[0]);\n"),
  "c3_symfirst": v("                    s32 symbol = slot->m_line.m_symbols[0];\n                    const s32 *results = s_results[slot->m_line.m_shotType];\n                    slot->m_result = results[symbol];\n"),
  "c3_two": v("                    s32 shotType = slot->m_line.m_shotType;\n                    s32 symbol = slot->m_line.m_symbols[0];\n                    slot->m_result = s_results[shotType][symbol];\n"),
  "c3_two_rev": v("                    s32 symbol = slot->m_line.m_symbols[0];\n                    s32 shotType = slot->m_line.m_shotType;\n                    slot->m_result = s_results[shotType][symbol];\n"),
  "c3_res_decl_first": v("                    const s32 *results;\n                    s32 symbol = slot->m_line.m_symbols[0];\n                    results = s_results[slot->m_line.m_shotType];\n                    slot->m_result = results[symbol];\n"),
}
