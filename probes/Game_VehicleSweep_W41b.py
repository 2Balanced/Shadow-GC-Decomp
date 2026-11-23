"""Wave 41: fn_801BFFD8 without the named &filter (retail gives &filter the lowest register, r17,
as a compiler temporary), every order of the other five scalars."""
import itertools
F = "fn_801BFFD8"
SCORE = [F]
DECL = {"self": "    const UnkSweepFilter_801BFF9C *self;\n", "world": "    void *world;\n",
        "hits": "    CollisionResultList *hits;\n", "previous": "    CollisionResultList *previous;\n",
        "hit": "    bool hit;\n", "again": "    bool again;\n"}
TOPOLD = "".join(DECL[k] for k in ("self", "world", "hits", "previous", "hit", "again"))
REPL = {}
for perm in itertools.permutations(("world", "hits", "previous", "hit", "again")):
    top = "".join(DECL[k] for k in perm)
    REPL["ns_" + "_".join(p[:2] for p in perm)] = [(TOPOLD, top), ("    self = &filter;\n", ""),
                                                   ("AcceptHit, self, 0);", "AcceptHit, &filter, 0);")]
