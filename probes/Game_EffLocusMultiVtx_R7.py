"""Impl::Draw: parameter orders of the inline colour setter (wave 39, second pass). Retail loads
a, g, b, r; arguments are evaluated right to left."""
import itertools
C = """                RwIm3DVertexSetRGBA(vertex, m_color.m_r, m_color.m_g, m_color.m_b,
                                    (int)(m_color.m_a * alpha));
"""
A = "void EffLocusMultiVtx::Impl::Draw() {\n"
ARG = {"r": "m_color.m_r", "g": "m_color.m_g", "b": "m_color.m_b", "a": "(int)(m_color.m_a * alpha)"}
IDX = {"r": 0, "g": 1, "b": 2, "a": 3}
REPL = {}
for perm in ["rbga", "grba", "gbra", "brga", "bgra", "argb", "arbg", "agbr", "rgab", "rbag"]:
    fn = "inline void SetC(RwIm3DVertex *vert, " + ", ".join("u8 " + k for k in perm) + ") {\n" + "".join("    vert->color[%d] = %s;\n" % (IDX[k], k) for k in "rgba") + "}\n\n"
    REPL["p_" + perm] = [(A, fn + A), (C, "                SetC(vertex, " + ", ".join(ARG[k] for k in perm) + ");\n")]
SCORE = ["Draw__Q34Test16EffLocusMultiVtx4ImplFv"]
