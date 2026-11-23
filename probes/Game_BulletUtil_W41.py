"""Wave 41: fn_8005B2F0 with copy propagation off and named copies (the wave-40 lever).  Retail's
saved registers: to r31, from r30, the result r29, the object r28, the two hoisted addresses,
kind/node r25 and player/contacts r24."""
SCORE = ["fn_8005B2F0"]
A = 'extern "C" HitResult fn_8005B2F0(int kind, const Vec3f *from, const Vec3f *to, s32 player) {\n'
END = "/* What a bullet moving from from to to hits first, with its attack collision (other units call\n * it by its address). */\nextern \"C\" HitResult fn_8005AFE4"
P = [(A, "#pragma opt_propagation off\n" + A), (END, "#pragma opt_propagation reset\n" + END)]
C = "    UnkColliContacts_8001EE04 *contacts = fn_80021FE4(fn_80020D40(), &line, 3, kind, LineFilter, &filter);\n    if (contacts != 0) {\n"
C2 = "    UnkColliContacts_8001EE04 *contacts = LineContacts(&line, kind, &filter);\n    if (contacts != 0) {\n"
INL = "inline UnkColliContacts_8001EE04 *LineContacts(UnkLine_80020A20 *line, int kind, UnkLineFilter_8005B5A0 *filter) {\n    return fn_80021FE4(fn_80020D40(), line, 3, kind, LineFilter, filter);\n}\n\n"
N = "            UnkColliContactNode_800587A0 *node = it.m_node;\n            void *object = node->m_object;\n"
TF = "AssignVector((Vec3f &)result.field_0x18, ((VectorSubtractFn)fn_8040CD74)(to, from));"
def cp(order):
    decl = "".join({"to": "    const Vec3f *end = to;\n", "from": "    const Vec3f *start = from;\n"}[o] for o in order)
    return [(A, A + decl), ("    fn_80020A20(&line, from, to);\n", "    fn_80020A20(&line, start, end);\n"),
            (TF, "AssignVector((Vec3f &)result.field_0x18, ((VectorSubtractFn)fn_8040CD74)(end, start));")]
REPL = {
  "prag": P,
  "prag_inl": P + [(A, INL + A), (C, C2)],
  "prag_cp_tf": P + cp(["to", "from"]),
  "prag_cp_ft": P + cp(["from", "to"]),
  "prag_cp_tf_inl": P + [(A, INL + A), (C, C2)] + cp(["to", "from"]),
  "prag_cp_ft_inl": P + [(A, INL + A), (C, C2)] + cp(["from", "to"]),
  "prag_obj_top_inl": P + [(A, INL + A), (C, C2), (N, "            node = it.m_node;\n            object = node->m_object;\n"), ("    UnkLine_80020A20 line;\n    fn_80020A20(&line, from, to);\n    UnkLineFilter_8005B5A0 filter;\n    filter.m_player = player;\n", "    void *object;\n    UnkColliContactNode_800587A0 *node;\n    UnkLine_80020A20 line;\n    fn_80020A20(&line, from, to);\n    UnkLineFilter_8005B5A0 filter;\n    filter.m_player = player;\n")],
  "prag_cp_tf_obj_inl": P + [(A, INL + A), (C, C2), (N, "            node = it.m_node;\n            object = node->m_object;\n"), ("    UnkLine_80020A20 line;\n    fn_80020A20(&line, from, to);\n", "    void *object;\n    UnkColliContactNode_800587A0 *node;\n    UnkLine_80020A20 line;\n    fn_80020A20(&line, from, to);\n")] + cp(["to", "from"]),
}

def ccp(order):
    decl = "".join({"to": "    const Vec3f *const end = to;\n", "from": "    const Vec3f *const start = from;\n"}[o] for o in order)
    return [(A, A + decl), ("    fn_80020A20(&line, from, to);\n", "    fn_80020A20(&line, start, end);\n"),
            (TF, "AssignVector((Vec3f &)result.field_0x18, ((VectorSubtractFn)fn_8040CD74)(end, start));")]
REPL.update({
  "ccp_tf": ccp(["to", "from"]),
  "ccp_ft": ccp(["from", "to"]),
  "ccp_tf_inl": [(A, INL + A), (C, C2)] + ccp(["to", "from"]),
  "ccp_ft_inl": [(A, INL + A), (C, C2)] + ccp(["from", "to"]),
})

LINE = "    UnkLine_80020A20 line;\n    fn_80020A20(&line, start, end);\n"
NN = "            node = it.m_node;\n            object = node->m_object;\n"
def top(decls):
    return [(A, INL + A), (C, C2)] + ccp(["to", "from"]) + [(N, NN), (LINE, decls + LINE)]
REPL.update({
  "t_obj_node": top("    void *object;\n    UnkColliContactNode_800587A0 *node;\n"),
  "t_node_obj": top("    UnkColliContactNode_800587A0 *node;\n    void *object;\n"),
  "t_obj_only": [(A, INL + A), (C, C2)] + ccp(["to", "from"]) + [(N, "            UnkColliContactNode_800587A0 *node = it.m_node;\n            object = node->m_object;\n"), (LINE, "    void *object;\n" + LINE)],
  "t_contacts_first": [(A, INL + A)] + ccp(["to", "from"]) + [(C, "    contacts = LineContacts(&line, kind, &filter);\n    if (contacts != 0) {\n"), (LINE, "    UnkColliContacts_8001EE04 *contacts;\n" + LINE)],
  "t_obj_contacts": [(A, INL + A)] + ccp(["to", "from"]) + [(C, "    contacts = LineContacts(&line, kind, &filter);\n    if (contacts != 0) {\n"), (N, "            UnkColliContactNode_800587A0 *node = it.m_node;\n            object = node->m_object;\n"), (LINE, "    void *object;\n    UnkColliContacts_8001EE04 *contacts;\n" + LINE)],
  "t_obj_node_contacts": [(A, INL + A)] + ccp(["to", "from"]) + [(C, "    contacts = LineContacts(&line, kind, &filter);\n    if (contacts != 0) {\n"), (N, NN), (LINE, "    void *object;\n    UnkColliContactNode_800587A0 *node;\n    UnkColliContacts_8001EE04 *contacts;\n" + LINE)],
})

for k in ["t_obj_node", "t_node_obj", "t_obj_only", "t_contacts_first", "t_obj_contacts", "t_obj_node_contacts", "ccp_tf_inl"]:
    REPL["p" + k] = P + REPL[k]

CD = "    if (UnkColliContacts_8001EE04 *contacts = fn_80021FE4(fn_80020D40(), &line, 3, kind, LineFilter, &filter)) {\n"
CD2 = "    if (UnkColliContacts_8001EE04 *contacts = LineContacts(&line, kind, &filter)) {\n"
REPL.update({
  "cd": [(C, CD)],
  "cd_ccp": [(C, CD)] + ccp(["to", "from"]),
  "cd_ccp_inl": [(A, INL + A), (C, CD2)] + ccp(["to", "from"]),
})
