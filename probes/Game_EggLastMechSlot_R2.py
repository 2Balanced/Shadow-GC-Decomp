"""Wave 40: fn_80313CC0's loops in inline helpers (retail: counter r28, address r27)."""
SCORE = ["fn_80313CC0"]
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
F = 'extern "C" void fn_80313CC0(UnkSlot_803142C0 *slot) {'
H2 = "inline void TickReels_(UnkSlot_803142C0 *slot) {\n" + L2 + "}\n\n"
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

"""
REPL = {
  "l2_helper": [(F, H2 + F), (L2, "    TickReels_(slot);\n")],
  "l1_helper": [(F, H1 + F), (L1, "        PickNext_(slot);\n")],
  "both": [(F, H1 + H2 + F), (L2, "    TickReels_(slot);\n"), (L1, "        PickNext_(slot);\n")],
}
