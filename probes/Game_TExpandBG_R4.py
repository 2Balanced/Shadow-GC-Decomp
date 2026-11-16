"""fn_80043C58 of Game/TExpandBG.cpp (wave 39, second pass): the swaps written in C."""
S16 = "                __sthbrx(*p, p, 0);\n"
S32 = "                __stwbrx(*p, p, 0);\n"
C16 = "                *p = ((*p >> 8) & 0xFF) | ((*p << 8) & 0xFF00);\n"
C32 = "                *p = (*p >> 24) | ((*p >> 8) & 0xFF00) | ((*p << 8) & 0xFF0000) | (*p << 24);\n"
D16 = "                u16 *p = (u16 *)(((u32)data + 1) & ~1);\n                data = (u8 *)(p + 1);\n"
D32 = "                u32 *p = (u32 *)(((u32)data + 3) & ~3);\n                data = (u8 *)(p + 1);\n"
REPL = {
 "c": [(S16, C16), (S32, C32)],
 "c_after": [(D16 + S16, "                u16 *p = (u16 *)(((u32)data + 1) & ~1);\n" + C16 + "                data = (u8 *)(p + 1);\n"), (D32 + S32, "                u32 *p = (u32 *)(((u32)data + 3) & ~3);\n" + C32 + "                data = (u8 *)(p + 1);\n")],
 "c_data": [(D16 + S16, "                data = (u8 *)(((u32)data + 1) & ~1);\n                u16 *p = (u16 *)data;\n" + C16 + "                data += 2;\n"), (D32 + S32, "                data = (u8 *)(((u32)data + 3) & ~3);\n                u32 *p = (u32 *)data;\n" + C32 + "                data += 4;\n")],
 "c_call": [(D16 + S16, "                data = (u8 *)(((u32)data + 1) & ~1);\n                fn_80043CE4(data, 2);\n                data += 2;\n"), (D32 + S32, "                data = (u8 *)(((u32)data + 3) & ~3);\n                fn_80043D0C(data, 4);\n                data += 4;\n")],
}
SCORE = ["fn_80043C58"]
