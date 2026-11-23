"""Wave 40: fn_80313CC0 (EggLastMechSlot), first probes. Case 3: retail loads the shot type into r0
and the symbol into r4 (GC/2.7 the other way round). The two loops: retail gives the counter r28
and the entry address r27 (GC/2.7 the other way round)."""
SCORE = ["fn_80313CC0"]
C3 = """                    const s32 *results = s_results[slot->m_line.m_shotType];
                    slot->m_result = results[slot->m_line.m_symbols[0]];
"""
L2 = """    for (int i = 0; i < 3; i++) {
        slot->m_reels[i]->Tick();
        fn_80313C60(slot, i, slot->m_reels[i]->m_angle);
    }
"""
L1 = """        for (int i = 0; i < 2; i++) {
            u32 mask = slot->m_symbolMask & slot->m_lineMasks[i];
            if (mask == 0) {
                mask = slot->m_lineMasks[i];
            }
            slot->m_nextSymbols[i] = PickSymbol(mask);
        }
"""
REPL = {
  "c3_2d": [(C3, "                    slot->m_result = s_results[slot->m_line.m_shotType][slot->m_line.m_symbols[0]];\n")],
  "c3_inl": [(C3, "                    slot->m_result = GetResult(slot->m_line.m_shotType, slot->m_line.m_symbols[0]);\n")],
  "c3_symfirst": [(C3, "                    s32 symbol = slot->m_line.m_symbols[0];\n                    const s32 *results = s_results[slot->m_line.m_shotType];\n                    slot->m_result = results[symbol];\n")],
  "c3_two": [(C3, "                    s32 shotType = slot->m_line.m_shotType;\n                    s32 symbol = slot->m_line.m_symbols[0];\n                    slot->m_result = s_results[shotType][symbol];\n")],
  "l2_i_outside": [(L2, """    int i;
    for (i = 0; i < 3; i++) {
        slot->m_reels[i]->Tick();
        fn_80313C60(slot, i, slot->m_reels[i]->m_angle);
    }
""")],
  "l2_reel_local": [(L2, """    for (int i = 0; i < 3; i++) {
        SlotSwitch *reel = slot->m_reels[i];
        reel->Tick();
        fn_80313C60(slot, i, slot->m_reels[i]->m_angle);
    }
""")],
  "l1_i_outside": [(L1, """        int i;
        for (i = 0; i < 2; i++) {
            u32 mask = slot->m_symbolMask & slot->m_lineMasks[i];
            if (mask == 0) {
                mask = slot->m_lineMasks[i];
            }
            slot->m_nextSymbols[i] = PickSymbol(mask);
        }
""")],
}
