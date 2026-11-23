"""Wave 41 (f1), round 4 on BuildVertices: the section pointer declared ahead of the outer counter
(retail: section r26, i r25; GC/2.7 with the section declared in the loop: the other way round)."""
SCORE = ["BuildVertices__Q36Effect3RW314EffectLocusRW3Fv"]
SEC = "        const UnkLocusSection_80439B18 *section = desc->m_sections;\n"
TOP = "    Vec3f position;\n"
PT = "    UnkLocusPoint_80439B18 *point = reinterpret_cast<UnkLocusPoint_80439B18 *>(m_head);\n"
REPL = {
    "base": [],
    "sec_top": [(TOP, TOP + "    const UnkLocusSection_80439B18 *section;\n"), (SEC, "        section = desc->m_sections;\n")],
    "sec_before_point": [(PT, "    const UnkLocusSection_80439B18 *section;\n" + PT), (SEC, "        section = desc->m_sections;\n")],
    "sec_after_point": [(PT, PT + "    const UnkLocusSection_80439B18 *section;\n"), (SEC, "        section = desc->m_sections;\n")],
}
