"""fn_80043FB8 of Game/TExpandBG.cpp, the PRS expander (wave 39, second pass). Retail: bits r10,
flags r9, in r8, start r7, count r6, from r5; set up in the order flags, in, bits, start."""
SYMBOLS = {"fn_80043FB8": 'extern "C" u32 fn_80043FB8(const u8 *src, u8 *dst) {'}
def body(decl, word="(lo | (hi << 8))", lohi="            u32 lo = in[0];\n            u32 hi = in[1];\n", frm="dst + offset", frm2="dst + (int)(*in++ | 0xFFFFFF00)", copy="            *dst++ = *from++;\n", loop=("    for (;;) {\n", "    }\n"), cnt0="count == 0"):
    return decl + loop[0] + """        u32 bit;
        do {
            if (--bits == 0) {
                flags = *in++;
                bits = 8;
            }
            bit = flags & 1;
            flags >>= 1;
            if (bit == 1) {
                *dst++ = *in++;
            }
        } while (bit == 1);
        if (--bits == 0) {
            flags = *in++;
            bits = 8;
        }
        bit = flags & 1;
        flags >>= 1;
        if (bit == 1) {
""" + lohi + """            in += 2;
            if (WORD == 0) {
                return dst - start;
            }
            count = lo & 7;
            int offset = (WORD >> 3) | 0xFFFFE000;
            if (CNT0) {
                count = *in++ + 1;
            } else {
                count += 2;
            }
            from = FRM;
        } else {
            count = 0;
            for (int i = 0; i < 2; i++) {
                if (--bits == 0) {
                    flags = *in++;
                    bits = 8;
                }
                bit = flags & 1;
                flags >>= 1;
                count = (count << 1) | bit;
            }
            count += 2;
            from = FRM2;
        }
        for (; count != 0; count--) {
""".replace("WORD", word).replace("FRM2", frm2).replace("FRM", frm).replace("CNT0", cnt0) + copy + "        }\n" + loop[1]
D0 = "    int bits = 9;\n    u32 count;\n    u8 *from;\n    u8 *start = dst;\n    const u8 *in = src + 1;\n    u32 flags = *src;\n"
D1 = "    u8 *from;\n    u32 count;\n    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n    start = dst;\n"
D2 = "    u32 flags = *src;\n    const u8 *in = src + 1;\n    int bits = 9;\n    u8 *start = dst;\n    u32 count;\n    u8 *from;\n"
D3 = "    int bits;\n    u32 flags;\n    const u8 *in;\n    u8 *start;\n    u32 count;\n    u8 *from;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n    start = dst;\n"
D4 = "    u32 flags = *src;\n    const u8 *in = src + 1;\n    int bits = 9;\n    u8 *start = dst;\n    u8 *from;\n    u32 count;\n"
BODIES = {"fn_80043FB8": {
 "d1": body(D1), "d2": body(D2), "d3": body(D3), "d4": body(D4),
 "d2_frm": body(D2, frm="offset + dst", frm2="(int)(*in++ | 0xFFFFFF00) + dst"),
 "d2_frm_u8": body(D2, frm="offset + dst", frm2="(int)(*in++ | 0xFFFFFF00) + dst", lohi="            u8 lo = in[0];\n            u8 hi = in[1];\n", word="(u16)(lo | (hi << 8))"),
 "d2_frm_mask": body(D2, frm="offset + dst", frm2="(int)(*in++ | 0xFFFFFF00) + dst", word="(lo | ((hi << 8) & 0xFF00))"),
 "d2_copy": body(D2, frm="offset + dst", frm2="(int)(*in++ | 0xFFFFFF00) + dst", word="(lo | ((hi << 8) & 0xFF00))", copy="            *dst = *from;\n            from++;\n            dst++;\n"),
 "d2_while": body(D2, frm="offset + dst", frm2="(int)(*in++ | 0xFFFFFF00) + dst", word="(lo | ((hi << 8) & 0xFF00))", loop=("    while (true) {\n", "    }\n")),
}}
F = dict(frm="offset + dst", frm2="(int)(*in++ | 0xFFFFFF00) + dst")
M = dict(word="(lo | ((hi << 8) & 0xFF00))")
BODIES2 = {"fn_80043FB8": {
 "d0": body(D0),
 "d0_frm": body(D0, **F),
 "d0_mask": body(D0, **M),
 "d0_frm_mask": body(D0, **F, **M),
 "d0_fm_copy": body(D0, copy="            *dst = *from;\n            from++;\n            dst++;\n", **F, **M),
 "d0_fm_copy2": body(D0, copy="            *dst++ = *from;\n            from++;\n", **F, **M),
 "d0_fm_u16": body(D0, frm="offset + dst", frm2="(int)(*in++ | 0xFFFFFF00) + dst", word="(u16)(lo | (hi << 8))"),
 "d0_fm_u8hi": body(D0, lohi="            u32 lo = in[0];\n            u8 hi = in[1];\n", word="(lo | (u16)(hi << 8))", **F),
}}

BODIES["fn_80043FB8"].update(BODIES2["fn_80043FB8"])
