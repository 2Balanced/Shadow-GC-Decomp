"""Impl::Update of Game/EffLocusMultiVtx.cpp (wave 39, second pass): retail keeps the point count in
r29 and the destination in r28."""
U = """        int pointCount = m_history.m_pointCount;
        if (m_matrix != 0) {
            for (int i = 0; i < pointCount; i++) {
                fn_800091D0(&m_transformed[i],
                            &(const Vec3f &)fn_8040B67C(m_matrix, &m_points[i]));
            }
"""
def b(decl, body, pre=""):
    return [(U, decl + "        if (m_matrix != 0) {\n" + pre + "            for (int i = 0; i < pointCount; i++) {\n" + body + "            }\n")]
ORIG = "                fn_800091D0(&m_transformed[i],\n                            &(const Vec3f &)fn_8040B67C(m_matrix, &m_points[i]));\n"
REPL = {
 "getter": b("        int pointCount = m_history.PointCount();\n", ORIG),
 "getter_dest": b("        int pointCount = m_history.PointCount();\n", "                Vec3f *dest = &m_transformed[i];\n                fn_800091D0(dest, &(const Vec3f &)fn_8040B67C(m_matrix, &m_points[i]));\n"),
 "getter_cond": [(U, U.replace("        int pointCount = m_history.m_pointCount;\n", "").replace("i < pointCount", "i < m_history.PointCount()"))],
 "member_cond": [(U, U.replace("        int pointCount = m_history.m_pointCount;\n", "").replace("i < pointCount", "i < m_history.m_pointCount"))],
 "getter_in": [(U, "        if (m_matrix != 0) {\n            int pointCount = m_history.PointCount();\n            for (int i = 0; i < pointCount; i++) {\n" + ORIG + "            }\n")],
 "i_first": [(U, "        int i;\n        int pointCount = m_history.m_pointCount;\n        if (m_matrix != 0) {\n            for (i = 0; i < pointCount; i++) {\n" + ORIG + "            }\n")],
 "tmp_vec": b("        int pointCount = m_history.m_pointCount;\n", "                Vec3f moved = fn_8040B67C(m_matrix, &m_points[i]);\n                fn_800091D0(&m_transformed[i], &moved);\n"),
 "getter_tmp": b("        int pointCount = m_history.PointCount();\n", "                Vec3f moved = fn_8040B67C(m_matrix, &m_points[i]);\n                fn_800091D0(&m_transformed[i], &moved);\n"),
}
SCORE = ["Update__Q34Test16EffLocusMultiVtx4ImplFv"]
