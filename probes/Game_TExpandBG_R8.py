"""fn_80043FB8 of Game/TExpandBG.cpp, the PRS expander (wave 39, second pass): the copy reads
dst[offset], which the compiler turns into retail's pointer in the offset's register."""
SYMBOLS = {"fn_80043FB8": 'extern "C" u32 fn_80043FB8(const u8 *src, u8 *dst) {'}
def body(decl, copy="            *dst = dst[offset];\n            dst++;\n", loop=("    for (;;) {\n", "    }\n"), bitdecl="        u32 bit;\n", off2="offset = *in++ | 0xFFFFFF00;", word="(lo | ((hi << 8) & 0xFF00))", tail=""):
    return decl + loop[0] + bitdecl + """        do {
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
            u32 lo = in[0];
            u32 hi = in[1];
            in += 2;
            if (WORD == 0) {
                return dst - start;
            }
            count = lo & 7;
            offset = (WORD >> 3) | 0xFFFFE000;
            if (count == 0) {
                count = *in++ + 1;
            } else {
                count += 2;
            }
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
            OFF2
        }
        for (; count != 0; count--) {
""".replace("WORD", word).replace("OFF2", off2) + copy + "        }\n" + loop[1] + tail
D1 = "    int offset;\n    u32 count;\n    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n    start = dst;\n"
D1b = "    int offset;\n    u32 count;\n    u8 *start = dst;\n    const u8 *in = src + 1;\n    u32 flags = *src;\n    int bits = 9;\n"
D0 = "    int bits = 9;\n    u32 count;\n    int offset;\n    u8 *start = dst;\n    const u8 *in = src + 1;\n    u32 flags = *src;\n"
D2 = "    u32 flags = *src;\n    const u8 *in = src + 1;\n    int bits = 9;\n    u8 *start = dst;\n    u32 count;\n    int offset;\n"
BODIES = {"fn_80043FB8": {
 "d1": body(D1), "d1b": body(D1b), "d0": body(D0), "d2": body(D2),
 "d1_bit_top": body("    u32 bit;\n" + D1, bitdecl=""),
 "d1_copy2": body(D1, copy="            *dst++ = dst[offset];\n"),
 "d1_copy3": body(D1, copy="            dst[0] = dst[offset];\n            dst++;\n"),
 "d1_while": body(D1, loop=("    while (true) {\n", "    }\n")),
 "d1_do": body(D1, loop=("    do {\n", "    } while (true);\n")),
}}
