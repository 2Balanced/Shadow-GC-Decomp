"""Wave 41: fn_80356190 and fn_80356260 with copy propagation off and named copies of the
parameters (retail fn_80356190: counter r27, mask r28, pad r29, row r30, byte offset r31)."""
A = "fn_80356190"
B = "fn_80356260"
SCORE = [A, B]
HA = 'extern "C" bool fn_80356190(int button, int pad) {\n'
NA = "/* Whether logical button button is held on pad pad. */\n"
BODY_A = """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && fn_80356214(fn_80051734(), pad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
}
"""
OFF_A = [(HA, "#pragma opt_propagation off\n" + HA), (NA, "#pragma opt_propagation reset\n" + NA)]
def a(body): return OFF_A + [(HA + BODY_A, HA + body)]
REPL = {
    "a_off": OFF_A,
    "a_p_first": a("    int p = pad;\n" + BODY_A.replace("pad, mask", "p, mask")),
    "a_i_p": a("    int i;\n    int p = pad;\n" + BODY_A.replace("for (int i = 0;", "for (i = 0;").replace("pad, mask", "p, mask")),
    "a_i_m_p": a("    int i;\n    u32 mask;\n    int p = pad;\n" + BODY_A.replace("for (int i = 0;", "for (i = 0;").replace("u32 mask = ", "mask = ").replace("pad, mask", "p, mask")),
    "a_p_i": a("    int p = pad;\n    int i;\n" + BODY_A.replace("for (int i = 0;", "for (i = 0;").replace("pad, mask", "p, mask")),
    "a_m_p_i": a("    u32 mask;\n    int p = pad;\n    int i;\n" + BODY_A.replace("for (int i = 0;", "for (i = 0;").replace("u32 mask = ", "mask = ").replace("pad, mask", "p, mask")),
    "a_i_m": a("    int i;\n    u32 mask;\n" + BODY_A.replace("for (int i = 0;", "for (i = 0;").replace("u32 mask = ", "mask = ")),
    "a_b_copy": a("    int b = button;\n" + BODY_A.replace("s_buttons[button]", "s_buttons[b]")),
}
REPL.update({
    "a_nomask": [(HA + BODY_A, HA + """    for (int i = 0; i < 3; i++) {
        if (s_buttons[button][i] != 0 && fn_80356214(fn_80051734(), pad, s_buttons[button][i], 12, 1)) {
            return true;
        }
    }
    return false;
}
""")],
    "a_itop_nomask": [(HA + BODY_A, HA + """    int i;
    for (i = 0; i < 3; i++) {
        if (s_buttons[button][i] != 0 && fn_80356214(fn_80051734(), pad, s_buttons[button][i], 12, 1)) {
            return true;
        }
    }
    return false;
}
""")],
    "a_row": [(HA + BODY_A, HA + """    u32 *row = s_buttons[button];
    for (int i = 0; i < 3; i++) {
        u32 mask = row[i];
        if (mask != 0 && fn_80356214(fn_80051734(), pad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
}
""")],
    "a_inl_pads": [(HA + BODY_A, HA + """    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && ((UnkPads_80051734 *)fn_80051734())->IsRepeated(pad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
}
""")],
})
