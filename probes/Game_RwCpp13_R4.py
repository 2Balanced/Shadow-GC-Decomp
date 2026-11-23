"""Wave 40, RwCpp13 round 4: saved registers of the reinstance callback fn_8044DA3C (retail:
instance r31, atomic r30, geometry r29, skin r28, positions r27, normals r26, repEntry r25, the
saved arrays r24/r23) and of the instance callback fn_8044D924 (retail: atomic r31, geometry r30,
out r29)."""
SCORE = ["fn_8044DA3C", "fn_8044D924"]
SIG = "R2Atomic *fn_8044DA3C(R2Atomic *atomic, R2ResEntry **repEntry) {\n    R2Geometry *geometry = atomic->geometry;\n    R2Skin *skin = SKIN_OF(geometry);\n\n    if (skin->maxWeights > 1 || skin->unk2C == 3) {\n        R2SkinInstance *instance = &((R2SkinResEntry *)*repEntry)->instance;\n        R2Geometry *owner = (R2Geometry *)(*repEntry)->owner;\n        void **positions;\n        void **normals;\n        void *savedPositions;\n        void *savedNormals;\n"
def mk(decls, obj=False):
    head = "R2Atomic *fn_8044DA3C(%s, R2ResEntry **repEntry) {\n" % ("void *object" if obj else "R2Atomic *atomic")
    d = {"i": "    R2SkinInstance *instance;\n", "a": "    R2Atomic *atomic = object;\n", "g": "    R2Geometry *geometry;\n",
         "s": "    R2Skin *skin;\n", "p": "    void **positions;\n", "n": "    void **normals;\n",
         "o": "    R2Geometry *owner;\n", "S": "    void *savedPositions;\n", "N": "    void *savedNormals;\n"}
    body = "".join(d[c] for c in decls)
    body += "\n    geometry = atomic->geometry;\n    skin = SKIN_OF(geometry);\n    if (skin->maxWeights > 1 || skin->unk2C == 3) {\n        instance = &((R2SkinResEntry *)*repEntry)->instance;\n        owner = (R2Geometry *)(*repEntry)->owner;\n"
    return [(SIG, head + body)]
REPL = {
 "base": [(SIG, SIG)],
 "top_same": mk("gsiopnSN"),
 "i_first": mk("igsopnSN"),
 "obj_iagspn": mk("iagsopnSN", True),
 "obj_iagspn2": mk("iagspnoSN", True),
 "obj_aigspn": mk("aigsopnSN", True),
 "obj_igaspn": mk("igasopnSN", True),
 "obj_iagspnNS": mk("iagspnoNS", True),
}
