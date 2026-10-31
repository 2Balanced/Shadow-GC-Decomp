"""Source shapes for Game/PadButtons.cpp (fn_80356190, fn_80356260).

Both units' remaining diffs are register-permutation only: the instruction sequence, the callee-
saved set (stmw r27,12(r1) / stmw r26,8(r1)) and every immediate are identical, only which slot
each value gets differs.

    fn_80356190 (33 insns, 8 rows; 5 with `u32 mask = ...` inside the loop)
        retail  i r27, mask r28, pad r29,      button*12 r30, byte offset r31
        ours    pad r27, mask r28, i r29,      button*12 r30, byte offset r31
    fn_80356260 (88 insns, 18 rows)
        retail  i/j r27, button*12 r28, pad*40 r29, mask r30, byte offset r31
        ours    mask r27, i/j r28, button*12 r29, pad*40 r30, byte offset r31

The best guide is fn_803560C8, which matches retail exactly (i r27, mask r28, pad*40 r29,
button*12 r30, offset r31): there the pad parameter never needs a register of its own because it
is consumed by `mulli r29,r4,40` in the prologue.  In fn_80356190 the parameter itself has to stay
live to become r4 at the call, and MWCC then hands it the first saved slot, pushing the loop
counter up.  So the axes worth probing are how pad reaches the call, and where the mask and the
counter are declared.
"""

SYMBOLS = {
    "fn_80356190": 'extern "C" bool fn_80356190(int button, int pad) {',
    "fn_80356260": 'extern "C" bool fn_80356260(int button, int pad) {',
}

_190_CALL = """        if (mask != 0 && fn_80356214(fn_80051734(), pad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
"""

B190 = {
    # committed form: mask declared at function scope
    "committed": """    u32 mask;
    for (int i = 0; i < 3; i++) {
        mask = s_buttons[button][i];
""" + _190_CALL,
    # best so far (5 rows): mask with initialiser inside the loop, mask lands in r28 like retail
    "mask_init": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
""" + _190_CALL,
    # counter as a function-scope local
    "counter_out": """    int i = 0;
    for (; i < 3; i++) {
        u32 mask = s_buttons[button][i];
""" + _190_CALL,
    # pad copied to a local before the loop
    "pad_out": """    int whichPad = pad;
    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && fn_80356214(fn_80051734(), whichPad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
""",
    # pad copied to a local declared before the loop, assigned inside it
    "pad_decl_out_assign_in": """    int whichPad;
    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        whichPad = pad;
        if (mask != 0 && fn_80356214(fn_80051734(), whichPad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
""",
    # pad copied inside the loop after the mask load
    "pad_late": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        int whichPad = pad;
        if (mask != 0 && fn_80356214(fn_80051734(), whichPad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
""",
    # unsigned parameter
    "pad_u32": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && fn_80356214(fn_80051734(), (u32)pad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
""",
    # explicit cast on the argument
    "pad_cast": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && fn_80356214(fn_80051734(), (int)pad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
""",
    # register declaration on the pad copy
    "pad_register": """    register int whichPad = pad;
    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && fn_80356214(fn_80051734(), whichPad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
""",
    # pad handed over through an index expression (keeps r4 live differently)
    "pad_scaled": """    int pads = pad;
    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && fn_80356214(fn_80051734(), pads + 0, mask, 12, 1)) {
            return true;
        }
    }
    return false;
""",
    # the pad-manager result named before the call
    "input_local": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        UnkPadInput_80051734 *input = fn_80051734();
        if (mask != 0 && fn_80356214(input, pad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
""",
    # early continue
    "cont": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask == 0) {
            continue;
        }
        if (fn_80356214(fn_80051734(), pad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
""",
    # nested if
    "nested": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0) {
            if (fn_80356214(fn_80051734(), pad, mask, 12, 1)) {
                return true;
            }
        }
    }
    return false;
""",
    # while form
    "while_loop": """    int i = 0;
    while (i < 3) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && fn_80356214(fn_80051734(), pad, mask, 12, 1)) {
            return true;
        }
        i++;
    }
    return false;
""",
    # row pointer instead of the double index
    "row_ptr": """    u32 *row = s_buttons[button];
    for (int i = 0; i < 3; i++) {
        u32 mask = row[i];
""" + _190_CALL,
    # call result named
    "held_local": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        bool held = fn_80356214(fn_80051734(), pad, mask, 12, 1);
        if (mask != 0 && held) {
            return true;
        }
    }
    return false;
""",
}

_260_TAIL = """    }
    return false;
"""

_260_BLOCK = """            if (button == 1) {
                for (u32 j = 0; j < 2; j++) {
                    u32 other = s_buttons[button][i];
                    if (Matches(other, fn_80051734()->m_pads[j].m_pressed)) {
                        s_pad = j;
                    }
                }
            }
            return true;
        }
"""

B260 = {
    "committed": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
""" + _260_BLOCK + _260_TAIL,
    "mask_outer": """    u32 mask;
    for (int i = 0; i < 3; i++) {
        mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
""" + _260_BLOCK + _260_TAIL,
    # the inner read of the table is the same value, spelled with the outer name
    "same_mask": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
            if (button == 1) {
                for (u32 j = 0; j < 2; j++) {
                    if (Matches(mask, fn_80051734()->m_pads[j].m_pressed)) {
                        s_pad = j;
                    }
                }
            }
            return true;
        }
""" + _260_TAIL,
    # inner counter signed
    "inner_int": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
            if (button == 1) {
                for (int j = 0; j < 2; j++) {
                    u32 other = s_buttons[button][i];
                    if (Matches(other, fn_80051734()->m_pads[j].m_pressed)) {
                        s_pad = j;
                    }
                }
            }
            return true;
        }
""" + _260_TAIL,
    # pads array named before the inner loop
    "pad_ptr": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
            if (button == 1) {
                UnkPadState_80051734 *pads = fn_80051734()->m_pads;
                for (u32 j = 0; j < 2; j++) {
                    u32 other = s_buttons[button][i];
                    if (Matches(other, pads[j].m_pressed)) {
                        s_pad = j;
                    }
                }
            }
            return true;
        }
""" + _260_TAIL,
    # the button test nested the other way
    "button_first": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0) {
            if (Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
                if (button == 1) {
                    for (u32 j = 0; j < 2; j++) {
                        u32 other = s_buttons[button][i];
                        if (Matches(other, fn_80051734()->m_pads[j].m_pressed)) {
                            s_pad = j;
                        }
                    }
                }
                return true;
            }
        }
""" + _260_TAIL,
    # row pointer
    "row_ptr": """    u32 *row = s_buttons[button];
    for (int i = 0; i < 3; i++) {
        u32 mask = row[i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
            if (button == 1) {
                for (u32 j = 0; j < 2; j++) {
                    u32 other = row[i];
                    if (Matches(other, fn_80051734()->m_pads[j].m_pressed)) {
                        s_pad = j;
                    }
                }
            }
            return true;
        }
""" + _260_TAIL,
    # early continue form
    "cont": """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask == 0) {
            continue;
        }
        if (!Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
            continue;
        }
        if (button == 1) {
            for (u32 j = 0; j < 2; j++) {
                u32 other = s_buttons[button][i];
                if (Matches(other, fn_80051734()->m_pads[j].m_pressed)) {
                    s_pad = j;
                }
            }
        }
        return true;
""" + _260_TAIL,
    # pad index scaled by hand before the loop (as fn_803560C8's prologue does)
    "pad_scaled": """    u32 *row = s_buttons[button];
    u32 mask;
    for (int i = 0; i < 3; i++) {
        mask = row[i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
            if (button == 1) {
                for (u32 j = 0; j < 2; j++) {
                    u32 other = row[i];
                    if (Matches(other, fn_80051734()->m_pads[j].m_pressed)) {
                        s_pad = j;
                    }
                }
            }
            return true;
        }
""" + _260_TAIL,
}

BODIES = {"fn_80356190": B190, "fn_80356260": B260}
