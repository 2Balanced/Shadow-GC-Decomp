"""Source shapes for Game/PadButtons.cpp fn_80356260 (round: combinations on mask_outer).

Round 1 (9 shapes, one axis at a time) scored: committed 18, cont 18, button_first 18,
inner_int 19, mask_outer 15, same_mask 24, row_ptr 24, pad_scaled 21, pad_ptr 32.
fn_80356190's floor is 5 rows over 16 shapes (see probes/Game_PadButtons.py history in git), so
this unit cannot become Matching while the pad/counter slot swap stands.

Mask register target (retail): i/j r27, button*12 r28, pad*40 r29, mask r30, byte offset r31;
ours puts mask in the first saved slot (r27) and shifts everything up.  Declaring the mask at
function scope (mask_outer) removes 3 rows, so the combinations below build on that.
"""

SYMBOLS = {"fn_80356260": 'extern "C" bool fn_80356260(int button, int pad) {'}

BLOCK = """            if (button == 1) {
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
    return false;
"""

MASK_OUTER_HEAD = """    u32 mask;
    for (int i = 0; i < 3; i++) {
        mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
"""

MASK_INNER_HEAD = """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
"""

HEADS = {
    "mask_outer": MASK_OUTER_HEAD,
    "mask_outer_pad_first": """    u32 mask;
    for (int i = 0; i < 3; i++) {
        mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
""",
    "mask_ptr": """    u32 *row = s_buttons[button];
    u32 mask;
    for (int i = 0; i < 3; i++) {
        mask = row[i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
""",
    "counter_out": """    u32 mask;
    int i = 0;
    for (; i < 3; i++) {
        mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
""",
}

# inner-loop spellings
INNER = {
    "as_committed": BLOCK,
    "inner_int": BLOCK.replace("for (u32 j = 0; j < 2; j++)", "for (int j = 0; j < 2; j++)"),
    "same_mask": BLOCK.replace("u32 other = s_buttons[button][i];\n", "").replace(
        "Matches(other,", "Matches(mask,"
    ),
    "pads_local": BLOCK.replace(
        "for (u32 j = 0; j < 2; j++) {",
        "UnkPadState_80051734 *pads = fn_80051734()->m_pads;\n"
        "                for (u32 j = 0; j < 2; j++) {",
    ).replace("fn_80051734()->m_pads[j]", "pads[j]"),
}

# whole-function alternative spellings (own head+body)
STANDALONE = {
    "mask_outer_cont": """    u32 mask;
    for (int i = 0; i < 3; i++) {
        mask = s_buttons[button][i];
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
    }
    return false;
""",
    "mask_outer_nested": """    u32 mask;
    for (int i = 0; i < 3; i++) {
        mask = s_buttons[button][i];
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
    }
    return false;
""",
    "mask_and_pad_outer": """    u32 mask;
    u32 padIndex = pad;
    for (int i = 0; i < 3; i++) {
        mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[padIndex].m_pressed)) {
""" + BLOCK,
    "mask_and_pad_scaled": """    u32 mask;
    u32 padOffset = pad * 40;
    for (int i = 0; i < 3; i++) {
        mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, *(u32 *)((u8 *)fn_80051734() + 48 + padOffset))) {
""" + BLOCK,
}

BODIES = {"fn_80356260": {}}
for _hn, _head in HEADS.items():
    for _in, _body in INNER.items():
        if _hn == "mask_outer" and _in == "as_committed":
            continue
        BODIES["fn_80356260"][_hn + "_" + _in] = "\n" + _head + _body
BODIES["fn_80356260"].update({k: "\n" + v for k, v in STANDALONE.items()})
