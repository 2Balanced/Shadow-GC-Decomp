"""Atomic node body fn_80463A5C of Game/RwCpp26.cpp (wave 40, agent r3)."""
A = """        if (geometry->numVertices == 0) {
            return 1;
        }
        if (geometry->mesh->numMeshes == 0) {
            return 1;
        }
        data.repEntry = (geometry->numMorphTargets != 1) ? atomic->repEntry : geometry->repEntry;
        data.mesh = geometry->mesh;
        data.flags = geometry->flags;
        if (data.repEntry != NULL &&
            ((RxGCInstanceHeader *)(data.repEntry + 1))->meshSerialNum != geometry->mesh->serialNum) {"""
def v(numv, meshdecl, serial):
    t = A
    t = t.replace("geometry->numVertices == 0", numv)
    if meshdecl:
        t = "        RpMeshHeader *mesh = geometry->mesh;\n\n" + t.replace("if (geometry->mesh->numMeshes == 0)", "if (mesh->numMeshes == 0)")
    t = t.replace("!= geometry->mesh->serialNum", serial)
    return [(A, t)]
REPL = {
    "u_mesh": v("(u32)geometry->numVertices == 0", True, "!= mesh->serialNum"),
    "s_mesh": v("geometry->numVertices == 0", True, "!= mesh->serialNum"),
    "u_dmesh": v("(u32)geometry->numVertices == 0", False, "!= data.mesh->serialNum"),
    "u_plain": v("(u32)geometry->numVertices == 0", False, "!= geometry->mesh->serialNum"),
}
SCORE = ["fn_80463A5C"]
