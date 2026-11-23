"""Wave 41: does the pool_data state at an inline function's definition decide its literals?"""
SCORE = ["InitCast__Q24Chao3CSDFPv"]
D0 = "inline void SetDefaults() {\n"
D1 = """            s_allAttributes |= 1 << i;
        }
    }
}
"""
H = "void InitCast(void *cast) {\n"
IC = """    u8 *p = (u8 *)cast;
    *(s32 *)(p + 0x24) = -1;
    *(s32 *)(p + 0x44) = -1;
    *(f32 *)(p + 0x38) = 1.0f;
    *(f32 *)(p + 0x3C) = 1.0f;
    *(f32 *)(p + 0x40) = -100000.0f;
    *(s32 *)(p + 0x48) = *(s32 *)(p + 0x4C) = *(s32 *)(p + 0x50) = *(s32 *)(p + 0x54) = -1;
    *(f32 *)(p + 0x6C) = -1.0f;
    *(f32 *)(p + 0x70) = -1000000.0f;
"""
TAIL = "inline void SetCastTail(void *cast) {\n" + IC + "}\n"
REPL = {
  "defaults_off": [(D0, "#pragma pool_data off\n" + D0), (D1, D1 + "#pragma pool_data reset\n")],
  "defaults_tail_off": [(D0, "#pragma pool_data off\n" + D0), (D1, D1 + TAIL + "#pragma pool_data reset\n"), (IC, "    SetCastTail(cast);\n")],
}
