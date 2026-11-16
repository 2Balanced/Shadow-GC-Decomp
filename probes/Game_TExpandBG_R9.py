"""fn_80043FB8 of Game/TExpandBG.cpp, the PRS expander (wave 39, second pass): the long match's
offset kept in the source pointer's own variable."""
SYMBOLS = {"fn_80043FB8": 'extern "C" u32 fn_80043FB8(const u8 *src, u8 *dst) {'}
def body(decl, off1, fin1, off2, copy="            *dst++ = *from++;\n", word="(lo | ((hi << 8) & 0xFF00))"):
    return decl + """    for (;;) {
        u32 bit;
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
            u32 lo = in[0];
            u32 hi = in[1];
            in += 2;
            if (WORD == 0) {
                return dst - start;
            }
            count = lo & 7;
            OFF1
            if (count == 0) {
                count = *in++ + 1;
            } else {
                count += 2;
            }
FIN1        } else {
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
""".replace("WORD", word).replace("OFF1", off1).replace("FIN1", fin1).replace("OFF2", off2) + copy + "        }\n    }\n"
DP = "    u8 *from;\n    u32 count;\n    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n    start = dst;\n"
DI = DP.replace("    u8 *from;\n", "    int offset;\n    u8 *from;\n")
DI2 = DP.replace("    u8 *from;\n", "    u8 *from;\n    int offset;\n")
W = "(lo | ((hi << 8) & 0xFF00))"
BODIES = {"fn_80043FB8": {
 "same_var": body(DP, "from = (u8 *)((%s >> 3) | 0xFFFFE000);" % W, "            from = (u8 *)((int)from + (u32)dst);\n", "from = (int)(*in++ | 0xFFFFFF00) + dst;"),
 "same_var2": body(DP, "from = (u8 *)((%s >> 3) | 0xFFFFE000);" % W, "            from += (u32)dst;\n", "from = (int)(*in++ | 0xFFFFFF00) + dst;"),
 "off_local_first": body(DI, "offset = (%s >> 3) | 0xFFFFE000;" % W, "            from = offset + dst;\n", "from = (int)(*in++ | 0xFFFFFF00) + dst;"),
 "off_local_second": body(DI2, "offset = (%s >> 3) | 0xFFFFE000;" % W, "            from = offset + dst;\n", "from = (int)(*in++ | 0xFFFFFF00) + dst;"),
 "off_block": body(DP, "int offset = (%s >> 3) | 0xFFFFE000;" % W, "            from = offset + dst;\n", "from = (int)(*in++ | 0xFFFFFF00) + dst;"),
 "off_block_idx": body(DP, "int offset = (%s >> 3) | 0xFFFFE000;" % W, "            from = &dst[offset];\n", "from = &dst[(int)(*in++ | 0xFFFFFF00)];"),
 "off_both": body(DI, "offset = (%s >> 3) | 0xFFFFE000;" % W, "            from = offset + dst;\n", "offset = *in++ | 0xFFFFFF00;\n            from = offset + dst;"),
}}
O1 = "from = (u8 *)((%s >> 3) | 0xFFFFE000);" % W
F1 = "            from += (u32)dst;\n"
DC = "    u32 count;\n    u8 *from;\n    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n    start = dst;\n"
DC2 = "    u32 count;\n    u8 *from;\n    u32 flags = *src;\n    const u8 *in = src + 1;\n    int bits = 9;\n    u8 *start = dst;\n"
DC3 = "    u32 count;\n    u8 *from;\n    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    start = dst;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n"
B2 = {"cf": body(DC, O1, F1, "from = (int)(*in++ | 0xFFFFFF00) + dst;"),
 "cf_b2": body(DC, O1, F1, "from = dst + (int)(*in++ | 0xFFFFFF00);"),
 "cf_b3": body(DC, O1, F1, "from = (u8 *)(*in++ | 0xFFFFFF00);\n            from += (u32)dst;"),
 "cf_b4": body(DC, O1, F1, "from = (u8 *)((*in++ | 0xFFFFFF00) + (u32)dst);"),
 "cf2": body(DC2, O1, F1, "from = (u8 *)((*in++ | 0xFFFFFF00) + (u32)dst);"),
 "cf3": body(DC3, O1, F1, "from = (u8 *)((*in++ | 0xFFFFFF00) + (u32)dst);"),
}
BODIES["fn_80043FB8"] = B2
def swapped(b):
    a = "            count = lo & 7;\n            " + O1 + "\n"
    assert a in b
    return b.replace(a, "            " + O1 + "\n            count = lo & 7;\n")
B3 = {
 "sw": swapped(B2["cf"]),
 "sw_dp": swapped(body(DP, O1, F1, "from = (int)(*in++ | 0xFFFFFF00) + dst;")),
 "from_init": body(DP.replace("    start = dst;\n", "    start = dst;\n    from = dst;\n"), O1, F1, "from = (int)(*in++ | 0xFFFFFF00) + dst;"),
 "count_u8": body(DP.replace("u32 count", "int count"), O1, F1, "from = (int)(*in++ | 0xFFFFFF00) + dst;"),
}
BODIES["fn_80043FB8"] = B3
OI = "from = (%s >> 3) | 0xFFFFE000;" % W
FI = "            from += (u32)dst;\n"
CI = "            *dst++ = *(u8 *)from;\n            from++;\n"
def D(first, second):
    return "    %s;\n    %s;\n    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n    start = dst;\n" % (first, second)
B4 = {
 "u32_fc": body(D("u32 from", "u32 count"), OI, FI, "from = (*in++ | 0xFFFFFF00) + (u32)dst;", copy=CI),
 "u32_cf": body(D("u32 count", "u32 from"), OI, FI, "from = (*in++ | 0xFFFFFF00) + (u32)dst;", copy=CI),
 "s32_fc": body(D("s32 from", "u32 count"), OI, FI, "from = (*in++ | 0xFFFFFF00) + (u32)dst;", copy=CI),
 "s32_fc_scount": body(D("s32 from", "s32 count"), OI, FI, "from = (*in++ | 0xFFFFFF00) + (u32)dst;", copy=CI),
 "cu8_fc": body(D("const u8 *from", "u32 count"), "from = (const u8 *)((%s >> 3) | 0xFFFFE000);" % W, FI, "from = (int)(*in++ | 0xFFFFFF00) + dst;"),
 "u32_fc_idx": body(D("u32 from", "u32 count"), OI, FI, "from = (*in++ | 0xFFFFFF00) + (u32)dst;", copy="            *dst++ = *(u8 *)from++;\n"),
}
BODIES["fn_80043FB8"] = B4
CU = CI
def mk(decl, loop=None, ret=None):
    b = body(decl, OI, FI, "from = (*in++ | 0xFFFFFF00) + (u32)dst;", copy=CI).replace("for (; count != 0; count--)", "for (; count != 0u; count--)")
    if loop:
        b = b.replace("    for (;;) {\n", loop[0], 1)
        k = b.rindex("    }\n")
        b = b[:k] + loop[1]
    if ret:
        b = b.replace("                return dst - start;\n", ret[0]) + ret[1]
    return b
H = "    s32 from;\n    s32 count;\n"
B5 = {
 "base": mk(H + "    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n    start = dst;\n"),
 "init_decl": mk(H + "    u8 *start = dst;\n    const u8 *in = src + 1;\n    u32 flags = *src;\n    int bits = 9;\n"),
 "start_last_stmt": mk(H + "    u8 *start;\n    const u8 *in = src + 1;\n    u32 flags = *src;\n    int bits = 9;\n    start = dst;\n"),
 "bits_decl_init": mk(H + "    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits = 9;\n    flags = *src;\n    in = src + 1;\n    start = dst;\n"),
 "src_inc": mk(H + "    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    in = src;\n    flags = *in++;\n    bits = 9;\n    start = dst;\n"),
 "while1": mk(H + "    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n    start = dst;\n", loop=("    while (1) {\n", "    }\n")),
 "break_ret": mk(H + "    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n    start = dst;\n", ret=("                break;\n", "    return dst - start;\n")),
 "goto_ret": mk(H + "    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n    start = dst;\n", ret=("                goto done;\n", "done:\n    return dst - start;\n")),
}
BODIES["fn_80043FB8"] = B5
HD = H + "    u8 *start;\n    const u8 *in;\n    u32 flags;\n    int bits;\n    flags = *src;\n    in = src + 1;\n    bits = 9;\n    start = dst;\n"
B6 = {
 "goto_loop": mk(HD, loop=("again: {\n", "    }\n    goto again;\n")),
 "do_while1": mk(HD, loop=("    do {\n", "    } while (1);\n")),
 "do_while_true": mk(HD, loop=("    do {\n", "    } while (true);\n")),
 "while_start": mk(HD, loop=("    while (start != 0) {\n", "    }\n    return 0;\n")),
 "for_cond": mk(HD, loop=("    for (;;) {\n", "    }\n    return 0;\n")),
}
BODIES["fn_80043FB8"] = B6
BODIES["fn_80043FB8"] = {"final": mk(HD)}
