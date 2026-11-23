"""Wave 41 (f1): EffectLocusRW3::BuildVertices.  The frame was 0x30 bytes larger than retail's: the
inverse matrix was copied out of the call's return slot (a 48-byte struct copy); retail uses the
returned matrix in place."""
SCORE = ["BuildVertices__Q36Effect3RW314EffectLocusRW3Fv"]
INV = """                Mtx34f inverse = fn_8040BEF8(&matrix);
                Vec3f direction;
                AssignVector(direction, fn_8040B67C(&inverse, &cameraPos));
"""
REPL = {
    "base": [],
    "inv_tmp": [(INV, """                Vec3f direction;
                AssignVector(direction,
                             fn_8040B67C(&(const Mtx34f &)fn_8040BEF8(&matrix), &cameraPos));
""")],
    "inv_tmp2": [(INV, """                Vec3f direction;
                AssignVector(direction, fn_8040B67C(&fn_8040BEF8(&matrix), &cameraPos));
""")],
    "inv_ctor": [(INV, """                Mtx34f inverse(fn_8040BEF8(&matrix));
                Vec3f direction;
                AssignVector(direction, fn_8040B67C(&inverse, &cameraPos));
""")],
}

INVT = REPL["inv_tmp"]
TR = """        bool transform;
        if ((m_config & 0x800) || (m_config & 0x4000) ||
            i + m_removedCount > m_pointCount - 1 - m_addedCount) {
            transform = true;
            if (m_config & 0x800) {
"""
TR_END = """        } else {
            transform = false;
        }
        const UnkLocusSection_80439B18 *section = desc->m_sections;
"""
# the false block first: if (!(...)) { transform = false; } else { ... }
REPL["inv_neg"] = INVT + [(TR, """        bool transform;
        s32 world = m_config & 0x800;
        if (!(world || (m_config & 0x4000) ||
              i + m_removedCount > m_pointCount - 1 - m_addedCount)) {
            transform = false;
        } else {
            transform = true;
            if (world) {
"""), (TR_END, """        }
        const UnkLocusSection_80439B18 *section = desc->m_sections;
""")]
REPL["inv_neg2"] = INVT + [(TR, """        bool transform;
        if (!((m_config & 0x800) || (m_config & 0x4000) ||
              i + m_removedCount > m_pointCount - 1 - m_addedCount)) {
            transform = false;
        } else {
            transform = true;
            if (m_config & 0x800) {
"""), (TR_END, """        }
        const UnkLocusSection_80439B18 *section = desc->m_sections;
""")]
REPL["inv_world"] = INVT + [(TR, """        bool transform;
        s32 world = m_config & 0x800;
        if (world || (m_config & 0x4000) ||
            i + m_removedCount > m_pointCount - 1 - m_addedCount) {
            transform = true;
            if (world) {
""")]
REPL["inv_neg_and"] = INVT + [(TR, """        bool transform;
        s32 world = m_config & 0x800;
        if (!world && !(m_config & 0x4000) &&
            i + m_removedCount <= m_pointCount - 1 - m_addedCount) {
            transform = false;
        } else {
            transform = true;
            if (world) {
"""), (TR_END, """        }
        const UnkLocusSection_80439B18 *section = desc->m_sections;
""")]
