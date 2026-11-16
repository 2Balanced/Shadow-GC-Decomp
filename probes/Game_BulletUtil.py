"""fn_8005B2F0 of Game/BulletUtil.cpp (wave 39, second pass): retail keeps the contact list in the
lowest saved register (r24, the player's) and the contact's object in r28."""
A = 'extern "C" HitResult fn_8005B2F0(int kind, const Vec3f *from, const Vec3f *to, s32 player) {\n'
C = "    UnkColliContacts_8001EE04 *contacts = fn_80021FE4(fn_80020D40(), &line, 3, kind, LineFilter, &filter);\n    if (contacts != 0) {\n"
N = "            UnkColliContactNode_800587A0 *node = it.m_node;\n            void *object = node->m_object;\n"
INL = "inline UnkColliContacts_8001EE04 *LineContacts(UnkLine_80020A20 *line, int kind, UnkLineFilter_8005B5A0 *filter) {\n    return fn_80021FE4(fn_80020D40(), line, 3, kind, LineFilter, filter);\n}\n\n"
OBJ = "inline void *ObjectOf(UnkColliContactNode_800587A0 *node) {\n    return node->m_object;\n}\n\n"
REPL = {
 "inl_contacts": [(A, INL + A), (C, "    UnkColliContacts_8001EE04 *contacts = LineContacts(&line, kind, &filter);\n    if (contacts != 0) {\n")],
 "if_decl": [(C, "    if (UnkColliContacts_8001EE04 *contacts = fn_80021FE4(fn_80020D40(), &line, 3, kind, LineFilter, &filter)) {\n")],
 "inl_obj": [(A, OBJ + A), (N, "            UnkColliContactNode_800587A0 *node = it.m_node;\n            void *object = ObjectOf(node);\n")],
 "inl_both": [(A, INL + OBJ + A), (C, "    UnkColliContacts_8001EE04 *contacts = LineContacts(&line, kind, &filter);\n    if (contacts != 0) {\n"), (N, "            UnkColliContactNode_800587A0 *node = it.m_node;\n            void *object = ObjectOf(node);\n")],
 "obj_first": [(N, "            void *object = it.m_node->m_object;\n            UnkColliContactNode_800587A0 *node = it.m_node;\n")],
 "no_node": [(N, "            void *object = it.m_node->m_object;\n"), ("            result.field_0x2C = fn_80026588(node) & 0x0FFFFFFF;\n            result.field_0x30 = fn_80026588(node) >> 28;\n            if (fn_800C3ED4(fn_80021B08(), node)) {", "            result.field_0x2C = fn_80026588(it.m_node) & 0x0FFFFFFF;\n            result.field_0x30 = fn_80026588(it.m_node) >> 28;\n            if (fn_800C3ED4(fn_80021B08(), it.m_node)) {")],
 "decl_top": [(C, "    UnkColliContacts_8001EE04 *contacts;\n    contacts = fn_80021FE4(fn_80020D40(), &line, 3, kind, LineFilter, &filter);\n    if (contacts != 0) {\n"), ("    UnkLine_80020A20 line;\n    fn_80020A20(&line, from, to);\n    UnkLineFilter_8005B5A0 filter;\n    filter.m_player = player;\n    UnkColliContacts_8001EE04 *contacts;", "    void *object;\n    UnkLine_80020A20 line;\n    fn_80020A20(&line, from, to);\n    UnkLineFilter_8005B5A0 filter;\n    filter.m_player = player;\n    UnkColliContacts_8001EE04 *contacts;"), (N, "            UnkColliContactNode_800587A0 *node = it.m_node;\n            object = node->m_object;\n")],
}
SCORE = ["fn_8005B2F0"]
