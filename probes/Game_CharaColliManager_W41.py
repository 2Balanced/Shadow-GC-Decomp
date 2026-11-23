"""Wave 41: fn_800C5478 (sphere query). Retail ranks the saved registers i r31, attack r30, manager
r29, the list offset r28, collider r27, sphere r26, kind r25, results r24; GC/2.7 puts attack,
manager, the offset and the collider ahead of i and results ahead of sphere and kind. Copy
propagation off (the wave-40 lever) with named copies."""
F = "fn_800C5478"
SCORE = [F]
HEAD = """CollisionResultList *fn_800C5478(Manager *manager, const UnkSphere_8004E6CC *sphere, u32 kind, bool collect,
                                 CollisionResultList *results, CharaColliBase::Filter filter, u32 filterArg,
                                 boost::weak_ptr<CharaColliAttack> attack) {
"""
NEXT = "/* The colliders of the kinds that touch kind which a sphere meets as it moves by move; ignore lists"
OFF = [(HEAD, "#pragma opt_propagation off\n" + HEAD), (NEXT, "#pragma opt_propagation reset\n" + NEXT)]
LOOP = "    for (int i = 0; i < manager->m_count; i++) {\n"
REPL = {
    "off": OFF,
    "off_itop": OFF + [(HEAD + LOOP, HEAD + "    int i;\n    for (i = 0; i < manager->m_count; i++) {\n")],
    "off_res": OFF + [(HEAD + LOOP, HEAD + "    CollisionResultList *list = results;\n" + LOOP)],
    "off_itop_mgr": OFF + [(HEAD + LOOP, HEAD + "    int i;\n    Manager *self = manager;\n    for (i = 0; i < self->m_count; i++) {\n")],
    "itop": [(HEAD + LOOP, HEAD + "    int i;\n    for (i = 0; i < manager->m_count; i++) {\n")],
}

# fn_800C4DE0: retail keeps filter in r20 above the block temporaries; GC/2.7 r16.
G = "fn_800C4DE0"
SCORE = [F, G]
GH = """CollisionResultList *fn_800C4DE0(Manager *manager, const UnkSphere_8004E6CC *sphere, const Vec3f *move, u32 kind,
                                 bool collect, CollisionResultList *results, CharaColliBase::Filter filter,
                                 u32 filterArg, boost::weak_ptr<CharaColliAttack> attack,
                                 CollisionResultList *ignore) {
"""
GNEXT = "/* Whether a line meets a collider. */\n"
import re as _re
_src = open("src/Game/CharaColliManager.cpp", encoding="utf-8").read()
_i = _src.index(GH)
_j = _src.index("\n}\n", _i) + 3
GBODY = _src[_i:_j]
GOFF_HEAD = "#pragma opt_propagation off\n"
GOFF_TAIL = "#pragma opt_propagation reset\n"
def g(body, off=True):
    b = body
    if off:
        b = GOFF_HEAD + b + GOFF_TAIL
    return [(GBODY, b)]
GB_F = GBODY.replace("filter, filterArg)", "f, filterArg)")
REPL.update({
    "g_off": g(GBODY),
    "g_off_f_first": g(GB_F.replace("    bool still = false;\n", "    CharaColliBase::Filter f = filter;\n    bool still = false;\n", 1)),
    "g_off_f_after": g(GB_F.replace("    bool still = false;\n", "    bool still = false;\n    CharaColliBase::Filter f = filter;\n", 1)),
    "g_f_first": g(GB_F.replace("    bool still = false;\n", "    CharaColliBase::Filter f = filter;\n    bool still = false;\n", 1), False),
})
