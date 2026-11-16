"""The byte-order helpers of Game/TExpandBG.cpp (wave 39, second pass): C forms of the swap and other
operand forms of the store intrinsics."""
S16 = "__sthbrx(*p, p, 0);"
S32 = "__stwbrx(*p, p, 0);"
REPL = {
 "c_swap": [(S16, "*p = (u16)((*p >> 8) | (*p << 8));"), (S32, "*p = (*p >> 24) | ((*p >> 8) & 0xFF00) | ((*p << 8) & 0xFF0000) | (*p << 24);")],
 "void_base": [(S16, "__sthbrx(*p, (void *)p, 0);"), (S32, "__stwbrx(*p, (void *)p, 0);")],
 "idx_form": [(S16, "__sthbrx(p[0], p, 0);"), (S32, "__stwbrx(p[0], p, 0);")],
 "cast_u32": [(S16, "__sthbrx((u32)*p, p, 0);"), (S32, "__stwbrx((u32)*p, p, 0);")],
 "cast_s": [(S16, "__sthbrx((u16)*p, p, 0);"), (S32, "__stwbrx((s32)*p, p, 0);")],
 "zero_base": [(S16, "__sthbrx(*p, 0, (int)p);"), (S32, "__stwbrx(*p, 0, (int)p);")],
 "lhbrx": [(S16, "*p = __lhbrx(p, 0);"), (S32, "*p = __lwbrx(p, 0);")],
}
SCORE = ["fn_80043CE4", "fn_80043D0C", "fn_80043C58"]
