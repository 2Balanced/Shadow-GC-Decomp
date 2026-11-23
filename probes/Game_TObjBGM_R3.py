"""Wave 40 (resumed agent): TObjBGM. JOBS lists (unit, name, [(old, new)], [symbols]) against the source
of the time, one round per function body; REPL/SCORE at the end for tools/pprobe.py. Winners: fn_801D7A50
inl_second (the second slot read through an inline getter), fn_801D7CB8 bsib1s1 (one counter, both slots
through inline getters); fn_801D78C4 stays at 12 rows."""
ALL = []
# --- s_bgm2.py
def _round():
    NL = chr(10)
    src = open('src/Game/TObjBGM.cpp', encoding='utf-8').read()
    F = 'extern "C" int fn_801D7A50(int voice, int priority, int owner) {' + NL
    i = src.index(F)
    j = src.index(NL + '}' + NL, i) + 3
    B = src[i:j]
    A0 = '    TObjSERIF *serif;' + NL
    S0 = '        s_serif[index]->Stop();' + NL + '        serif = s_serif[index];' + NL
    _anchors_present = (A0 in B and S0 in B)
    H = 'inline TObjSERIF *Serif_(int i) { return s_serif[i]; }' + NL
    V = {
        "cur": B,
        "decl_at": B.replace(A0, '').replace(S0, '        s_serif[index]->Stop();' + NL + '        TObjSERIF *serif = s_serif[index];' + NL),
        "stop_local": B.replace(S0, '        TObjSERIF *stopping = s_serif[index];' + NL + '        stopping->Stop();' + NL + '        serif = s_serif[index];' + NL),
        "inl_both": B.replace(S0, '        Serif_(index)->Stop();' + NL + '        serif = Serif_(index);' + NL),
        "inl_second": B.replace(S0, '        s_serif[index]->Stop();' + NL + '        serif = Serif_(index);' + NL),
        "inl_first": B.replace(S0, '        Serif_(index)->Stop();' + NL + '        serif = s_serif[index];' + NL),
        "decl_first": B.replace(A0, '').replace(F, F + A0) if False else B.replace('    int index = fn_801D78C4(owner, priority);' + NL, '    int index = fn_801D78C4(owner, priority);' + NL + A0).replace(A0 + '    int index', '    int index'),
    }
    JOBS = []
    for k, nb in V.items():
        for p in ("opt_propagation off", "opt_dead_assignments off", None):
            rep = [] if nb == B else [(B, nb)]
            pre = H if k.startswith("inl") else ""
            if p:
                pre += "#pragma " + p + NL
            if pre:
                rep.append((F, pre + F))
            JOBS.append(("TObjBGM", k + "_" + (p.split()[0][4:9] if p else "none"), rep, ["fn_801D7A50"]))
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError, NameError):
    pass  # the source has moved on since this round

# --- s_bgm3.py
def _round():
    NL = chr(10)
    src = open('src/Game/TObjBGM.cpp', encoding='utf-8').read()
    H = ('inline TObjSERIF *Serif_(int i) { return s_serif[i]; }' + NL +
         'inline TObjBGM *Bgm_(int i) { return s_bgm[i]; }' + NL +
         'inline s8 Prio_(TObjSERIF *s) { return s->m_priority; }' + NL)
    F1 = 'extern "C" void fn_801D7CB8(void) {' + NL
    F2 = 'extern "C" int fn_801D78C4(int owner, s8 priority) {' + NL
    JOBS = []
    # fn_801D7CB8
    V1 = {
        "bgm_inl": [('        bgm = s_bgm[i];' + NL, '        bgm = Bgm_(i);' + NL)],
        "serif_inl": [('        serif = s_serif[i];' + NL, '        serif = Serif_(i);' + NL)],
        "both_inl": [('        bgm = s_bgm[i];' + NL, '        bgm = Bgm_(i);' + NL), ('        serif = s_serif[i];' + NL, '        serif = Serif_(i);' + NL)],
    }
    for k, rep in V1.items():
        for p in ("opt_propagation off", None):
            r = list(rep) + [(F1, H + ("#pragma " + p + NL if p else "") + F1)]
            JOBS.append(("TObjBGM", "cb_" + k + "_" + ("p" if p else "n"), r, ["fn_801D7CB8"]))
    # fn_801D78C4
    V2 = {
        "s1_inl": [('            TObjSERIF *serif = s_serif[i];' + NL + '            if (serif != 0 && serif->m_owner == owner) {', '            TObjSERIF *serif = Serif_(i);' + NL + '            if (serif != 0 && serif->m_owner == owner) {')],
        "s2_inl": [('        TObjSERIF *serif = s_serif[i];' + NL + '        if (serif != 0) {' + NL + '            if (serif->m_state != 1)', '        TObjSERIF *serif = Serif_(i);' + NL + '        if (serif != 0) {' + NL + '            if (serif->m_state != 1)')],
        "found_inl": [('s_serif[found]->m_priority', 'Serif_(found)->m_priority')],
        "prio_inl": [('CanInterrupt(serif->m_priority, priority) == true', 'CanInterrupt(Prio_(serif), priority) == true')],
    }
    import itertools
    keys = list(V2)
    for n in range(1, len(keys) + 1):
        for combo in itertools.combinations(keys, n):
            rep = []
            for c in combo:
                rep += V2[c]
            for p in ("opt_propagation off", None):
                r = list(rep) + [(F2, H + ("#pragma " + p + NL if p else "") + F2)]
                JOBS.append(("TObjBGM", "c4_" + "+".join(c[:5] for c in combo) + "_" + ("p" if p else "n"), r, ["fn_801D78C4"]))
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError, NameError):
    pass  # the source has moved on since this round

# --- s_bgm4.py
def _round():
    import itertools
    NL = chr(10)
    src = open('src/Game/TObjBGM.cpp', encoding='utf-8').read()
    F = 'extern "C" void fn_801D7CB8(void) {' + NL
    i = src.index(F)
    j = src.index(NL + '}' + NL, i) + 3
    B = src[i:j]
    body = B
    body = body.replace(F + '    TObjBGM *bgm;' + NL, F + '@DECL@')
    body = body.replace('    TObjSERIF *serif;' + NL + '    for', '    for')
    body = body.replace('    for (u8 i = 0; i < 2; i++) {' + NL + '        bgm = s_bgm[i];', '    for (i = 0; i < 2; i++) {' + NL + '        bgm = s_bgm[i];')
    body = body.replace('    for (u8 i = 0; i < 2; i++) {' + NL + '        serif = s_serif[i];', '    for (i = 0; i < 2; i++) {' + NL + '        serif = s_serif[i];')
    _anchors_present = ('@DECL@' in body and 'u8 i' not in body)
    D = {"b": '    TObjBGM *bgm;' + NL, "s": '    TObjSERIF *serif;' + NL, "i": '    u8 i;' + NL}
    JOBS = []
    for perm in itertools.permutations("bsi"):
        nb = body.replace('@DECL@', "".join(D[c] for c in perm))
        for p in ("opt_propagation off", None):
            rep = [(B, nb)]
            if p:
                rep.append((F, "#pragma " + p + NL + F))
            JOBS.append(("TObjBGM", "".join(perm) + ("_p" if p else "_n"), rep, ["fn_801D7CB8"]))
    # separate counters, serif declared at top
    nb2 = B.replace(F + '    TObjBGM *bgm;' + NL, F + '    TObjBGM *bgm;' + NL + '    TObjSERIF *serif;' + NL).replace('    TObjSERIF *serif;' + NL + '    for (u8 i = 0; i < 2; i++) {' + NL + '        serif', '    for (u8 i = 0; i < 2; i++) {' + NL + '        serif')
    for p in ("opt_propagation off", None):
        rep = [(B, nb2)]
        if p:
            rep.append((F, "#pragma " + p + NL + F))
        JOBS.append(("TObjBGM", "bs_sep" + ("_p" if p else "_n"), rep, ["fn_801D7CB8"]))
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError, NameError):
    pass  # the source has moved on since this round

# --- s_bgm5.py
def _round():
    NL = chr(10)
    src = open('src/Game/TObjBGM.cpp', encoding='utf-8').read()
    F = 'extern "C" void fn_801D7CB8(void) {' + NL
    i = src.index(F)
    j = src.index(NL + '}' + NL, i) + 3
    B = src[i:j]
    k = B.index('    TObjSERIF *serif;' + NL)
    loop1 = B[len(F):k]
    loop2 = B[k:-2]
    H1 = 'inline void PauseBgms_() {' + NL + loop1 + '}' + NL
    H2 = 'inline void PauseSerifs_() {' + NL + loop2 + '}' + NL
    V = {
        "both": H1 + H2 + F + '    PauseBgms_();' + NL + '    PauseSerifs_();' + NL + '}' + NL,
        "first": H1 + F + '    PauseBgms_();' + NL + loop2 + '}' + NL,
        "second": H2 + F + loop1 + '    PauseSerifs_();' + NL + '}' + NL,
    }
    JOBS = []
    for k2, nb in V.items():
        for p in ("opt_propagation off", None):
            t = nb if not p else nb.replace(F, "#pragma " + p + NL + F)
            JOBS.append(("TObjBGM", k2 + ("_p" if p else "_n"), [(B, t)], ["fn_801D7CB8"]))
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError, NameError):
    pass  # the source has moved on since this round

# --- s_bgm7.py
def _round():
    NL = chr(10)
    
    OLD = ('            if (CanInterrupt(serif->m_priority, priority) == true) {' + NL +
           '                if (found < 0) {' + NL +
           '                    found = i;' + NL +
           '                } else if (CanInterrupt(serif->m_priority, s_serif[found]->m_priority) == true) {' + NL)
    _anchors_present = (OLD in src)
    LOC = {
        "s8loc": ('            s8 current = serif->m_priority;' + NL, 'current'),
        "intloc": ('            int current = serif->m_priority;' + NL, 'current'),
        "s8loc_top": ('', 'current'),
    }
    JOBS = []
    S1 = [('            TObjSERIF *serif = s_serif[i];' + NL + '            if (serif != 0 && serif->m_owner == owner) {', '            TObjSERIF *serif = Serif_(i);' + NL + '            if (serif != 0 && serif->m_owner == owner) {')]
    for lk, (decl, name) in LOC.items():
        new = decl + OLD.replace('CanInterrupt(serif->m_priority,', 'CanInterrupt(' + name + ',')
        rep0 = [(OLD, new)]
        if lk == "s8loc_top":
            rep0.append(('    int found = -1;' + NL, '    int found = -1;' + NL + '    s8 current;' + NL))
            rep0[0] = (OLD, '            current = serif->m_priority;' + NL + OLD.replace('CanInterrupt(serif->m_priority,', 'CanInterrupt(current,'))
        for s1 in (True, False):
            for p in ("opt_propagation off", "opt_dead_assignments off", None):
                rep = list(rep0) + (S1 if s1 else [])
                rep.append((F2, H + ("#pragma " + p + NL if p else "") + F2))
                JOBS.append(("TObjBGM", lk + ("_s1" if s1 else "") + "_" + (p.split()[0][4:9] if p else "none"), rep, ["fn_801D78C4"]))
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError, NameError):
    pass  # the source has moved on since this round

# --- s_bgm9.py
def _round():
    import itertools
    NL = chr(10)
    src = open('src/Game/TObjBGM.cpp', encoding='utf-8').read()
    F = 'extern "C" void fn_801D7CB8(void) {' + NL
    i = src.index(F)
    j = src.index(NL + '}' + NL, i) + 3
    B = src[i:j]
    H = 'inline TObjBGM *GetBgm_(int i) { return s_bgm[i]; }' + NL
    body = B
    body = body.replace(F + '    TObjBGM *bgm;' + NL, F + '@DECL@')
    body = body.replace('    TObjSERIF *serif;' + NL + '    for', '    for')
    body = body.replace('    for (u8 i = 0; i < 2; i++) {' + NL + '        bgm = s_bgm[i];', '    for (i = 0; i < 2; i++) {' + NL + '        bgm = @BGM@;')
    body = body.replace('    for (u8 i = 0; i < 2; i++) {' + NL + '        serif = s_serif[i];', '    for (i = 0; i < 2; i++) {' + NL + '        serif = @SERIF@;')
    D = {"b": '    TObjBGM *bgm;' + NL, "s": '    TObjSERIF *serif;' + NL, "i": '    u8 i;' + NL}
    JOBS = []
    for perm in itertools.permutations("bsi"):
        for bk, bv in (("b0", "s_bgm[i]"), ("b1", "GetBgm_(i)")):
            for sk, sv in (("s0", "s_serif[i]"), ("s1", "GetSerif(i)")):
                nb = body.replace('@DECL@', "".join(D[c] for c in perm)).replace('@BGM@', bv).replace('@SERIF@', sv)
                for p in ("opt_propagation off", None):
                    t = H + ("#pragma " + p + NL if p else "") + nb
                    JOBS.append(("TObjBGM", "".join(perm) + bk + sk + ("_p" if p else "_n"), [(B, t)], ["fn_801D7CB8"]))
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError, NameError):
    pass  # the source has moved on since this round

# --- s_bgm10.py
def _round():
    NL = chr(10)
    src = open('src/Game/TObjBGM.cpp', encoding='utf-8').read()
    CI = 'inline bool CanInterrupt(s8 current, s8 incoming) {' + NL
    _anchors_present = (CI in src)
    F2 = 'extern "C" int fn_801D78C4(int owner, s8 priority) {' + NL
    i = src.index(F2)
    j = src.index(NL + '}' + NL, i) + 3
    B = src[i:j]
    SIG = {
        "int_s8": 'inline bool CanInterrupt(int current, s8 incoming) {' + NL,
        "s8_int": 'inline bool CanInterrupt(s8 current, int incoming) {' + NL,
        "int_int": 'inline bool CanInterrupt(int current, int incoming) {' + NL,
        "cs8": 'inline bool CanInterrupt(const s8 current, const s8 incoming) {' + NL,
        "ref": 'inline bool CanInterrupt(const s8 &current, const s8 &incoming) {' + NL,
    }
    OLD2 = ('            if (CanInterrupt(serif->m_priority, priority) == true) {' + NL)
    LOC = {
        "none": None,
        "s8loc": ('            s8 current = serif->m_priority;' + NL, 'current'),
        "intloc": ('            int current = serif->m_priority;' + NL, 'current'),
    }
    JOBS = []
    for sk, sv in SIG.items():
        for lk, lv in LOC.items():
            nb = B
            if lv:
                nb = nb.replace(OLD2, lv[0] + OLD2.replace('serif->m_priority', lv[1])).replace('CanInterrupt(serif->m_priority, s_serif[found]', 'CanInterrupt(' + lv[1] + ', s_serif[found]')
            for p in ("opt_propagation off", None):
                rep = [(CI, sv), (B, ("#pragma " + p + NL if p else "") + nb)]
                JOBS.append(("TObjBGM", sk + "_" + lk + ("_p" if p else "_n"), rep, ["fn_801D78C4"]))
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError, NameError):
    pass  # the source has moved on since this round

JOBS = ALL
REPL = {name: pairs for (_unit, name, pairs, _syms) in JOBS}
SCORE = ["fn_801D7A50", "fn_801D78C4", "fn_801D7CB8"]
