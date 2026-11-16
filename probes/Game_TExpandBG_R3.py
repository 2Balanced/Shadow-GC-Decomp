"""The byte-order helpers of Game/TExpandBG.cpp (wave 39, second pass): C forms of the 16-bit swap
(the 32-bit one is retail's as (v >> 24) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000) | (v << 24))."""
S16 = "__sthbrx(*p, p, 0);"
S32 = "__stwbrx(*p, p, 0);"
W = (S32, "*p = (*p >> 24) | ((*p >> 8) & 0xFF00) | ((*p << 8) & 0xFF0000) | (*p << 24);")
F = {
 "nocast": "*p = (*p >> 8) | (*p << 8);",
 "mask_lo": "*p = (*p >> 8) | ((*p & 0xFF) << 8);",
 "mask_both": "*p = ((*p >> 8) & 0xFF) | ((*p << 8) & 0xFF00);",
 "shl_first": "*p = (*p << 8) | (*p >> 8);",
 "mask_shl_first": "*p = ((*p << 8) & 0xFF00) | ((*p >> 8) & 0xFF);",
 "cast_each": "*p = (u16)(*p >> 8) | (u16)(*p << 8);",
 "local": "u16 value = *p;\n        *p = (value >> 8) | (value << 8);",
 "local_mask": "u16 value = *p;\n        *p = ((value >> 8) & 0xFF) | ((value << 8) & 0xFF00);",
 "ff00": "*p = ((*p & 0xFF00) >> 8) | ((*p & 0x00FF) << 8);",
 "ff00_rev": "*p = ((*p & 0x00FF) << 8) | ((*p & 0xFF00) >> 8);",
}
REPL = {k: [(S16, v), W] for k, v in F.items()}
SCORE = ["fn_80043CE4", "fn_80043D0C", "fn_80043C58"]
