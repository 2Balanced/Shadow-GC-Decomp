"""Wave 40 (resumed agent): VehicleColliMove Settle, all 720 orders of the world, counter, filter, object,
hit list and clear flag declared at the top (node last), with and without copy propagation: 20 rows each.
JOBS lists (unit, name, [(old, new)], [symbols]); REPL/SCORE at the end for tools/pprobe.py."""
try:
    import itertools
    NL = chr(10)
    src = open('src/Game/VehicleColliMove.cpp', encoding='utf-8').read()
    F = 'static UnkCopyVector3_800091EC Settle(const Vec3f &position, const Vec3f &direction,' + NL
    i0 = src.index(F)
    j0 = src.index(NL + '}' + NL, i0) + 3
    B = src[i0:j0]
    R = [
        ('    void *world = fn_80020D40();' + NL + '    int i;' + NL, '@DECL@    world = fn_80020D40();' + NL),
        ('    const UnkColliMoveFilter_80333AE4 *self = &filter;' + NL + '    void *object;' + NL + '    CollisionResultList *hits;' + NL, '    self = &filter;' + NL),
        ('        bool clear = true;' + NL + '        UnkColliContactNode_800587A0 *node;' + NL, '        clear = true;' + NL),
    ]
    T = B
    for o, n in R:
        _anchors_present = (o in T, o)
        T = T.replace(o, n, 1)
    DECL = {
        "w": '    void *world;' + NL,
        "i": '    int i;' + NL,
        "s": '    const UnkColliMoveFilter_80333AE4 *self;' + NL,
        "o": '    void *object;' + NL,
        "h": '    CollisionResultList *hits;' + NL,
        "c": '    bool clear;' + NL,
        "n": '    UnkColliContactNode_800587A0 *node;' + NL,
    }
    JOBS = []
    for perm in itertools.permutations("wisohc"):
        order = "".join(perm) + "n"
        body = T.replace('@DECL@', "".join(DECL[c] for c in order))
        for p in ("opt_propagation off", None):
            t = body if not p else "#pragma " + p + NL + body
            JOBS.append(("VehicleColliMove", "S" + order + ("_p" if p else "_x"), [(B, t)], ["Settle__FRC5Vec3fRC5Vec3ff"]))
    
except (ValueError, KeyError):
    JOBS = []
REPL = {name: pairs for (_unit, name, pairs, _syms) in JOBS}
SCORE = ["Settle__FRC5Vec3fRC5Vec3ff"]
