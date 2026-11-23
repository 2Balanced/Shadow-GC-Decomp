"""fn_804210E4 (wave 40): a counting-down loop in the inlined swap keeps retail's `bne; b`; here
the call forms and the order of the two byte temporaries on top of it."""
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
F = "    for (; count > 0; count--) {\n"
B1 = "        int lo = src[0];\n        int hi = src[1];\n        src += 2;\n        dst[0] = hi;\n        dst[1] = lo;\n        dst += 2;\n    }"
B2 = "        int hi = src[1];\n        int lo = src[0];\n        src += 2;\n        dst[0] = hi;\n        dst[1] = lo;\n        dst += 2;\n    }"
B3 = "        u8 lo = src[0];\n        u8 hi = src[1];\n        src += 2;\n        dst[0] = hi;\n        dst[1] = lo;\n        dst += 2;\n    }"
B4 = "        u8 lo = *src++;\n        u8 hi = *src++;\n        *dst++ = hi;\n        *dst++ = lo;\n    }"
B5 = "        int lo = src[0];\n        int hi = src[1];\n        dst[0] = hi;\n        dst[1] = lo;\n        src += 2;\n        dst += 2;\n    }"
B6 = "        int lo;\n        int hi;\n        hi = src[1];\n        lo = src[0];\n        src += 2;\n        dst[1] = lo;\n        dst[0] = hi;\n        dst += 2;\n    }"
C1 = "        SwapBytes16((u8 *)&half[0], (u8 *)&half[0], 1);\n        SwapBytes16((u8 *)&half[1], (u8 *)&half[1], 1);\n"
C2 = OLD
C3 = "        SwapBytes16((u8 *)half, (u8 *)half, 1);\n        SwapBytes16((u8 *)half + 2, (u8 *)half + 2, 1);\n"
B7 = "        u8 lo = *src++;\n        u8 hi = *src++;\n        dst[0] = hi;\n        dst[1] = lo;\n        dst += 2;\n    }"
B8 = "        u8 lo = src[0];\n        u8 hi = src[1];\n        src += 2;\n        *dst++ = hi;\n        *dst++ = lo;\n    }"
B9 = "        int lo = *src++;\n        int hi = *src++;\n        *dst++ = hi;\n        *dst++ = lo;\n    }"
B10 = "        u8 hi = src[1];\n        u8 lo = src[0];\n        src += 2;\n        *dst++ = hi;\n        *dst++ = lo;\n    }"
B11 = "        u8 lo = *src;\n        src++;\n        u8 hi = *src;\n        src++;\n        *dst = hi;\n        dst++;\n        *dst = lo;\n        dst++;\n    }"
B12 = "        u8 hi = src[1];\n        u8 lo = *src;\n        src += 2;\n        *dst++ = hi;\n        *dst++ = lo;\n    }"
B13 = "        u8 lo = *src++;\n        u8 hi = *src++;\n        dst[1] = lo;\n        dst[0] = hi;\n        dst += 2;\n    }"
C4 = "        u8 *p0 = (u8 *)&half[0];\n        SwapBytes16(p0, (u8 *)&half[0], 1);\n        SwapBytes16((u8 *)&half[1], (u8 *)&half[1], 1);\n"
C5 = "        SwapBytes16((u8 *)half, (const u8 *)half, 1);\n        const u8 *p1 = (const u8 *)&half[1];\n        SwapBytes16((u8 *)p1, p1, 1);\n"
REPL = {}
for bn, b in (("b4", B4), ("b7", B7), ("b8", B8), ("b9", B9), ("b10", B10), ("b11", B11), ("b12", B12), ("b13", B13)):
    for cn, c in (("c1", C1), ("c2", C2), ("c3", C3), ("c4", C4), ("c5", C5)):
        REPL[bn + cn] = [(HOLD, F + b + T), (OLD, c)]
SCORE = ["fn_804210E4"]
