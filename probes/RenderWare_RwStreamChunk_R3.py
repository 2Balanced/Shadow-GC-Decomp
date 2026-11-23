# Round 3 for src/RenderWare/RwStreamChunk.c: the swap's declarations, scored on its inlined copies.
import itertools
OLD = "    u32 v;\n    u32 *p = (u32 *)mem;\n    u32 n;\n\n    for (n = size >> 2; n != 0; n--) {\n        v = *p;\n"
decls = ["u32 *p = (u32 *)mem;", "u32 n;", "u32 v;"]
REPL = {}
for i, perm in enumerate(itertools.permutations(decls)):
    new = "    " + "\n    ".join(perm) + "\n\n    for (n = size >> 2; n != 0; n--) {\n        v = *p;\n"
    REPL["both%d" % i] = [(OLD, new), (OLD, new)]
    REPL["nat%d" % i] = [(OLD, OLD), (OLD, new)]
# a version that names the size in the loop
SCORE = ["fn_8047D158", "fn_8047D254", "fn_8047D350", "fn_8047D784", "fn_8047D94C", "fn_8047CF50"]
