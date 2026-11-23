"""Wave 41: the sweep fn_801BFFD8 with copy propagation off (retail: world r21, hits r20, the
loop's node r19 and object r18, &filter r17)."""
F = "fn_801BFFD8"
SCORE = [F]
HEAD = "extern \"C\" bool fn_801BFFD8(TVehicleBase *vehicle, u32 mask, Vec3f *position, Vec3f *velocity, bool slide,\n"
NEXT = "/* Tests line against the world (skipping the vehicle's colliders); the distance to the first hit,\n"
OFF = [(HEAD, "#pragma opt_propagation off\n" + HEAD), (NEXT, "#pragma opt_propagation reset\n" + NEXT)]
REPL = {
    "base": [],
    "off": OFF,
}

# The loop's node and object declared with the function's scalars, at every position.
DECL = ["    const UnkSweepFilter_801BFF9C *self;\n", "    void *world;\n", "    CollisionResultList *hits;\n",
        "    CollisionResultList *previous;\n", "    bool hit;\n", "    bool again;\n"]
TOPOLD = "".join(DECL)
LOOPOLD = "                UnkColliContactNode_800587A0 *node = it.m_node;\n                void *object = node->m_object;\n"
LOOPNEW = "                node = it.m_node;\n                object = node->m_object;\n"
ND = "    UnkColliContactNode_800587A0 *node;\n"
OB = "    void *object;\n"
for i in range(len(DECL) + 1):
    for j in range(len(DECL) + 2):
        d = list(DECL)
        d.insert(i, ND)
        d.insert(j, OB)
        REPL["no_%d_%d" % (i, j)] = [(TOPOLD, "".join(d)), (LOOPOLD, LOOPNEW)]
