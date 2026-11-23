"""Wave 41 (f1), round 2 on BuildVertices (source has inv_neg of _W41f): the stack layout.  Retail:
matrix 248, call temps 200/152, cameraPos 140, position 128, color 112, direction 100, axis 88,
normal 76, rotation 60."""
SCORE = ["BuildVertices__Q36Effect3RW314EffectLocusRW3Fv"]
TOPD = """    Mtx34f matrix;
    Vec3f cameraPos;
"""
AX = """                Vec3f axis(1.0f, 0.0f, 0.0f);
                Vec3f direction;
"""
POS = """                Vec3f position(section->m_position.x, section->m_position.y,
                               section->m_position.z);
"""
POS_ASSIGN = """                position.x = section->m_position.x;
                position.y = section->m_position.y;
                position.z = section->m_position.z;
"""
REPL = {
    "base": [],
    "pos_top": [(TOPD, TOPD + "    Vec3f position;\n"), (POS, POS_ASSIGN)],
    "dir_axis": [(AX, """                Vec3f direction;
                Vec3f axis(1.0f, 0.0f, 0.0f);
""")],
    "pos_top_dir_axis": [(TOPD, TOPD + "    Vec3f position;\n"), (POS, POS_ASSIGN), (AX, """                Vec3f direction;
                Vec3f axis(1.0f, 0.0f, 0.0f);
""")],
}

B3 = REPL["pos_top_dir_axis"]
ACOS = [("inline void CopyMatrix(", "/* The float overload of <cmath>'s acos, out of line elsewhere in the game (acos__3stdFf). */\nnamespace std {\nfloat acos(float x);\n} // namespace std\n\ninline void CopyMatrix(")]
VPOS = """                m_vertices[index].x = position.x;
                m_vertices[index].y = position.y;
                m_vertices[index].z = position.z;
"""
COL = """            m_vertices[index].r = color[0] * section->m_color[0];
            m_vertices[index].g = color[1] * section->m_color[1];
            m_vertices[index].b = color[2] * section->m_color[2];
            m_vertices[index].a = color[3] * section->m_color[3];
"""
SETTERS = [("inline void CopyMatrix(", """/* Sets a vertex's position and colour (names inferred). */
inline void SetVertexPosition(UnkLocusVertex_80439B18 &vertex, float x, float y, float z) {
    vertex.x = x;
    vertex.y = y;
    vertex.z = z;
}

inline void SetVertexColor(UnkLocusVertex_80439B18 &vertex, float r, float g, float b, float a) {
    vertex.r = r;
    vertex.g = g;
    vertex.b = b;
    vertex.a = a;
}

inline void CopyMatrix(""")]
REPL["r3_acos"] = B3 + ACOS
REPL["r3_pos"] = B3 + ACOS + SETTERS + [(VPOS, "                SetVertexPosition(m_vertices[index], position.x, position.y, position.z);\n")]
REPL["r3_col"] = B3 + ACOS + SETTERS + [(COL, """            SetVertexColor(m_vertices[index], color[0] * section->m_color[0],
                           color[1] * section->m_color[1], color[2] * section->m_color[2],
                           color[3] * section->m_color[3]);
""")]
REPL["r3_both"] = B3 + ACOS + SETTERS + [(VPOS, "                SetVertexPosition(m_vertices[index], position.x, position.y, position.z);\n"), (COL, """            SetVertexColor(m_vertices[index], color[0] * section->m_color[0],
                           color[1] * section->m_color[1], color[2] * section->m_color[2],
                           color[3] * section->m_color[3]);
""")]
