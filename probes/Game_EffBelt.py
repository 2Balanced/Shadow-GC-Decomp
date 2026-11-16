"""Shapes for Impl::Add, Impl::Update and Impl::Draw of Game/EffBelt.cpp (wave 39, second pass)."""
GET = ("    /* 0x0 */ boost::scoped_array<BeltPoint> m_points;\n", "    int Count() const {\n        return m_count;\n    }\n    /* 0x0 */ boost::scoped_array<BeltPoint> m_points;\n")
W = "    float width = m_width;\n    AssignVector(side, Scale(fn_8040CEE8(&side), width));\n"
C1 = "                RwIm3DVertexSetRGBA(vertex, m_color.m_r, m_color.m_g, m_color.m_b,\n                                    (int)newest->m_alpha);\n"
C2 = "                RwIm3DVertexSetRGBA(vertex, m_color.m_r, m_color.m_g, m_color.m_b,\n                                    (int)point->m_alpha);\n"
SETC = ("void EffBelt::Impl::Draw() {\n", "inline void SetVertexColor(RwIm3DVertex *vert, u8 r, u8 b, u8 g, u8 a) {\n    vert->color[0] = r;\n    vert->color[1] = g;\n    vert->color[2] = b;\n    vert->color[3] = a;\n}\n\nvoid EffBelt::Impl::Draw() {\n")
COL = [SETC, (C1, "                SetVertexColor(vertex, m_color.m_r, m_color.m_b, m_color.m_g,\n                               (int)newest->m_alpha);\n"),
       (C2, "                SetVertexColor(vertex, m_color.m_r, m_color.m_b, m_color.m_g,\n                               (int)point->m_alpha);\n")]
def pos(who, form):
    old = "                float x = %s->m_position.x + side.x;\n                float y = %s->m_position.y + side.y;\n                float z = %s->m_position.z + side.z;\n                vertex->objVertex.x = x;\n                vertex->objVertex.y = y;\n                vertex->objVertex.z = z;\n" % (who, who, who)
    return (old, form.replace("WHO", who))
SP = "                RwIm3DVertexSetPos(vertex, WHO->m_position.x + side.x, WHO->m_position.y + side.y,\n                                   WHO->m_position.z + side.z);\n"
SPz = "                SetPosZ(vertex, WHO->m_position.x + side.x, WHO->m_position.z + side.z,\n                        WHO->m_position.y + side.y);\n"
SETZ = ("void EffBelt::Impl::Draw() {\n", "inline void SetPosZ(RwIm3DVertex *vert, float x, float z, float y) {\n    vert->objVertex.x = x;\n    vert->objVertex.y = y;\n    vert->objVertex.z = z;\n}\n\nvoid EffBelt::Impl::Draw() {\n")
REPL = {
 "u_getter": [GET, ("    s32 count = m_ring.m_count;\n", "    s32 count = m_ring.Count();\n")],
 "a_member": [(W, "    AssignVector(side, Scale(fn_8040CEE8(&side), m_width));\n")],
 "a_getter": [("    void Finish() {\n", "    float Width() const {\n        return m_width;\n    }\n    void Finish() {\n"), (W, "    AssignVector(side, Scale(fn_8040CEE8(&side), Width()));\n")],
 "a_mul": [(W, "    AssignVector(side, fn_8040CEE8(&side) * m_width);\n"), ("namespace Test {\n\n/* A position of the ribbon", "inline Vec3f operator*(const Vec3f &vector, float scale) {\n    return fn_8040CDA8(&vector, scale);\n}\n\nnamespace Test {\n\n/* A position of the ribbon")],
 "d_col": COL,
 "d_col_setpos": COL + [pos("newest", SP), pos("point", SP)],
 "d_col_setposz": COL + [pos("newest", SPz), pos("point", SPz), (SETC[1], SETZ[1].replace("void EffBelt::Impl::Draw() {\n", "") + SETC[1])],
}
REPL["d_col_setposz"] = [(SETC[0], SETZ[1].replace("void EffBelt::Impl::Draw() {\n", "") + SETC[1]), COL[1], COL[2], pos("newest", SPz), pos("point", SPz)]
SCORE = ["Add__Q34Test7EffBelt4ImplFRC5Vec3f", "Update__Q34Test7EffBelt4ImplFv", "Draw__Q34Test7EffBelt4ImplFv"]
