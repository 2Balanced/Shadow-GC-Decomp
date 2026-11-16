"""Shapes for Impl::Draw and Impl::Update of Game/EffLocusMultiVtx.cpp (wave 39, second pass)."""
D = """        RwIm3DVertex *vertices = m_history.Vertices();
        int count = m_history.m_count;
        int pointCount = m_history.m_pointCount;
        int vertexCount = count * 2;
"""
C = """                RwIm3DVertexSetRGBA(vertex, m_color.m_r, m_color.m_g, m_color.m_b,
                                    (int)(m_color.m_a * alpha));
"""
U = """                fn_800091D0(&m_transformed[i],
                            &(const Vec3f &)fn_8040B67C(m_matrix, &m_points[i]));
"""
V = "        RwIm3DVertex *vertices = m_history.Vertices();\n"
REPL = {
 "d_cpvV": [(D, "        int count;\n        int pointCount;\n        int vertexCount;\n" + V + "        count = m_history.m_count;\n        pointCount = m_history.m_pointCount;\n        vertexCount = count * 2;\n")],
 "d_cpVv": [(D, "        int count;\n        int pointCount;\n" + V + "        int vertexCount;\n        count = m_history.m_count;\n        pointCount = m_history.m_pointCount;\n        vertexCount = count * 2;\n")],
 "d_cVpv": [(D, "        int count;\n" + V + "        count = m_history.m_count;\n        int pointCount = m_history.m_pointCount;\n        int vertexCount = count * 2;\n")],
 "d_Vcvp": [(D, V + "        int count = m_history.m_count;\n        int vertexCount = count * 2;\n        int pointCount = m_history.m_pointCount;\n")],
 "d_cVvp": [(D, "        int count;\n" + V + "        count = m_history.m_count;\n        int vertexCount;\n        int pointCount = m_history.m_pointCount;\n        vertexCount = count * 2;\n")],
 "d_pcVv": [(D, "        int pointCount;\n        int count;\n" + V + "        count = m_history.m_count;\n        pointCount = m_history.m_pointCount;\n        int vertexCount = count * 2;\n")],
 "c_direct": [(C, "                vertex->color[0] = m_color.m_r;\n                vertex->color[1] = m_color.m_g;\n                vertex->color[2] = m_color.m_b;\n                vertex->color[3] = (int)(m_color.m_a * alpha);\n")],
 "c_locals": [(C, "                u8 g = m_color.m_g;\n                u8 b = m_color.m_b;\n                RwIm3DVertexSetRGBA(vertex, m_color.m_r, g, b,\n                                    (int)(m_color.m_a * alpha));\n")],
 "u_dest": [(U, "                Vec3f *dest = &m_transformed[i];\n                fn_800091D0(dest, &(const Vec3f &)fn_8040B67C(m_matrix, &m_points[i]));\n")],
 "u_assign": [(U, "                m_transformed[i] = fn_8040B67C(m_matrix, &m_points[i]);\n")],
 "u_cnt_in": [("        int pointCount = m_history.m_pointCount;\n        if (m_matrix != 0) {\n", "        int pointCount;\n        pointCount = m_history.m_pointCount;\n        if (m_matrix != 0) {\n")],
}
SCORE = ["Draw__Q34Test16EffLocusMultiVtx4ImplFv", "Update__Q34Test16EffLocusMultiVtx4ImplFv"]
