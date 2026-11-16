"""Impl::Draw: the vertex colour (wave 39, second pass). Retail loads a, g, b, r."""
C = """                RwIm3DVertexSetRGBA(vertex, m_color.m_r, m_color.m_g, m_color.m_b,
                                    (int)(m_color.m_a * alpha));
"""
A = "void EffLocusMultiVtx::Impl::Draw() {\n"
GET = "    u8 R() const {\n        return m_r;\n    }\n    u8 G() const {\n        return m_g;\n    }\n    u8 B() const {\n        return m_b;\n    }\n    u8 A() const {\n        return m_a;\n    }\n"
HG = {"Game/EffLocusMultiVtx.h": [("    /* 0x0 */ u8 m_a;\n", GET + "    /* 0x0 */ u8 m_a;\n")]}
REPL = {
 "getters": [(C, "                RwIm3DVertexSetRGBA(vertex, m_color.R(), m_color.G(), m_color.B(),\n                                    (int)(m_color.A() * alpha));\n")],
 "get_gb": [(C, "                RwIm3DVertexSetRGBA(vertex, m_color.m_r, m_color.G(), m_color.B(),\n                                    (int)(m_color.m_a * alpha));\n")],
 "get_r": [(C, "                RwIm3DVertexSetRGBA(vertex, m_color.R(), m_color.m_g, m_color.m_b,\n                                    (int)(m_color.m_a * alpha));\n")],
 "get_g": [(C, "                RwIm3DVertexSetRGBA(vertex, m_color.m_r, m_color.G(), m_color.m_b,\n                                    (int)(m_color.m_a * alpha));\n")],
 "inl_color": [(A, "inline void SetColor(RwIm3DVertex *vert, const UnkColor_8040D2EC &c, float alpha) {\n    RwIm3DVertexSetRGBA(vert, c.m_r, c.m_g, c.m_b, (int)(c.m_a * alpha));\n}\n\n" + A),
               (C, "                SetColor(vertex, m_color, alpha);\n")],
 "inl_color_d": [(A, "inline void SetColor(RwIm3DVertex *vert, const UnkColor_8040D2EC &c, float alpha) {\n    vert->color[0] = c.m_r;\n    vert->color[1] = c.m_g;\n    vert->color[2] = c.m_b;\n    vert->color[3] = (int)(c.m_a * alpha);\n}\n\n" + A),
               (C, "                SetColor(vertex, m_color, alpha);\n")],
 "a_local": [(C, "                u8 a = (int)(m_color.m_a * alpha);\n                RwIm3DVertexSetRGBA(vertex, m_color.m_r, m_color.m_g, m_color.m_b, a);\n")],
 "r_local": [(C, "                u8 r = m_color.m_r;\n                RwIm3DVertexSetRGBA(vertex, r, m_color.m_g, m_color.m_b,\n                                    (int)(m_color.m_a * alpha));\n")],
 "int_args": [(A, "inline void SetRGBAi(RwIm3DVertex *vert, int r, int g, int b, int a) {\n    vert->color[0] = r;\n    vert->color[1] = g;\n    vert->color[2] = b;\n    vert->color[3] = a;\n}\n\n" + A),
               (C, "                SetRGBAi(vertex, m_color.m_r, m_color.m_g, m_color.m_b,\n                                    (int)(m_color.m_a * alpha));\n")],
}
HDR = {k: HG for k in ("getters", "get_gb", "get_r", "get_g")}
SCORE = ["Draw__Q34Test16EffLocusMultiVtx4ImplFv"]
