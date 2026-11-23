"""Wave 41 (f1), round 3 on BuildVertices: `#pragma opt_propagation off` scoped to the function, alone
and with named local copies (the wave 41 lever)."""
SCORE = ["BuildVertices__Q36Effect3RW314EffectLocusRW3Fv"]
FU = "void EffectLocusRW3::BuildVertices() {\n"
ENDF = "\n} // namespace RW3\n"
PR = [(FU, "#pragma opt_propagation off\n" + FU), (ENDF, "\n#pragma opt_propagation reset\n} // namespace RW3\n")]
LOOPI = "    for (int i = 0; point != 0; i++) {\n"
SEC = "        const UnkLocusSection_80439B18 *section = desc->m_sections;\n"
REPL = {
    "base": [],
    "prag": PR,
    "prag_icopy": PR + [(SEC, "        int n = i;\n" + SEC)],
    "prag_desc_copy": PR + [(SEC, "        const UnkLocusDesc_80439B18 *d = desc;\n        const UnkLocusSection_80439B18 *section = d->m_sections;\n")],
    "prag_color_copy": PR + [(SEC, "        float c0 = color[0];\n" + SEC)],
}
