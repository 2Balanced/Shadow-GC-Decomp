"""fn_8005B2F0 of Game/BulletUtil.cpp (wave 39, second pass): the contact's object defined before
the contact list."""
A = 'extern "C" HitResult fn_8005B2F0(int kind, const Vec3f *from, const Vec3f *to, s32 player) {\n'
C = "    UnkColliContacts_8001EE04 *contacts = fn_80021FE4(fn_80020D40(), &line, 3, kind, LineFilter, &filter);\n    if (contacts != 0) {\n"
C2 = "    UnkColliContacts_8001EE04 *contacts = LineContacts(&line, kind, &filter);\n    if (contacts != 0) {\n"
N = "            UnkColliContactNode_800587A0 *node = it.m_node;\n            void *object = node->m_object;\n"
N2 = "            UnkColliContactNode_800587A0 *node = it.m_node;\n            object = node->m_object;\n"
INL = "inline UnkColliContacts_8001EE04 *LineContacts(UnkLine_80020A20 *line, int kind, UnkLineFilter_8005B5A0 *filter) {\n    return fn_80021FE4(fn_80020D40(), line, 3, kind, LineFilter, filter);\n}\n\n"
TOP = "    UnkLine_80020A20 line;\n    fn_80020A20(&line, from, to);\n"
B = [(A, INL + A), (C, C2)]
REPL = {
 "obj0_inl": B + [(TOP, "    void *object = 0;\n" + TOP), (N, N2)],
 "obj0": [(TOP, "    void *object = 0;\n" + TOP), (N, N2)],
 "obj0_after_line": B + [(TOP, TOP + "    void *object = 0;\n"), (N, N2)],
 "inl_wide": [(A, "inline UnkColliContacts_8001EE04 *LineContacts(const Vec3f *from, const Vec3f *to, int kind, s32 player) {\n    UnkLine_80020A20 line;\n    fn_80020A20(&line, from, to);\n    UnkLineFilter_8005B5A0 filter;\n    filter.m_player = player;\n    return fn_80021FE4(fn_80020D40(), &line, 3, kind, LineFilter, &filter);\n}\n\n" + A),
              (TOP + "    UnkLineFilter_8005B5A0 filter;\n    filter.m_player = player;\n" + C, "    UnkColliContacts_8001EE04 *contacts = LineContacts(from, to, kind, player);\n    if (contacts != 0) {\n")],
}
SCORE = ["fn_8005B2F0"]
