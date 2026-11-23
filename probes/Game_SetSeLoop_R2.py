"""Wave 40 (resumed agent): SetSeLoop::Behave. JOBS lists (unit, name, [(old, new)], [symbols]) against the
source of the time; the REPL/SCORE lines at the end give tools/pprobe.py the same variants. Winner:
self_full_p (a copy of this, the target pointer, the manager and the id in locals, copy propagation off)."""
try:
    import re
    NL = chr(10)
    src = open('src/Game/SetSeLoop.cpp', encoding='utf-8').read()
    F = 'void SetSeLoop::Behave(float dt) {' + NL
    i = src.index(F)
    j = src.index(NL + '}' + NL, i) + 3
    B = src[i:j]
    B2 = B.replace(F, F + '    SetSeLoop *self = this;' + NL)
    B2 = re.sub(r'(?<![\w>.])m_', 'self->m_', B2)
    B2 = B2.replace(', this, ', ', self, ')
    P_OLD = "fn_801D53C4(fn_80053164(), self->m_params.seId, &self->m_chara->m_position, self, 3, 0, 2);"
    A_OLD = "fn_8040CD74(&offset, &target->m_position, &self->m_chara->m_position);"
    A = "CharInfo *t = target.get();" + NL + "                        fn_8040CD74(&offset, &t->m_position, &self->m_chara->m_position);"
    P_MGR = "void *mgr = fn_80053164();" + NL + "                    fn_801D53C4(mgr, self->m_params.seId, &self->m_chara->m_position, self, 3, 0, 2);"
    PV2 = {
        "mgr": P_MGR,
        "orig": P_OLD,
        "mgr_s8": P_MGR.replace(", self, 3, 0, 2);", ", self, 3, (s8)0, 2);"),
        "mgr_id": "void *mgr = fn_80053164();" + NL + "                    u16 id = self->m_params.seId;" + NL + "                    fn_801D53C4(mgr, id, &self->m_chara->m_position, self, 3, 0, 2);",
        "mgr_chara": "void *mgr = fn_80053164();" + NL + "                    GadgetCharInfo *chara = self->m_chara.get();" + NL + "                    fn_801D53C4(mgr, self->m_params.seId, &chara->m_position, self, 3, 0, 2);",
        "mgr_pos": "void *mgr = fn_80053164();" + NL + "                    const Vec3f *pos = &self->m_chara->m_position;" + NL + "                    fn_801D53C4(mgr, self->m_params.seId, pos, self, 3, 0, 2);",
        "pos_mgr": "const Vec3f *pos = &self->m_chara->m_position;" + NL + "                    fn_801D53C4(fn_80053164(), self->m_params.seId, pos, self, 3, 0, 2);",
    }
    JOBS = []
    for pk, pv in PV2.items():
        nb = B2.replace(A_OLD, A).replace(P_OLD, pv)
        JOBS.append(("SetSeLoop", "tptr_" + pk, [(B, nb), (F, "#pragma opt_propagation off" + NL + F)], ["Behave"]))
    A0 = "fn_8040CD74(&offset, &target->m_position, &m_chara->m_position);"
    P0 = "fn_801D53C4(fn_80053164(), m_params.seId, &m_chara->m_position, this, 3, 0, 2);"
    _anchors_present = (A0 in B and P0 in B)
    A0n = "CharInfo *t = target.get();" + NL + "                        fn_8040CD74(&offset, &t->m_position, &m_chara->m_position);"
    P0n = "void *mgr = fn_80053164();" + NL + "                    u16 id = m_params.seId;" + NL + "                    fn_801D53C4(mgr, id, &m_chara->m_position, this, 3, 0, 2);"
    JOBS_B = []
    for k, nb in (("noself", B.replace(A0, A0n).replace(P0, P0n)), ("noself_noT", B.replace(P0, P0n)), ("self_full", B2.replace(A_OLD, A).replace(P_OLD, PV2["mgr_id"]))):
        for p in ("opt_propagation off", None):
            JOBS_B.append(("SetSeLoop", k + ("_p" if p else "_n"), [(B, nb)] + ([(F, "#pragma " + p + NL + F)] if p else []), ["Behave"]))
    
    JOBS = JOBS + JOBS_B
except (ValueError, KeyError):
    JOBS = []  # the source has moved on since this round
REPL = {name: pairs for (_unit, name, pairs, _syms) in JOBS}
SCORE = ["Behave__9SetSeLoopFf"]
