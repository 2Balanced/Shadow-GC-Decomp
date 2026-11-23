"""Wave 41 (f1): EffectParticle3DUSA::Update, the last 5 rows (retail: alpha byte r7, the clump's
offset r8; GC/2.7 the other way round).  `#pragma opt_propagation off` scoped to the function with a
named local copy (the wave 41 lever), and the shape-B spellings of the colour block."""
SCORE = ["Update__Q412SonicteamUSA6System6Effect19EffectParticle3DUSAFf"]
FU = "void EffectParticle3DUSA::Update(float dt) {\n"
ENDF = "\n} // namespace Effect\n} // namespace System\n} // namespace SonicteamUSA"
PR = [(FU, "#pragma opt_propagation off\n" + FU), (ENDF, "\n#pragma opt_propagation reset" + ENDF)]
BLOCK = """                float scale = 1.0f / 255.0f;
                /* The alpha byte is read first and converted after the colour (retail's stack
                 * slots: blue, green, red, then alpha from the bottom up). */
                u32 alpha = (particle->m_color[3] >> 22) & 0xFF;
                UnkClumpFade_80027D5C *fade = &m_fades[i];
                SetFadeColor(fade, ((particle->m_color[0] >> 22) & 0xFF) * scale,
                             ((particle->m_color[1] >> 22) & 0xFF) * scale,
                             ((particle->m_color[2] >> 22) & 0xFF) * scale);
                m_fades[i].m_color[3] = alpha * scale;
"""
COND = """            UnkParticleModel_80027DC4 *model = static_cast<UnkParticleModel_80027DC4 *>(m_model);
            if (model[i].m_clump != 0 && m_fades != 0 && m_fadeSet != 0) {
"""
LOOP = "        EffectParticleExecUnit *particle = m_particles;\n        for (int i = 0; i < m_count; i++, particle = particle->m_next) {\n"
def blk(old, new):
    return [(BLOCK, BLOCK.replace(old, new))]
REPL = {
    "base": [],
    "prag": PR,
    "prag_acopy": PR + blk("m_fades[i].m_color[3] = alpha * scale;", "u32 a = alpha;\n                m_fades[i].m_color[3] = a * scale;"),
    "prag_acopy_top": PR + blk("u32 alpha = (particle->m_color[3] >> 22) & 0xFF;\n", "u32 alpha = (particle->m_color[3] >> 22) & 0xFF;\n                u32 a = alpha;\n").__add__(blk("m_fades[i].m_color[3] = alpha * scale;", "m_fades[i].m_color[3] = a * scale;")) if False else PR + [(BLOCK, BLOCK.replace("u32 alpha = (particle->m_color[3] >> 22) & 0xFF;\n", "u32 alpha = (particle->m_color[3] >> 22) & 0xFF;\n                u32 a = alpha;\n").replace("m_fades[i].m_color[3] = alpha * scale;", "m_fades[i].m_color[3] = a * scale;"))],
    "prag_icopy": PR + [(COND, COND.replace("            if (model[i].m_clump", "            int n = i;\n            if (model[n].m_clump"))],
    "prag_pcopy": PR + [(BLOCK, BLOCK.replace("particle->", "p->").replace("                float scale = 1.0f / 255.0f;\n", "                float scale = 1.0f / 255.0f;\n                EffectParticleExecUnit *p = particle;\n"))],
    "prag_mcopy": PR + [(COND, COND.replace("            if (model[i].m_clump != 0", "            UnkParticleModel_80027DC4 *m = model;\n            if (m[i].m_clump != 0"))],
    "prag_word": PR + [(BLOCK, BLOCK.replace("u32 alpha = (particle->m_color[3] >> 22) & 0xFF;", "u32 word = particle->m_color[3];\n                u32 alpha = (word >> 22) & 0xFF;"))],
    "prag_clump": PR + [(COND, """            UnkParticleModel_80027DC4 *model = static_cast<UnkParticleModel_80027DC4 *>(m_model);
            void *clump = model[i].m_clump;
            if (clump != 0 && m_fades != 0 && m_fadeSet != 0) {
""")],
    "prag_fcopy": PR + [(BLOCK, BLOCK.replace("m_fades[i].m_color[3] = alpha * scale;", "float fa = alpha * scale;\n                m_fades[i].m_color[3] = fa;"))],
    "acopy": blk("m_fades[i].m_color[3] = alpha * scale;", "u32 a = alpha;\n                m_fades[i].m_color[3] = a * scale;"),
    "prag_scale_copy": PR + [(BLOCK, BLOCK.replace("m_fades[i].m_color[3] = alpha * scale;", "float s = scale;\n                m_fades[i].m_color[3] = alpha * s;"))],
}

A0 = "u32 alpha = (particle->m_color[3] >> 22) & 0xFF;"
REPL.update({
    "a_u8": blk(A0, "u8 alpha = particle->m_color[3] >> 22;"),
    "a_u8b": blk(A0, "u8 alpha = (particle->m_color[3] >> 22) & 0xFF;"),
    "a_int": blk(A0, "int alpha = (particle->m_color[3] >> 22) & 0xFF;"),
    "a_u16": blk(A0, "u16 alpha = (particle->m_color[3] >> 22) & 0xFF;"),
    "a_sum": [(BLOCK, BLOCK.replace("                SetFadeColor(fade, ((particle->m_color[0] >> 22) & 0xFF) * scale,\n                             ((particle->m_color[1] >> 22) & 0xFF) * scale,\n                             ((particle->m_color[2] >> 22) & 0xFF) * scale);\n",
        "                u32 red = (particle->m_color[0] >> 22) & 0xFF;\n                SetFadeColor(fade, red * scale,\n                             ((particle->m_color[1] >> 22) & 0xFF) * scale,\n                             ((particle->m_color[2] >> 22) & 0xFF) * scale);\n"))],
    "a_blue": [(BLOCK, BLOCK.replace("                SetFadeColor(fade, ((particle->m_color[0] >> 22) & 0xFF) * scale,\n                             ((particle->m_color[1] >> 22) & 0xFF) * scale,\n                             ((particle->m_color[2] >> 22) & 0xFF) * scale);\n",
        "                u32 blue = (particle->m_color[2] >> 22) & 0xFF;\n                SetFadeColor(fade, ((particle->m_color[0] >> 22) & 0xFF) * scale,\n                             ((particle->m_color[1] >> 22) & 0xFF) * scale, blue * scale);\n"))],
    "fade_first": [(BLOCK, BLOCK.replace("                u32 alpha = (particle->m_color[3] >> 22) & 0xFF;\n                UnkClumpFade_80027D5C *fade = &m_fades[i];\n", "                UnkClumpFade_80027D5C *fade = &m_fades[i];\n                u32 alpha = (particle->m_color[3] >> 22) & 0xFF;\n"))],
    "nofade": [(BLOCK, BLOCK.replace("                UnkClumpFade_80027D5C *fade = &m_fades[i];\n                SetFadeColor(fade, ", "                SetFadeColor(&m_fades[i], "))],
})
