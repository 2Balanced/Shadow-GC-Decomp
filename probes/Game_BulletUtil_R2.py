"""fn_8005B2F0 of Game/BulletUtil.cpp (wave 39, second pass), on top of the inline LineContacts."""
A = 'extern "C" HitResult fn_8005B2F0(int kind, const Vec3f *from, const Vec3f *to, s32 player) {\n'
C = "    UnkColliContacts_8001EE04 *contacts = fn_80021FE4(fn_80020D40(), &line, 3, kind, LineFilter, &filter);\n    if (contacts != 0) {\n"
C2 = "    UnkColliContacts_8001EE04 *contacts = LineContacts(&line, kind, &filter);\n    if (contacts != 0) {\n"
N = "            UnkColliContactNode_800587A0 *node = it.m_node;\n            void *object = node->m_object;\n"
INL = "inline UnkColliContacts_8001EE04 *LineContacts(UnkLine_80020A20 *line, int kind, UnkLineFilter_8005B5A0 *filter) {\n    return fn_80021FE4(fn_80020D40(), line, 3, kind, LineFilter, filter);\n}\n\n"
TOP = "    UnkLine_80020A20 line;\n    fn_80020A20(&line, from, to);\n"
B = [(A, INL + A), (C, C2)]
REPL = {
 "obj_top": B + [(TOP, "    void *object;\n" + TOP), (N, "            UnkColliContactNode_800587A0 *node = it.m_node;\n            object = node->m_object;\n")],
 "obj_node_top": B + [(TOP, "    void *object;\n    UnkColliContactNode_800587A0 *node;\n" + TOP), (N, "            node = it.m_node;\n            object = node->m_object;\n")],
 "node_obj_top": B + [(TOP, "    UnkColliContactNode_800587A0 *node;\n    void *object;\n" + TOP), (N, "            node = it.m_node;\n            object = node->m_object;\n")],
 "obj_first": B + [(N, "            void *object = it.m_node->m_object;\n            UnkColliContactNode_800587A0 *node = it.m_node;\n")],
 "contacts_ref": [(A, INL.replace("inline UnkColliContacts_8001EE04 *LineContacts", "inline UnkColliContacts_8001EE04 *LineContacts") + A), (C, "    UnkColliContacts_8001EE04 *const contacts = LineContacts(&line, kind, &filter);\n    if (contacts != 0) {\n")],
 "if_inl": [(A, INL + A), (C, "    if (UnkColliContacts_8001EE04 *contacts = LineContacts(&line, kind, &filter)) {\n")],
 "result_out": B + [("        HitResult result(false);\n        for (UnkColliContactIter", "        void *object;\n        HitResult result(false);\n        for (UnkColliContactIter"), (N, "            UnkColliContactNode_800587A0 *node = it.m_node;\n            object = node->m_object;\n")],
 "obj_typed": B + [(N, "            UnkColliContactNode_800587A0 *node = it.m_node;\n            void *const object = node->m_object;\n")],
}
SCORE = ["fn_8005B2F0"]
