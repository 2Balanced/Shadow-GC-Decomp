"""Source shapes for fn_80027D5C, the clump fade's constructor (wave 40): retail keeps the loop's
offset in r7 (created before the colour's copy, which uses r5 and r4 from the base in r6); GC/2.7
makes the offset last and gives it r4."""
SYMBOLS = {"fn_80027D5C": "extern \"C\" UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade) {"}
XTRA = {"fn_80027D5C": """inline void FillOnes_(float *values) {
    for (int i = 0; i < 2; i++) {
        values[i] = 1.0f;
    }
}
inline void FillOnesOf_(UnkClumpFade_80027D5C *fade) {
    for (int i = 0; i < 2; i++) {
        fade->field_0x10[i] = 1.0f;
    }
}
inline void SetOnes3_(float *v) {
    v[0] = 1.0f;
    v[1] = 1.0f;
    v[2] = 1.0f;
}
inline void CopyColor_(UnkClumpFade_80027D5C *fade, const UnkFadeColor_80027D5C &color) {
    *reinterpret_cast<UnkFadeColor_80027D5C *>(fade->m_color) = color;
}
"""}
C = "    *reinterpret_cast<UnkFadeColor_80027D5C *>(fade->m_color) = lbl_804AB220;\n"
O = "    fade->field_0x18[0] = 1.0f;\n    fade->field_0x18[1] = 1.0f;\n    fade->field_0x18[2] = 1.0f;\n"
L = "    for (int i = 0; i < 2; i++) {\n        fade->field_0x10[i] = 1.0f;\n    }\n"
T = "    fade->m_mode = 0;\n    fade->field_0x28 = -1.0f;\n    return fade;\n"
BODIES = {"fn_80027D5C": {
    "natural": C + O + L + T,
    "helper": C + O + "    FillOnes_(fade->field_0x10);\n" + T,
    "helper_of": C + O + "    FillOnesOf_(fade);\n" + T,
    "helper3": C + "    SetOnes3_(fade->field_0x18);\n" + L + T,
    "helper_both": C + "    SetOnes3_(fade->field_0x18);\n    FillOnes_(fade->field_0x10);\n" + T,
    "copy_inl": "    CopyColor_(fade, lbl_804AB220);\n" + O + L + T,
    "copy_inl_top_i": "    int i = 0;\n    CopyColor_(fade, lbl_804AB220);\n" + O + "    for (; i < 2; i++) {\n        fade->field_0x10[i] = 1.0f;\n    }\n" + T,
    "ptr_loop": C + O + "    float *p = fade->field_0x10;\n    for (int i = 0; i < 2; i++) {\n        *p++ = 1.0f;\n    }\n" + T,
    "i_decl_only": "    int i;\n" + C + O + "    for (i = 0; i < 2; i++) {\n        fade->field_0x10[i] = 1.0f;\n    }\n" + T,
    "u32_i": "    u32 i = 0;\n" + C + O + "    for (; i < 2; i++) {\n        fade->field_0x10[i] = 1.0f;\n    }\n" + T,
    "color_local": "    int i = 0;\n    const UnkFadeColor_80027D5C &color = lbl_804AB220;\n    *reinterpret_cast<UnkFadeColor_80027D5C *>(fade->m_color) = color;\n" + O + "    for (; i < 2; i++) {\n        fade->field_0x10[i] = 1.0f;\n    }\n" + T,
    "words": "    int i = 0;\n    const u32 *src = reinterpret_cast<const u32 *>(&lbl_804AB220);\n    u32 *dst = reinterpret_cast<u32 *>(fade->m_color);\n    dst[0] = src[0];\n    dst[1] = src[1];\n    dst[2] = src[2];\n    dst[3] = src[3];\n" + O + "    for (; i < 2; i++) {\n        fade->field_0x10[i] = 1.0f;\n    }\n" + T,
}}
