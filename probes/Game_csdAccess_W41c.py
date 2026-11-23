"""Wave 41: InitCast's float constants as namespace-scope const objects (not literals)."""
SCORE = ["InitCast__Q24Chao3CSDFPv"]
D0 = "inline void SetDefaults() {\n"
SD = """        s_defaults[1].m_float = 0.0f;
        s_defaults[2].m_float = 0.0f;
        s_defaults[3].m_float = 0.0f;
        s_defaults[4].m_float = 1.0f;
        s_defaults[5].m_float = 1.0f;
        s_defaults[6].m_float = 0.0f;
"""
SD2 = SD.replace("0.0f", "kZero").replace("1.0f", "kOne")
IC = """    *(f32 *)(p + 0x38) = 1.0f;
    *(f32 *)(p + 0x3C) = 1.0f;
    *(f32 *)(p + 0x40) = -100000.0f;
    *(s32 *)(p + 0x48) = *(s32 *)(p + 0x4C) = *(s32 *)(p + 0x50) = *(s32 *)(p + 0x54) = -1;
    *(f32 *)(p + 0x6C) = -1.0f;
    *(f32 *)(p + 0x70) = -1000000.0f;
"""
IC2 = IC.replace("-100000.0f", "kFar").replace("-1000000.0f", "kFarther").replace("-1.0f", "kMinusOne").replace("1.0f", "kOne")
DECL_C = "const float kZero = 0.0f;\nconst float kOne = 1.0f;\nconst float kFar = -100000.0f;\nconst float kMinusOne = -1.0f;\nconst float kFarther = -1000000.0f;\n"
DECL_S = DECL_C.replace("const float", "static const float")
DECL_E = DECL_C.replace("const float", "extern const float")
REPL = {
  "ns_const": [(D0, DECL_C + D0), (SD, SD2), (IC, IC2)],
  "ns_static_const": [(D0, DECL_S + D0), (SD, SD2), (IC, IC2)],
  "ns_static_const_poolprag": [(D0, "#pragma pool_data off\n" + DECL_S + "#pragma pool_data reset\n" + D0), (SD, SD2), (IC, IC2)],
}
