"""Vertex repacking fn_804635D4 of Game/RwCpp26.cpp (wave 40, agent r3): prologue load order."""
P = """    desc = *(RxGCVertexDesc **)((u8 *)geometry + lbl_805F1520);
    arrayIndex = 0;
    locked = geometry->lockedSinceLastInst;
    flags = geometry->flags | rpGEOMETRYPOSITIONS;
    numVertices = geometry->numVertices;
"""
L = P.splitlines(True)
import itertools
REPL = {}
for perm in itertools.permutations(range(5)):
    REPL["o" + "".join(map(str, perm))] = [(P, "".join(L[i] for i in perm))]
SCORE = ["fn_804635D4"]
