"""fn_804210E4 (wave 40): shapes of the inlined single swap."""
OLD = """        u8 *p0 = (u8 *)&half[0];
        SwapBytes16(p0, p0, 1);
        u8 *p1 = (u8 *)&half[1];
        SwapBytes16(p1, p1, 1);
"""
HOLD = """    for (int i = 0; i < count; i++) {
        int lo = src[0];
        int hi = src[1];
        src += 2;
        dst[0] = hi;
        dst[1] = lo;
        dst += 2;
    }
}

/* Sets whether"""
T = "\n}\n\n/* Sets whether"
E = "        SwapBytes16((u8 *)&half[0], (u8 *)&half[0], 1);\n        SwapBytes16((u8 *)&half[1], (u8 *)&half[1], 1);\n"
def h(body, call=E):
    return [(HOLD, body + T), (OLD, call)]
REPL = {
 "one": h("    int lo = src[0];\n    int hi = src[1];\n    dst[0] = hi;\n    dst[1] = lo;"),
 "one_u8": h("    u8 lo = src[0];\n    u8 hi = src[1];\n    dst[0] = hi;\n    dst[1] = lo;"),
 "one_hilo": h("    u8 hi = src[1];\n    u8 lo = src[0];\n    dst[0] = hi;\n    dst[1] = lo;"),
 "while": h("    while (count-- > 0) {\n        int lo = src[0];\n        int hi = src[1];\n        src += 2;\n        dst[0] = hi;\n        dst[1] = lo;\n        dst += 2;\n    }"),
 "dowhile": h("    do {\n        int lo = src[0];\n        int hi = src[1];\n        src += 2;\n        dst[0] = hi;\n        dst[1] = lo;\n        dst += 2;\n    } while (--count > 0);"),
 "one_tmp": h("    u8 t = src[0];\n    dst[0] = src[1];\n    dst[1] = t;"),
 "one_noalias": h("    u8 hi = src[1];\n    dst[1] = src[0];\n    dst[0] = hi;"),
}

L = "        int lo = src[0];\n        int hi = src[1];\n        src += 2;\n        dst[0] = hi;\n        dst[1] = lo;\n        dst += 2;\n    }"
REPL.update({
 "for_dec": h("    for (; count > 0; count--) {\n" + L),
 "for_ne": h("    for (int i = 0; i != count; i++) {\n" + L),
 "for_rev": h("    for (int i = count; i > 0; i--) {\n" + L),
 "while_nz": h("    while (count--) {\n" + L),
 "while_gt": h("    while (count > 0) {\n        count--;\n" + L),
 "for_u": h("    for (u32 i = 0; i < (u32)count; i++) {\n" + L),
})
SCORE = ["fn_804210E4"]
