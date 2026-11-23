"""Wave 41, RenderWare/RpClump: RpClumpCreate (fn_804573A8) with chained list initialisations."""
SCORE = ["fn_804573A8"]
OLD = """    { u32 *linkA = created + 2;
    u32 *linkB = created + 4;
    u32 *linkC = created + 6;
    ((u8 *)created)[0] = 2;
    ((u8 *)created)[1] = 0;
    ((u8 *)created)[2] = 0;
    ((u8 *)created)[3] = 0;
    created[1] = 0;
    created[1] = 0;
    created[2] = (u32)linkA;
    created[3] = (u32)linkA;
    created[4] = (u32)linkB;
    created[5] = (u32)linkB;
    created[6] = (u32)linkC;
    created[7] = (u32)linkC;
    created[9] = 0;
    created[8] = 0;
    created[0xA] = (u32)fn_80456230;
    fn_8048209C(lbl_8056D7F8, created);
    return created;
 }}"""
def v(lists, zeros, head):
    return head + lists + zeros + """    created[0xA] = (u32)fn_80456230;
    fn_8048209C(lbl_8056D7F8, created);
    return created;
}"""
HEAD1 = """    ((u8 *)created)[0] = 2;
    ((u8 *)created)[1] = 0;
    ((u8 *)created)[2] = 0;
    ((u8 *)created)[3] = 0;
    created[1] = 0;
    created[1] = 0;
"""
L1 = """    created[3] = created[2] = (u32)(created + 2);
    created[5] = created[4] = (u32)(created + 4);
    created[7] = created[6] = (u32)(created + 6);
"""
L2 = """    created[2] = created[3] = (u32)(created + 2);
    created[4] = created[5] = (u32)(created + 4);
    created[6] = created[7] = (u32)(created + 6);
"""
Z1 = """    created[9] = 0;
    created[8] = 0;
"""
Z2 = """    created[9] = created[8] = 0;
"""
Z3 = """    created[8] = created[9] = 0;
"""
HEAD2 = """    ((u8 *)created)[0] = 2;
    ((u8 *)created)[1] = 0;
    ((u8 *)created)[2] = 0;
    ((u8 *)created)[3] = 0;
    created[1] = 0;
    created[1] = 0;
"""
REPL = {"base": []}
for ln, l in (("l1", L1), ("l2", L2)):
    for zn, z in (("z1", Z1), ("z2", Z2), ("z3", Z3)):
        REPL[ln + zn] = [(OLD, v(l, z, HEAD1))]
