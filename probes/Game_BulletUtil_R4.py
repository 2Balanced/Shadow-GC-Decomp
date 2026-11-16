"""fn_8005B2F0 of Game/BulletUtil.cpp (wave 39, second pass): the node and its object through inline
functions of the iterator, with and without the inline LineContacts."""
A = 'extern "C" HitResult fn_8005B2F0(int kind, const Vec3f *from, const Vec3f *to, s32 player) {\n'
C = "    UnkColliContacts_8001EE04 *contacts = fn_80021FE4(fn_80020D40(), &line, 3, kind, LineFilter, &filter);\n    if (contacts != 0) {\n"
C2 = "    UnkColliContacts_8001EE04 *contacts = LineContacts(&line, kind, &filter);\n    if (contacts != 0) {\n"
N = "            UnkColliContactNode_800587A0 *node = it.m_node;\n            void *object = node->m_object;\n"
INL = "inline UnkColliContacts_8001EE04 *LineContacts(UnkLine_80020A20 *line, int kind, UnkLineFilter_8005B5A0 *filter) {\n    return fn_80021FE4(fn_80020D40(), line, 3, kind, LineFilter, filter);\n}\n\n"
H = "inline UnkColliContactNode_800587A0 *NodeOf(const UnkColliContactIter_8001EDFC &it) {\n    return it.m_node;\n}\ninline void *ObjectOf(const UnkColliContactIter_8001EDFC &it) {\n    return it.m_node->m_object;\n}\ninline void *ObjectOfNode(UnkColliContactNode_800587A0 *node) {\n    return node->m_object;\n}\n\n"
B = [(A, INL + H + A), (C, C2)]
B0 = [(A, H + A)]
V = {
 "node_inl": "            UnkColliContactNode_800587A0 *node = NodeOf(it);\n            void *object = node->m_object;\n",
 "obj_it": "            UnkColliContactNode_800587A0 *node = it.m_node;\n            void *object = ObjectOf(it);\n",
 "obj_it_first": "            void *object = ObjectOf(it);\n            UnkColliContactNode_800587A0 *node = it.m_node;\n",
 "both_it": "            UnkColliContactNode_800587A0 *node = NodeOf(it);\n            void *object = ObjectOf(it);\n",
 "both_node": "            UnkColliContactNode_800587A0 *node = NodeOf(it);\n            void *object = ObjectOfNode(node);\n",
}
REPL = {}
for k, v in V.items():
    REPL["c_" + k] = B + [(N, v)]
    REPL["p_" + k] = B0 + [(N, v)]
SCORE = ["fn_8005B2F0"]
