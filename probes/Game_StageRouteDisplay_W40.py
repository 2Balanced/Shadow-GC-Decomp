"""StageRouteDisplay's constructor (wave 40): the render pointer as a named local in a block, so
that its slot lies above the cursor position's."""
O = "    m_render = fn_8004E24C(fn_80046178()->field_0x54);\n"
T = "boost::shared_ptr<Render>"
REPL = {
 "blk": [(O, "    {\n        " + T + " render = fn_8004E24C(fn_80046178()->field_0x54);\n        m_render = render;\n    }\n")],
 "blk_ctor": [(O, "    {\n        " + T + " render(fn_8004E24C(fn_80046178()->field_0x54));\n        m_render = render;\n    }\n")],
 "for": [(O, "    for (int n = 0; n < 1; n++) {\n    " + O + "    }\n")],
 "cref": [(O, "    {\n        const " + T + " &render = fn_8004E24C(fn_80046178()->field_0x54);\n        m_render = render;\n    }\n")],
}
C = "        const " + T + " &render = fn_8004E24C(fn_80046178()->field_0x54);\n"
REPL.update({
 "cref_for": [(O, "    {\n" + C + "        for (int n = 0; n < 1; n++) {\n            m_render = render;\n        }\n    }\n")],
 "cref_op": [(O, "    {\n" + C + "        m_render.operator=(render);\n    }\n")],
 "cref_noblk": [(O, C[4:].replace("    const", "const") if False else "    const " + T + " &render = fn_8004E24C(fn_80046178()->field_0x54);\n    m_render = render;\n")],
 "cref_this": [(O, "    {\n" + C + "        this->m_render = render;\n    }\n")],
 "cref_ptr": [(O, "    {\n" + C + "        " + T + " *dst = &m_render;\n        *dst = render;\n    }\n")],
 "cref_forall": [(O, "    for (int n = 0; n < 1; n++) {\n" + C + "        m_render = render;\n    }\n")],
})
P = "            UnkCsdPosition_8036AE98 position = fn_8036AE98(node.Get());\n            fn_8036AE70(cursor.Get(), &position);\n"
R = "            const UnkCsdPosition_8036AE98 &position = fn_8036AE98(node.Get());\n"
REPL.update({
 "p_cref": [(P, R + "            fn_8036AE70(cursor.Get(), &position);\n")],
 "p_cref_for": [(P, R + "            for (int n = 0; n < 1; n++) {\n                fn_8036AE70(cursor.Get(), &position);\n            }\n")],
 "p_cref_ptr_for": [(P, R + "            const UnkCsdPosition_8036AE98 *where;\n            for (int n = 0; n < 1; n++) {\n                where = &position;\n            }\n            fn_8036AE70(cursor.Get(), where);\n")],
 "p_cref_cur_for": [(P, R + "            CNode *target;\n            for (int n = 0; n < 1; n++) {\n                target = cursor.Get();\n            }\n            fn_8036AE70(target, &position);\n")],
 "p_cref_cur": [(P, R + "            CNode *target = cursor.Get();\n            fn_8036AE70(target, &position);\n")],
 "p_tmp_for": [(P, "            for (int n = 0; n < 1; n++) {\n                fn_8036AE70(cursor.Get(), &(const UnkCsdPosition_8036AE98 &)fn_8036AE98(node.Get()));\n            }\n")],
 "p_for_all": [(P, "            for (int n = 0; n < 1; n++) {\n    " + P.replace("\n            fn", "\n                fn") + "            }\n")],
})
H = "StageRouteDisplay::StageRouteDisplay(int stageId) : StageBase::Base(0), m_project(0) {\n"
REPL.update({
 "inl_free": [(H, "inline void AttachRender40(boost::shared_ptr<Render> &render) {\n    render = fn_8004E24C(fn_80046178()->field_0x54);\n}\n\n" + H), (O, "    AttachRender40(m_render);\n")],
 "inl_self": [(H, "inline void AttachRender40(StageRouteDisplay *self) {\n    self->m_render = fn_8004E24C(fn_80046178()->field_0x54);\n}\n\n" + H), (O, "    AttachRender40(this);\n")],
 "inl_get": [(H, "inline boost::shared_ptr<Render> StageRender40() {\n    return fn_8004E24C(fn_80046178()->field_0x54);\n}\n\n" + H), (O, "    m_render = StageRender40();\n")],
})
X = "&(const UnkCsdPosition_8036AE98 &)fn_8036AE98(node.Get())"
REPL.update({
 "t_plain": [(P, "            fn_8036AE70(cursor.Get(), " + X + ");\n")],
 "t_cur_local": [(P, "            CNode *target = cursor.Get();\n            fn_8036AE70(target, " + X + ");\n")],
 "t_cur_for": [(P, "            CNode *target;\n            for (int n = 0; n < 1; n++) {\n                target = cursor.Get();\n            }\n            fn_8036AE70(target, " + X + ");\n")],
 "t_cur_after": [(P, "            const UnkCsdPosition_8036AE98 *where = " + X + ";\n            fn_8036AE70(cursor.Get(), where);\n")],
 "t_where_for": [(P, "            const UnkCsdPosition_8036AE98 *where;\n            for (int n = 0; n < 1; n++) {\n                where = " + X + ";\n            }\n            fn_8036AE70(cursor.Get(), where);\n")],
})
S = "struct UnkCsdPosition_8036AE98 {\n    float x;\n    float y;\n};\n"
REPL.update({
 "u64": [(S, "typedef u64 UnkCsdPosition_8036AE98;\n")],
 "s64": [(S, "typedef s64 UnkCsdPosition_8036AE98;\n")],
 "two_u32": [(S, "struct UnkCsdPosition_8036AE98 {\n    u32 x;\n    u32 y;\n};\n")],
 "arr": [(S, "struct UnkCsdPosition_8036AE98 {\n    float v[2];\n};\n")],
 "double": [(S, "struct UnkCsdPosition_8036AE98 {\n    u64 v;\n};\n")],
})
SCORE = ["__ct__17StageRouteDisplayFi"]
