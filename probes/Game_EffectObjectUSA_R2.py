"""Wave 40: the colour conversion of EffectObjectUSA's constructor and Update (retail: alpha's
conversion in the highest stack slot, then red, green and blue; all four loads first)."""
SCORE = ["Update__Q412SonicteamUSA6System6Effect15EffectObjectUSAFf", "__ct__Q412SonicteamUSA6System6Effect15EffectObjectUSAFPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"]
OLD = "        float scale = 1.0f / 255.0f;\n        SetFadeColor(&m_fade, ((m_color[3] >> 22) & 0xFF) * scale,\n                     ((m_color[0] >> 22) & 0xFF) * scale, ((m_color[1] >> 22) & 0xFF) * scale,\n                     ((m_color[2] >> 22) & 0xFF) * scale);\n"
A = "extern \"C\" UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade) {"
H = """inline float ColorByte_(const u32 *color, int i) { return ((color[i] >> 22) & 0xFF) * (1.0f / 255.0f); }
inline float ColorByteS_(const u32 *color, int i, float scale) { return ((color[i] >> 22) & 0xFF) * scale; }
inline u32 Byte_(const u32 *color, int i) { return (color[i] >> 22) & 0xFF; }
inline u32 ByteN_(u32 *color, int i) { return (color[i] >> 22) & 0xFF; }
inline void SetFadeFrom_(UnkClumpFade_80027D5C *fade, const u32 *color) {
    float scale = 1.0f / 255.0f;
    fade->m_color[0] = ((color[0] >> 22) & 0xFF) * scale;
    fade->m_color[1] = ((color[1] >> 22) & 0xFF) * scale;
    fade->m_color[2] = ((color[2] >> 22) & 0xFF) * scale;
    fade->m_color[3] = ((color[3] >> 22) & 0xFF) * scale;
}
inline void SetFadeFrom2_(UnkClumpFade_80027D5C *fade, const u32 *color) {
    float scale = 1.0f / 255.0f;
    SetFadeColor(fade, ((color[3] >> 22) & 0xFF) * scale, ((color[0] >> 22) & 0xFF) * scale,
                 ((color[1] >> 22) & 0xFF) * scale, ((color[2] >> 22) & 0xFF) * scale);
}
inline void SetFadeFromN_(UnkClumpFade_80027D5C *fade, u32 *color) {
    float scale = 1.0f / 255.0f;
    fade->m_color[0] = ((color[0] >> 22) & 0xFF) * scale;
    fade->m_color[1] = ((color[1] >> 22) & 0xFF) * scale;
    fade->m_color[2] = ((color[2] >> 22) & 0xFF) * scale;
    fade->m_color[3] = ((color[3] >> 22) & 0xFF) * scale;
}
inline void SetFadeBytes_(UnkClumpFade_80027D5C *fade, u32 a, u32 r, u32 g, u32 b) {
    float scale = 1.0f / 255.0f;
    fade->m_color[0] = r * scale;
    fade->m_color[1] = g * scale;
    fade->m_color[2] = b * scale;
    fade->m_color[3] = a * scale;
}
""" + A
def v(new):
    return [(A, H), (OLD, new), (OLD, new)]
REPL = {
  "byte_inl": v("        SetFadeColor(&m_fade, ColorByte_(m_color, 3), ColorByte_(m_color, 0), ColorByte_(m_color, 1), ColorByte_(m_color, 2));\n"),
  "byte_inl_s": v("        float scale = 1.0f / 255.0f;\n        SetFadeColor(&m_fade, ColorByteS_(m_color, 3, scale), ColorByteS_(m_color, 0, scale), ColorByteS_(m_color, 1, scale), ColorByteS_(m_color, 2, scale));\n"),
  "from_const": v("        SetFadeFrom_(&m_fade, m_color);\n"),
  "from_const2": v("        SetFadeFrom2_(&m_fade, m_color);\n"),
  "from_nonconst": v("        SetFadeFromN_(&m_fade, m_color);\n"),
  "bytes": v("        SetFadeBytes_(&m_fade, (m_color[3] >> 22) & 0xFF, (m_color[0] >> 22) & 0xFF, (m_color[1] >> 22) & 0xFF, (m_color[2] >> 22) & 0xFF);\n"),
  "bytes_inl": v("        SetFadeBytes_(&m_fade, Byte_(m_color, 3), Byte_(m_color, 0), Byte_(m_color, 1), Byte_(m_color, 2));\n"),
  "bytes_inl_n": v("        SetFadeBytes_(&m_fade, ByteN_(m_color, 3), ByteN_(m_color, 0), ByteN_(m_color, 1), ByteN_(m_color, 2));\n"),
  "scale_bytes": v("        float scale = 1.0f / 255.0f;\n        SetFadeColor(&m_fade, Byte_(m_color, 3) * scale, Byte_(m_color, 0) * scale, Byte_(m_color, 1) * scale, Byte_(m_color, 2) * scale);\n"),
  "scale_bytes_n": v("        float scale = 1.0f / 255.0f;\n        SetFadeColor(&m_fade, ByteN_(m_color, 3) * scale, ByteN_(m_color, 0) * scale, ByteN_(m_color, 1) * scale, ByteN_(m_color, 2) * scale);\n"),
}
