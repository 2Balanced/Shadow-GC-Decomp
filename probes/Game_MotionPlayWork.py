"""Wave 40 (resumed agent): MotionPlayWork. Written as JOBS lists (unit, name, [(old, new)], [symbols])
against the source of the time (the files under build/t of that session, gathered here); the REPL/SCORE
lines at the end give tools/pprobe.py the same variants. Winners: fn_80418044 dP_aPR_p (position
flag declared first, copy propagation off); the const-model variants; fn_80415EA4 EA4_inl2."""
ALL = []
# --- s_mpw3.py
def _round():
    NL = chr(10)
    src = open('src/Game/MotionPlayWork.cpp', encoding='utf-8').read()
    F = 'extern "C" void fn_80418044(MotionPlayWork *work) {' + NL
    D = '    int fixRotation;' + NL + '    int fixPosition;' + NL
    A = '        fixPosition = 0;' + NL + '        fixRotation = 0;' + NL
    _anchors_present = (D in src and A in src)
    D2 = '    int fixPosition;' + NL + '    int fixRotation;' + NL
    A2 = '        fixRotation = 0;' + NL + '        fixPosition = 0;' + NL
    A3 = '        fixPosition = fixRotation = 0;' + NL
    A4 = '        fixRotation = fixPosition = 0;' + NL
    JOBS = []
    for dk, d in (("dR", D), ("dP", D2)):
        for ak, a in (("aPR", A), ("aRP", A2), ("aP=R", A3), ("aR=P", A4)):
            for p in ("opt_propagation off", None):
                rep = []
                if d != D:
                    rep.append((D, d))
                if a != A:
                    rep.append((A, a))
                if p:
                    rep.append((F, "#pragma " + p + NL + F))
                JOBS.append(("MotionPlayWork", dk + "_" + ak + ("_p" if p else "_n"), rep, ["fn_80418044"]))
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError):
    pass  # the source has moved on since this round

# --- s_mpw4.py
def _round():
    NL = chr(10)
    src = open('src/Game/MotionPlayWork.cpp', encoding='utf-8').read()
    JOBS = []
    T = {
        "fn_8041CE74": [("void fn_8041CE74(MotionModel *model, int layer);", "void fn_8041CE74(const MotionModel *model, int layer);"),
                        ('extern "C" void fn_8041CE74(MotionModel *model, int index) {', 'extern "C" void fn_8041CE74(const MotionModel *model, int index) {')],
        "fn_8041CD6C": [("void fn_8041CD6C(MotionModel *model, int layer);", "void fn_8041CD6C(const MotionModel *model, int layer);"),
                        ('extern "C" void fn_8041CD6C(MotionModel *model, int index) {', 'extern "C" void fn_8041CD6C(const MotionModel *model, int index) {')],
        "fn_8041CC18": [("void fn_8041CC18(MotionModel *model, int layer);", "void fn_8041CC18(const MotionModel *model, int layer);"),
                        ('extern "C" void fn_8041CC18(MotionModel *model, int index) {', 'extern "C" void fn_8041CC18(const MotionModel *model, int index) {')],
        "fn_8041CFB0": [("int fn_8041CFB0(MotionModel *model, int index, u32 key, float scale, float offset);", "int fn_8041CFB0(const MotionModel *model, int index, u32 key, float scale, float offset);"),
                        ('extern "C" int fn_8041CFB0(MotionModel *model, int index, u32 key, float scale, float offset) {', 'extern "C" int fn_8041CFB0(const MotionModel *model, int index, u32 key, float scale, float offset) {')],
        "fn_8041D31C": [('extern "C" MotionNode *fn_8041D31C(MotionModel *model, u32 key) {', 'extern "C" MotionNode *fn_8041D31C(const MotionModel *model, u32 key) {')],
    }
    for sym, rep in T.items():
        JOBS.append(("MotionPlayWork", sym + "_const", rep, [sym]))
    JOBS = [("MotionPlayWork", "D31C_const", [("MotionNode *fn_8041D31C(MotionModel *model, u32 key);", "MotionNode *fn_8041D31C(const MotionModel *model, u32 key);"), ('extern "C" MotionNode *fn_8041D31C(MotionModel *model, u32 key) {', 'extern "C" MotionNode *fn_8041D31C(const MotionModel *model, u32 key) {')], ["fn_8041D31C"]),
            ("MotionPlayWork", "D31C_const_inl", [("MotionNode *fn_8041D31C(MotionModel *model, u32 key);", "MotionNode *fn_8041D31C(const MotionModel *model, u32 key);"), ('extern "C" MotionNode *fn_8041D31C(MotionModel *model, u32 key) {', 'extern "C" MotionNode *fn_8041D31C(const MotionModel *model, u32 key) {'), ("inline MotionNode *FindNode(MotionModel *model, u32 key) {", "inline MotionNode *FindNode(const MotionModel *model, u32 key) {")], ["fn_8041D31C"]),
            ("MotionPlayWork", "CFB0_const_inl", [("int fn_8041CFB0(MotionModel *model, int index, u32 key, float scale, float offset);", "int fn_8041CFB0(const MotionModel *model, int index, u32 key, float scale, float offset);"), ('extern "C" int fn_8041CFB0(MotionModel *model, int index, u32 key, float scale, float offset) {', 'extern "C" int fn_8041CFB0(const MotionModel *model, int index, u32 key, float scale, float offset) {')], ["fn_8041CFB0"]),
    ]
    
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError):
    pass  # the source has moved on since this round

# --- s_mpw5.py
def _round():
    NL = chr(10)
    P = [("int fn_8041CFB0(MotionModel *model, int index, u32 key, float scale, float offset);", "int fn_8041CFB0(const MotionModel *model, int index, u32 key, float scale, float offset);"),
         ('extern "C" int fn_8041CFB0(MotionModel *model, int index, u32 key, float scale, float offset) {', 'extern "C" int fn_8041CFB0(const MotionModel *model, int index, u32 key, float scale, float offset) {')]
    OLD = "        weightScale = &model->m_weightScales[index];" + NL
    V = {
        "plus": "        weightScale = model->m_weightScales + index;" + NL,
        "plus_rev": "        weightScale = index + model->m_weightScales;" + NL,
        "local_arr": "        MotionWeightScale *scales = model->m_weightScales;" + NL + "        weightScale = &scales[index];" + NL,
        "local_arr_top": None,
        "idx_rev": "        weightScale = &index[model->m_weightScales];" + NL,
    }
    JOBS = []
    for k, v in V.items():
        if v is None:
            rep = P + [("    MotionWeightScale *weightScale;" + NL + NL + "    if (key == model->m_header->m_key) {" + NL + OLD,
                        "    MotionWeightScale *weightScale;" + NL + "    MotionWeightScale *scales = model->m_weightScales;" + NL + NL + "    if (key == model->m_header->m_key) {" + NL + "        weightScale = &scales[index];" + NL)]
        else:
            rep = P + [(OLD, v)]
        JOBS.append(("MotionPlayWork", "CFB0_" + k, rep, ["fn_8041CFB0"]))
        JOBS.append(("MotionPlayWork", "CFB0_" + k + "_nc", rep[2:], ["fn_8041CFB0"]))
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError):
    pass  # the source has moved on since this round

# --- s_mpw7.py
def _round():
    NL = chr(10)
    src = open('src/Game/MotionPlayWork.cpp', encoding='utf-8').read()
    F = 'extern "C" void fn_80415EA4(MotionPlayWork *work) {' + NL
    i = src.index(F)
    j = src.index(NL + '}' + NL, i) + 3
    B = src[i:j]
    LOOP = B[B.index('    for (i = 0; i < work->m_layerCount; i++) {'):B.index('    fn_80419F5C(work);')]
    H = ('inline void StartLayers_(MotionPlayWork *work) {' + NL + '    MotionLayer *layer;' + NL + '    int i;' + NL + '    int index;' + NL +
         NL + LOOP + '}' + NL + NL)
    NB = B.replace(LOOP, '    StartLayers_(work);' + NL).replace('    MotionLayer *layer;' + NL + '    int i;' + NL + '    int index;' + NL + NL, '')
    H2 = H.replace('    MotionLayer *layer;' + NL + '    int i;' + NL + '    int index;' + NL + NL, '').replace('    for (i = 0;', '    for (int i = 0;').replace('        index = work->m_order[i];', '        int index = work->m_order[i];').replace('        layer = &work->m_layers[index];', '        MotionLayer *layer = &work->m_layers[index];')
    JOBS = []
    for k, h in (("inl", H), ("inl2", H2)):
        for p in ("opt_propagation off", None):
            t = h + NB if not p else h + "#pragma " + p + NL + NB
            JOBS.append(("MotionPlayWork", "EA4_" + k + ("_p" if p else "_n"), [(B, t)], ["fn_80415EA4"]))
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError):
    pass  # the source has moved on since this round

# --- s_mpw8.py
def _round():
    NL = chr(10)
    src = open('src/Game/MotionPlayWork.cpp', encoding='utf-8').read()
    H = ('inline MotionChannelLayer *Entry_(MotionChannelArgs *a) { return a->m_entries + a->m_list->m_order[0]; }' + NL +
         'inline MotionChannelLayer *Entries_(MotionChannelArgs *a) { return a->m_entries; }' + NL +
         'inline int Order_(MotionChannelArgs *a) { return a->m_list->m_order[0]; }' + NL)
    JOBS = []
    for fn in ("fn_8041FAD8", "fn_8041FB4C"):
        F = 'extern "C" void %s(void *data) {' % fn + NL
        i = src.index(F)
        j = src.index(NL + '}' + NL, i) + 3
        B = src[i:j]
        A = ('    MotionChannelArgs *args = (MotionChannelArgs *)data;' + NL +
             '    MotionChannelLayer *entry = args->m_entries;' + NL)
        E = '    entry += args->m_list->m_order[0];' + NL
        A1 = '    MotionChannelArgs *args = (MotionChannelArgs *)data;' + NL
        V = {
            "one": [(A, A1 + '    MotionChannelLayer *entry = args->m_entries + args->m_list->m_order[0];' + NL), (E, '')],
            "inl": [(A, A1 + '    MotionChannelLayer *entry = Entry_(args);' + NL), (E, '')],
            "inl_entries": [(A, A1 + '    MotionChannelLayer *entry = Entries_(args);' + NL)],
            "inl_order": [(E, '    entry += Order_(args);' + NL)],
            "idx": [(A, A1 + '    MotionChannelLayer *entry = &args->m_entries[args->m_list->m_order[0]];' + NL), (E, '')],
            "list_first": [(A, A1 + '    MotionLayerList *list = args->m_list;' + NL + '    MotionChannelLayer *entry = args->m_entries;' + NL), (E, '    entry += list->m_order[0];' + NL)],
        }
        for k, rep in V.items():
            nb = B
            for o, n in rep:
                nb = nb.replace(o, n, 1)
            for p in ("opt_propagation off", None):
                JOBS.append(("MotionPlayWork", fn[-4:] + "_" + k + ("_p" if p else "_n"), [(B, H + ("#pragma " + p + NL if p else "") + nb)], [fn]))
    for fn in ("fn_8041FAD8", "fn_8041FB4C"):
        F = 'extern "C" void %s(void *data) {' % fn + NL
        i = src.index(F)
        j = src.index(NL + '}' + NL, i) + 3
        B = src[i:j]
        for k, (o, n) in {"cargs": ('    MotionChannelArgs *args = (MotionChannelArgs *)data;', '    const MotionChannelArgs *args = (const MotionChannelArgs *)data;'),
                          "centry": ('    MotionChannelLayer *entry = args->m_entries;', '    const MotionChannelLayer *entry = args->m_entries;')}.items():
            nb = B.replace(o, n, 1)
            for p in ("opt_propagation off", None):
                JOBS.append(("MotionPlayWork", fn[-4:] + "_" + k + ("_p" if p else "_n"), [(B, ("#pragma " + p + NL if p else "") + nb)], [fn]))
        nb = B.replace('    MotionChannelArgs *args = (MotionChannelArgs *)data;', '    const MotionChannelArgs *args = (const MotionChannelArgs *)data;', 1).replace('    MotionChannelLayer *entry = args->m_entries;', '    const MotionChannelLayer *entry = args->m_entries;', 1)
        JOBS.append(("MotionPlayWork", fn[-4:] + "_cboth_n", [(B, nb)], [fn]))
    
    return JOBS
try:
    ALL += _round()
except (ValueError, KeyError):
    pass  # the source has moved on since this round

JOBS = ALL
REPL = {name: pairs for (_unit, name, pairs, _syms) in JOBS}
SCORE = sorted({s for (_u, _n, _p, syms) in JOBS for s in syms})
