"""Wave 40 (r7): fn_8048FED8 (palette quantiser set-up): declaration orders and the root clear."""
import itertools

SYM = "fn_8048FED8"
SYMBOLS = {SYM: "s32 fn_8048FED8(RwDlPalQuant *pq) {"}

TAIL_LOCAL = """
    pq->nodeFreeList = fn_8047FBD8(sizeof(RwDlPalNode), 0x400, 4, 0x30411);
    pq->root = (RwDlPalNode *)lbl_805F15B4->freeListAlloc(pq->nodeFreeList, 0x30411);
    root = pq->root;
    for (i = 0; i < 16; i++) root->child[i] = NULL;
    return 1;
"""

def body(decls, depthexpr="lbl_805EDF20", pre=""):
    return decls + pre + """
    for (i = 0; i < (1 << @D@); i++) {
        spread = 0;
        for (bit = 0; bit < @D@; bit++) {
            spread |= (i & (1 << bit)) ? (1 << ((@D@ - 1 - bit) * 4)) : 0;
        }
        lbl_805E2D58[i] = spread;
    }""".replace("@D@", depthexpr) + TAIL_LOCAL

BODIES = {SYM: {}}
names = ["i", "bit", "spread"]
for order in itertools.permutations(names):
    decl = "".join("    u32 %s;\n" % n for n in order) + "    RwDlPalNode *root;\n"
    BODIES[SYM]["g_" + "_".join(order)] = body(decl)
    decl2 = "    u32 depth;\n" + decl
    BODIES[SYM]["d_" + "_".join(order)] = body(decl2, "depth", "    depth = lbl_805EDF20;\n")
    decl3 = decl + "    u32 depth;\n"
    BODIES[SYM]["e_" + "_".join(order)] = body(decl3, "depth", "    depth = lbl_805EDF20;\n")
