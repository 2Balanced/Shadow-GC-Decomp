"""Wave 41, RenderWare/RpClump: RpClumpDestroy (fn_80457468) light and camera loops in blocks."""
SCORE = ["fn_80457468"]
OLD = """    current = (u32 *)node[4];
    sentinel = node + 4;
    while (current != sentinel) {
        link = (u32 *)((u8 *)(current - 1) - clumpLightExtOffset);
        { u32 *callback = *(u32 **)((u8 *)link + clumpLightExtOffset);
        current = *(u32 **)current;
        fn_80457630((u8 *)callback, (u8 *)link);
        fn_8045ACD4(link);
        if (link == NULL) break;
     }}

    current = (u32 *)node[6];
    sentinel = node + 6;
    while (current != sentinel) {
        link = (u32 *)((u8 *)(current - 1) - clumpCameraExtOffset);
        { u32 *callback = *(u32 **)((u8 *)link + clumpCameraExtOffset);
        current = *(u32 **)current;
        fn_80457664((u8 *)callback, (u8 *)link);
        fn_804859F8(link);
        if (link == NULL) break;
     }}
"""
def blk(decl_order):
    D = {"c": "        u32 *cur;\n", "e": "        u32 *end;\n", "o": "        u32 *obj;\n", "k": "        u32 *callback;\n"}
    decl = "".join(D[x] for x in decl_order)
    def one(idx, ext, rem, destroy):
        return """    {
""" + decl + """
        cur = (u32 *)node[%d];
        end = node + %d;
        while (cur != end) {
            obj = (u32 *)((u8 *)(cur - 1) - %s);
            callback = *(u32 **)((u8 *)obj + %s);
            cur = *(u32 **)cur;
            %s((u8 *)callback, (u8 *)obj);
            %s(obj);
            if (obj == NULL) break;
        }
    }
""" % (idx, idx, ext, ext, rem, destroy)
    return one(4, "clumpLightExtOffset", "fn_80457630", "fn_8045ACD4") + "\n" + one(6, "clumpCameraExtOffset", "fn_80457664", "fn_804859F8")
REPL = {"base": []}
import itertools
for perm in itertools.permutations("ceok"):
    REPL["b_" + "".join(perm)] = [(OLD, blk(perm))]
