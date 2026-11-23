"""Wave 41: EffectParticle3DUSA::Update's colour conversion.  Retail gives blue, green, red and alpha
the stack slots from the bottom up (alpha's conversion made after the three colour arguments) but
loads alpha's word before the stores; the fade-set flags are signed bytes (extsb.)."""
SCORE = ["Update__Q412SonicteamUSA6System6Effect19EffectParticle3DUSAFf"]
OLD = """                float scale = 1.0f / 255.0f;
                float alpha = ((particle->m_color[3] >> 22) & 0xFF) * scale;
                SetFadeColor(&m_fades[i], ((particle->m_color[0] >> 22) & 0xFF) * scale,
                             ((particle->m_color[1] >> 22) & 0xFF) * scale,
                             ((particle->m_color[2] >> 22) & 0xFF) * scale);
                m_fades[i].m_color[3] = alpha;
"""
H = [("include/Game/EffectManagerPJS.h", [("bool *m_fadeSet;", "s8 *m_fadeSet;  ")])]
HS = {"Game/EffectManagerPJS.h": [("bool *m_fadeSet;", "s8 *m_fadeSet;  ")]}
NEWB = [("    m_fadeSet = new bool[m_count];", "    m_fadeSet = new s8[m_count];")]
A_INT = """                float scale = 1.0f / 255.0f;
                u32 a = (particle->m_color[3] >> 22) & 0xFF;
                SetFadeColor(&m_fades[i], ((particle->m_color[0] >> 22) & 0xFF) * scale,
                             ((particle->m_color[1] >> 22) & 0xFF) * scale,
                             ((particle->m_color[2] >> 22) & 0xFF) * scale);
                m_fades[i].m_color[3] = a * scale;
"""
A_AFTER = """                float scale = 1.0f / 255.0f;
                SetFadeColor(&m_fades[i], ((particle->m_color[0] >> 22) & 0xFF) * scale,
                             ((particle->m_color[1] >> 22) & 0xFF) * scale,
                             ((particle->m_color[2] >> 22) & 0xFF) * scale);
                m_fades[i].m_color[3] = ((particle->m_color[3] >> 22) & 0xFF) * scale;
"""
A_CONST = """                float scale = 1.0f / 255.0f;
                const u32 *color = particle->m_color;
                SetFadeColor(&m_fades[i], ((color[0] >> 22) & 0xFF) * scale,
                             ((color[1] >> 22) & 0xFF) * scale,
                             ((color[2] >> 22) & 0xFF) * scale);
                m_fades[i].m_color[3] = ((color[3] >> 22) & 0xFF) * scale;
"""
A_INT2 = """                float scale = 1.0f / 255.0f;
                u32 a = (particle->m_color[3] >> 22) & 0xFF;
                u32 r = (particle->m_color[0] >> 22) & 0xFF;
                u32 g = (particle->m_color[1] >> 22) & 0xFF;
                u32 b = (particle->m_color[2] >> 22) & 0xFF;
                SetFadeColor(&m_fades[i], r * scale, g * scale, b * scale);
                m_fades[i].m_color[3] = a * scale;
"""
REPL = {
  "s8": NEWB,
  "s8_a_int": NEWB + [(OLD, A_INT)],
  "s8_a_after": NEWB + [(OLD, A_AFTER)],
  "s8_a_const": NEWB + [(OLD, A_CONST)],
  "s8_a_int2": NEWB + [(OLD, A_INT2)],
}
HDR = {k: HS for k in REPL}

LOOPHEAD = """            UnkParticleModel_80027DC4 *model = static_cast<UnkParticleModel_80027DC4 *>(m_model);
"""
def a_int(decl_where, a_decl="u32 a = (particle->m_color[3] >> 22) & 0xFF;", use="a * scale"):
    body = """                float scale = 1.0f / 255.0f;
""" + ("                " + a_decl + "\n" if decl_where == "after_scale" else "") + """                SetFadeColor(&m_fades[i], ((particle->m_color[0] >> 22) & 0xFF) * scale,
                             ((particle->m_color[1] >> 22) & 0xFF) * scale,
                             ((particle->m_color[2] >> 22) & 0xFF) * scale);
                m_fades[i].m_color[3] = """ + use + """;
"""
    pairs = NEWB + [(OLD, body)]
    if decl_where == "before_scale":
        pairs.append(("                float scale = 1.0f / 255.0f;\n                SetFade", "                " + a_decl + "\n                float scale = 1.0f / 255.0f;\n                SetFade"))
    if decl_where == "loop_top":
        pairs.append((LOOPHEAD, "            " + a_decl + "\n" + LOOPHEAD))
    if decl_where == "model_after":
        pairs.append((LOOPHEAD, LOOPHEAD + "            " + a_decl + "\n"))
    return pairs
INL = [("inline void SetFadeColor(", "inline u32 AlphaByte(const EffectParticleExecUnit *p) { return (p->m_color[3] >> 22) & 0xFF; }\ninline u32 ColorWord(const u32 *c, int i) { return (c[i] >> 22) & 0xFF; }\ninline void SetFadeColor(")]
REPL.update({
  "a_before_scale": a_int("before_scale"),
  "a_loop_top": a_int("loop_top"),
  "a_model_after": a_int("model_after"),
  "a_word": a_int("after_scale", "u32 a = particle->m_color[3];", "((a >> 22) & 0xFF) * scale"),
  "a_word_top": a_int("loop_top", "u32 a = particle->m_color[3];", "((a >> 22) & 0xFF) * scale"),
  "a_inl": INL + a_int("after_scale", "u32 a = AlphaByte(particle);"),
  "a_inl2": INL + a_int("after_scale", "u32 a = ColorWord(particle->m_color, 3);"),
  "a_ulong": a_int("after_scale", "unsigned long a = (particle->m_color[3] >> 22) & 0xFF;"),
})
HDR = {k: HS for k in REPL}

FU = "void EffectParticle3DUSA::Update(float dt) {"
PR = [(FU, "#pragma opt_propagation off\n" + FU), ("} // namespace Effect\n} // namespace System\n} // namespace SonicteamUSA", "#pragma opt_propagation reset\n} // namespace Effect\n} // namespace System\n} // namespace SonicteamUSA")]
REPL.update({
  "prag_a_int": PR + a_int("after_scale"),
  "prag_a_before_scale": PR + a_int("before_scale"),
  "prag_a_loop_top": PR + a_int("loop_top"),
  "prag_s8": PR + NEWB,
  "prag_a_word": PR + a_int("after_scale", "u32 a = particle->m_color[3];", "((a >> 22) & 0xFF) * scale"),
})
HDR = {k: HS for k in REPL}

COND = """            UnkParticleModel_80027DC4 *model = static_cast<UnkParticleModel_80027DC4 *>(m_model);
            if (model[i].m_clump != 0 && m_fades != 0 && m_fadeSet != 0) {
"""
REPL.update({
  "nomodel": a_int("after_scale") + [(COND, """            if (static_cast<UnkParticleModel_80027DC4 *>(m_model)[i].m_clump != 0 && m_fades != 0 && m_fadeSet != 0) {
""")],
  "fadeptr": a_int("after_scale") + [("SetFadeColor(&m_fades[i], ", "SetFadeColor(m_fades + i, ")],
  "fade_local": a_int("after_scale") + [("                SetFadeColor(&m_fades[i], ", "                UnkClumpFade_80027D5C *fade = &m_fades[i];\n                SetFadeColor(fade, ")],
  "a_first_fade_local": a_int("after_scale") + [("                SetFadeColor(&m_fades[i], ", "                UnkClumpFade_80027D5C *fade = &m_fades[i];\n                SetFadeColor(fade, ")],
  "a_scale_swap": a_int("after_scale") + [("                float scale = 1.0f / 255.0f;\n                u32 a = (particle->m_color[3] >> 22) & 0xFF;\n", "                u32 a = (particle->m_color[3] >> 22) & 0xFF;\n                float scale = 1.0f / 255.0f;\n")],
  "a_s32": a_int("after_scale", "s32 a = (particle->m_color[3] >> 22) & 0xFF;"),
})
HDR = {k: HS for k in REPL}

FL = ("                SetFadeColor(&m_fades[i], ", "                UnkClumpFade_80027D5C *fade = &m_fades[i];\n                SetFadeColor(fade, ")
def fl_before_a(pairs):
    return pairs + [("                float scale = 1.0f / 255.0f;\n                u32 a = (particle->m_color[3] >> 22) & 0xFF;\n                SetFadeColor(&m_fades[i], ",
                     "                float scale = 1.0f / 255.0f;\n                UnkClumpFade_80027D5C *fade = &m_fades[i];\n                u32 a = (particle->m_color[3] >> 22) & 0xFF;\n                SetFadeColor(fade, ")]
REPL.update({
  "fl_a_before_scale": a_int("before_scale") + [FL],
  "fl_a_loop_top": a_int("loop_top") + [FL],
  "fl_before_a": fl_before_a(a_int("after_scale")),
  "fl_top_a": a_int("after_scale") + [FL[::-1] if False else ("                float scale = 1.0f / 255.0f;\n                u32 a", "                UnkClumpFade_80027D5C *fade = &m_fades[i];\n                float scale = 1.0f / 255.0f;\n                u32 a"), ("SetFadeColor(&m_fades[i], ", "SetFadeColor(fade, ")],
  "fl_clump_local": a_int("after_scale") + [FL, (COND, """            UnkParticleModel_80027DC4 *model = static_cast<UnkParticleModel_80027DC4 *>(m_model);
            void **clump = &model[i].m_clump;
            if (*clump != 0 && m_fades != 0 && m_fadeSet != 0) {
""")],
  "fl_fade_alpha": a_int("after_scale", use="a * scale") + [FL, ("                m_fades[i].m_color[3] = a * scale;", "                fade->m_color[3] = a * scale;")],
})
HDR = {k: HS for k in REPL}

def a_decl_top(clump_local, a_where="after_scale"):
    pairs = a_int(a_where, a_decl="a = (particle->m_color[3] >> 22) & 0xFF;") + [FL]
    if clump_local:
        pairs.append((COND, """            UnkParticleModel_80027DC4 *model = static_cast<UnkParticleModel_80027DC4 *>(m_model);
            void **clump = &model[i].m_clump;
            if (*clump != 0 && m_fades != 0 && m_fadeSet != 0) {
"""))
    pairs.append((LOOPHEAD, "            u32 a;\n" + LOOPHEAD))
    return pairs
REPL.update({
  "adecl_top": a_decl_top(False),
  "adecl_top_clump": a_decl_top(True),
  "adecl_top_bs": a_decl_top(False, "before_scale"),
  "adecl_top_clump_bs": a_decl_top(True, "before_scale"),
})
HDR = {k: HS for k in REPL}

REPL.update({
  "prag_fl": PR + a_int("after_scale") + [FL],
  "prag_fl_top": PR + a_decl_top(False),
  "const_a_fl": a_int("after_scale", "const u32 a = (particle->m_color[3] >> 22) & 0xFF;") + [FL],
  "fl_const_fade": a_int("after_scale") + [("                SetFadeColor(&m_fades[i], ", "                UnkClumpFade_80027D5C *const fade = &m_fades[i];\n                SetFadeColor(fade, ")],
  "fl_a_int_tmpcol": a_int("after_scale", "u32 a = particle->m_color[3] >> 22 & 0xFF;") + [FL],
})
HDR = {k: HS for k in REPL}

FTOP = "    EffectParticle3DRW3::Update(dt);\n"
def a_fn_top(fl=True, prag=False, where="after_scale"):
    pairs = a_int(where, a_decl="a = (particle->m_color[3] >> 22) & 0xFF;")
    if fl:
        pairs = pairs + [FL]
    pairs.append((FTOP, "    u32 a;\n" + FTOP))
    if prag:
        pairs = PR + pairs
    return pairs
REPL.update({
  "afn_fl": a_fn_top(True),
  "afn": a_fn_top(False),
  "afn_fl_prag": a_fn_top(True, True),
  "afn_prag": a_fn_top(False, True),
})
HDR = {k: HS for k in REPL}

INL2 = [("inline void SetFadeColor(", "inline float Scaled(u32 v, float s) { return v * s; }\ninline void SetFadeRGB(UnkClumpFade_80027D5C *fade, u32 r, u32 g, u32 b, float s) {\n    fade->m_color[0] = r * s;\n    fade->m_color[1] = g * s;\n    fade->m_color[2] = b * s;\n}\ninline void SetFadeColor(")]
REPL.update({
  "scaled_a": INL2 + a_int("after_scale", use="Scaled(a, scale)") + [FL],
  "rgb_ints": INL2 + [(OLD, """                float scale = 1.0f / 255.0f;
                u32 a = (particle->m_color[3] >> 22) & 0xFF;
                UnkClumpFade_80027D5C *fade = &m_fades[i];
                SetFadeRGB(fade, (particle->m_color[0] >> 22) & 0xFF, (particle->m_color[1] >> 22) & 0xFF,
                           (particle->m_color[2] >> 22) & 0xFF, scale);
                m_fades[i].m_color[3] = a * scale;
""")] + NEWB,
})
HDR = {k: HS for k in REPL}
