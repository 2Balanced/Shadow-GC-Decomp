"""Wave 40 (r7): fn_8048FED8 with the depth and the table size as named locals, all orders."""
import itertools

SYM = "fn_8048FED8"
SYMBOLS = {SYM: "s32 fn_8048FED8(RwDlPalQuant *pq) {"}

TAIL = """
    pq->nodeFreeList = fn_8047FBD8(sizeof(RwDlPalNode), 0x400, 4, 0x30411);
    pq->root = (RwDlPalNode *)lbl_805F15B4->freeListAlloc(pq->nodeFreeList, 0x30411);
    root = pq->root;
    for (i = 0; i < 16; i++) root->child[i] = NULL;
    return 1;
"""

BODIES = {SYM: {}}
for order in itertools.permutations(["depth", "i", "bit", "count", "spread"]):
    decl = "".join("    u32 %s;\n" % n for n in order) + "    RwDlPalNode *root;\n"
    for k, (d, c) in enumerate([("depth", "count"), ("lbl_805EDF20", "count")]):
        pre = ""
        if d == "depth":
            pre += "    depth = lbl_805EDF20;\n"
        pre += "    count = 1 << %s;\n" % d
        b = decl + pre + """
    for (i = 0; i < count; i++) {
        spread = 0;
        for (bit = 0; bit < @D@; bit++) {
            spread |= (i & (1 << bit)) ? (1 << ((@D@ - 1 - bit) * 4)) : 0;
        }
        lbl_805E2D58[i] = spread;
    }""".replace("@D@", d) + TAIL
        if d != "depth":
            b = b.replace("    u32 depth;\n", "")
            if "depth" in order[:0]:
                pass
        BODIES[SYM]["%d_%s" % (k, "_".join(order))] = b
