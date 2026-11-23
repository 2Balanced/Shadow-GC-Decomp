"""Wave 40 (resumed agent): fn_801BFFD8: all 720 orders of the scalar locals declared at the top, with and without copy propagation (best 21 rows: self, world, hits, previous, hit, again).
Written as a JOBS list (unit, name, [(old, new)], [symbols]) against the source of the time;
the REPL/SCORE lines at the end give tools/pprobe.py the same variants."""
import itertools
NL = chr(10)
src = open('src/Game/VehicleSweep.cpp', encoding='utf-8').read()
F = 'extern "C" bool fn_801BFFD8(TVehicleBase *vehicle, u32 mask, Vec3f *position, Vec3f *velocity, bool slide,' + NL
OLD_HEAD = ('    void *world = fn_80020D40();' + NL +
            '    CollisionResultList *previous = 0;' + NL +
            '    UnkSphere_80021C04 sphere;' + NL)
OLD_MID = ('    bool again;' + NL +
           '    CollisionResultList *hits;' + NL +
           '    bool hit = false;' + NL +
           '    const UnkSweepFilter_801BFF9C *self = &filter;' + NL)
_anchors_present = (OLD_HEAD in src and OLD_MID in src)  # False once the source moved on
DECL = {
    "world": '    void *world;' + NL,
    "previous": '    CollisionResultList *previous;' + NL,
    "again": '    bool again;' + NL,
    "hits": '    CollisionResultList *hits;' + NL,
    "hit": '    bool hit;' + NL,
    "self": '    const UnkSweepFilter_801BFF9C *self;' + NL,
}
NEW_HEAD = ('    world = fn_80020D40();' + NL + '    previous = 0;' + NL + '    UnkSphere_80021C04 sphere;' + NL)
NEW_MID = ('    hit = false;' + NL + '    self = &filter;' + NL)
JOBS = []
names = list(DECL)
for perm in itertools.permutations(names):
    decl = "".join(DECL[n] for n in perm)
    tag = "".join(n[0] if n != "hits" else "x" for n in perm)
    for p in ("opt_propagation off", None):
        rep = [(OLD_HEAD, decl + NEW_HEAD), (OLD_MID, NEW_MID)]
        if p:
            rep.append((F, "#pragma " + p + NL + F))
        JOBS.append(("VehicleSweep", tag + ("_p" if p else "_n"), rep, ["fn_801BFFD8"]))

REPL = {name: pairs for (_unit, name, pairs, _syms) in JOBS}
SCORE = ['fn_801BFFD8']
