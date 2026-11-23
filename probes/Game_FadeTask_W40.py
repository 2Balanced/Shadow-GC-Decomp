"""UnkFader_80044254's constructor (wave 40): retail adds the strip width to x with x first
(`fadds f1,f5,f2`), which GC/2.7 does only when the constant has a name; the float registers of
0.0f and 0.1f follow from the order of the loads."""
SCORE = ["__ct__17UnkFader_80044254Fv"]
A = """    int k = 0;
    int alpha = (int)(255.0f * m_level);
    float x = 0.0f;
    float y = 0.0f;
"""
X64 = "            m_vertices[k].x = x + 64.0f;\n"
Y480 = "            y += 480.0f;\n"
XINC = "        x += 64.0f;\n"
Z1 = "            m_vertices[k].z = 0.1f;\n"
def v(decl, w="width", h=None, z=None):
    r = [(A, A + decl), (X64, "            m_vertices[k].x = x + %s;\n" % w), (XINC, "        x += %s;\n" % w)]
    if h:
        r.append((Y480, "            y += %s;\n" % h))
    if z:
        r.append((Z1, "            m_vertices[k].z = %s;\n" % z))
        r.append((Z1, "            m_vertices[k].z = %s;\n" % z))
    return r
REPL = {
  "w": v("    float width = 64.0f;\n"),
  "w_const": v("    const float width = 64.0f;\n"),
  "w_h": v("    float width = 64.0f;\n    float height = 480.0f;\n", h="height"),
  "w_h_z": v("    float width = 64.0f;\n    float height = 480.0f;\n    float z = 0.1f;\n", h="height", z="z"),
  "w_first": [(A, "    float width = 64.0f;\n" + A), (X64, "            m_vertices[k].x = x + width;\n"), (XINC, "        x += width;\n")],
  "w_only_vertex": [(A, A + "    float width = 64.0f;\n"), (X64, "            m_vertices[k].x = x + width;\n")],
}
LOGO = """    float scale = 1.0f;
    for (int i = 0; i < 4; i++) {
"""
AL = "        m_logo[i].emissiveColor.alpha = 0;\n"
W = REPL["w"]
REPL["logo_alpha_local"] = W + [(LOGO, "    float scale = 1.0f;\n    u8 logoAlpha = 0;\n    for (int i = 0; i < 4; i++) {\n"), (AL, "        m_logo[i].emissiveColor.alpha = logoAlpha;\n")]
REPL["logo_alpha_first"] = W + [(LOGO, "    u8 logoAlpha = 0;\n    float scale = 1.0f;\n    for (int i = 0; i < 4; i++) {\n"), (AL, "        m_logo[i].emissiveColor.alpha = logoAlpha;\n")]
REPL["logo_white_local"] = W + [(LOGO, "    float scale = 1.0f;\n    u8 white = 0xFF;\n    for (int i = 0; i < 4; i++) {\n"),
    ("        m_logo[i].emissiveColor.red = 0xFF;\n        m_logo[i].emissiveColor.green = 0xFF;\n        m_logo[i].emissiveColor.blue = 0xFF;\n",
     "        m_logo[i].emissiveColor.red = white;\n        m_logo[i].emissiveColor.green = white;\n        m_logo[i].emissiveColor.blue = white;\n")]
REPL["logo_i_outside"] = W + [(LOGO, "    float scale = 1.0f;\n    int i;\n    for (i = 0; i < 4; i++) {\n")]
T0 = """    void *state = fn_8047A170();
    void *previous = fn_804799C0();
    fn_804799B8(state, 0);
    char *path = fn_80051810("FadeLogo.txd");
    UnkFile_8047A178 *file = fn_8047A178(path, "r");
    if (file != 0) {
        u32 size = file->m_size;
        void *buffer = lbl_805F15B4->fpMalloc(size, 0);
"""
T1 = """    state = fn_8047A170();
    previous = fn_804799C0();
    fn_804799B8(state, 0);
    path = fn_80051810("FadeLogo.txd");
    file = fn_8047A178(path, "r");
    if (file != 0) {
        size = file->m_size;
        buffer = lbl_805F15B4->fpMalloc(size, 0);
"""
D = {"size": "    u32 size;\n", "file": "    UnkFile_8047A178 *file;\n", "buffer": "    void *buffer;\n", "state": "    void *state;\n", "previous": "    void *previous;\n", "path": "    char *path;\n"}
for name, order in (("top6", ("size", "file", "buffer", "state", "previous", "path")),
                    ("top6b", ("size", "buffer", "file", "state", "previous", "path")),
                    ("top6c", ("file", "size", "buffer", "state", "previous", "path"))):
    REPL[name] = W + [(T0, "".join(D[n] for n in order) + T1)]
    REPL[name + "_fn"] = W + [("UnkFader_80044254::UnkFader_80044254() {\n", "UnkFader_80044254::UnkFader_80044254() {\n" + "".join(D[n] for n in order)), (T0, T1)]
P0 = "void *fn_804799C0(void);\n"
REPL["top6_prevarg"] = W + [(P0, "void *fn_804799C0(void *state);\n"), (T0, "".join(D[n] for n in ("size", "file", "buffer", "state", "previous", "path")) + T1.replace("previous = fn_804799C0();", "previous = fn_804799C0(state);"))]
REPL["prevarg"] = W + [(P0, "void *fn_804799C0(void *state);\n"), ("    void *previous = fn_804799C0();\n", "    void *previous = fn_804799C0(state);\n")]
B0 = "void fn_804799B8(void *a, void *b);\n"
TOP6 = "".join(D[n] for n in ("size", "file", "buffer", "state", "previous", "path"))
REPL["u32_prev"] = W + [(P0, "u32 fn_804799C0(void *state);\n"), (B0, "void fn_804799B8(void *state, u32 value);\n"),
                        (T0, TOP6.replace("    void *previous;\n", "    u32 previous;\n") + T1.replace("previous = fn_804799C0();", "previous = fn_804799C0(state);"))]
REPL["u32_all"] = W + [("void *fn_8047A170(void);\n", "u32 *fn_8047A170(void);\n"), (P0, "u32 fn_804799C0(u32 *state);\n"), (B0, "void fn_804799B8(u32 *state, u32 value);\n"),
                        (T0, TOP6.replace("    void *previous;\n", "    u32 previous;\n").replace("    void *state;\n", "    u32 *state;\n") + T1.replace("previous = fn_804799C0();", "previous = fn_804799C0(state);"))]
F1 = "    for (int i = 0; i < 10; i++) {\n        for (int j = 0; j < 2; j++) {\n"
BASE = W + [(T0, TOP6 + T1.replace("previous = fn_804799C0();", "previous = fn_804799C0(state);")), (P0, "void *fn_804799C0(void *state);\n")]
REPL["i_shared"] = BASE + [(F1, "    int i;\n    for (i = 0; i < 10; i++) {\n        for (int j = 0; j < 2; j++) {\n"), (LOGO, "    float scale = 1.0f;\n    for (i = 0; i < 4; i++) {\n")]
REPL["i_shared_top"] = BASE + [("    int k = 0;\n", "    int i;\n    int k = 0;\n"), (F1, "    for (i = 0; i < 10; i++) {\n        for (int j = 0; j < 2; j++) {\n"), (LOGO, "    float scale = 1.0f;\n    for (i = 0; i < 4; i++) {\n")]
REPL["ij_top"] = BASE + [("    int k = 0;\n", "    int i;\n    int j;\n    int k = 0;\n"), (F1, "    for (i = 0; i < 10; i++) {\n        for (j = 0; j < 2; j++) {\n"), (LOGO, "    float scale = 1.0f;\n    for (i = 0; i < 4; i++) {\n")]
REPL["k_reuse"] = BASE + [(LOGO, "    float scale = 1.0f;\n    for (k = 0; k < 4; k++) {\n")]
IS = REPL["i_shared"]
def swap_top(order):
    return [(p if p[0] != T0 else (T0, "".join(D[n] for n in order) + T1.replace("previous = fn_804799C0();", "previous = fn_804799C0(state);"))) for p in IS]
REPL["is_prev_first"] = swap_top(("previous", "size", "file", "buffer", "state", "path"))
REPL["is_noprev_decl"] = [(p if p[0] != T0 else (T0, "".join(D[n] for n in ("size", "file", "buffer", "state", "path")) + T1.replace("    previous = fn_804799C0();", "    void *previous = fn_804799C0(state);"))) for p in IS]
REPL["is_noarg"] = [p for p in IS if p[0] != P0]
REPL["is_noarg"] = [(p if p[0] != T0 else (T0, TOP6 + T1)) for p in REPL["is_noarg"]]
def retype(pairs, ret, var, cast=""):
    out = []
    for p in pairs:
        if p[0] == P0:
            out.append((P0, "%s fn_804799C0(void *state);\n" % ret))
        elif p[0] == T0:
            out.append((T0, p[1].replace("    void *previous;\n", "    %s previous;\n" % var).replace("previous = fn_804799C0(state);", "previous = %sfn_804799C0(state);" % cast)))
        else:
            out.append(p)
    out.append((B0, "void fn_804799B8(void *a, %s b);\n" % var))
    return out
REPL["is_u32"] = retype(IS, "u32", "u32")
REPL["is_u32_cast"] = retype(IS, "u32", "void *", "(void *)")
REPL["is_s32"] = retype(IS, "s32", "s32")
ANCH = "UnkFader_80044254::UnkFader_80044254() {\n"
INL = "inline void *SaveState_(void *state) {\n    void *previous = fn_804799C0(state);\n    fn_804799B8(state, 0);\n    return previous;\n}\n\n"
def inl(pairs):
    out = []
    for p in pairs:
        if p[0] == T0:
            out.append((T0, p[1].replace("    previous = fn_804799C0(state);\n    fn_804799B8(state, 0);\n", "    previous = SaveState_(state);\n")))
        else:
            out.append(p)
    out.append((ANCH, INL + ANCH))
    return out
REPL["is_inl_save"] = inl(IS)
REPL["is_prev_ref"] = [(p if p[0] != T0 else (T0, p[1].replace("    previous = fn_804799C0(state);\n", "    void *&prevRef = previous;\n    prevRef = fn_804799C0(state);\n"))) for p in IS]
REPL["is_two_stmt"] = [(p if p[0] != T0 else (T0, p[1].replace("    state = fn_8047A170();\n    previous = fn_804799C0(state);\n", "    previous = fn_804799C0(state = fn_8047A170());\n"))) for p in IS]
END = "    fn_80051830(path);\n    fn_804799B8(state, previous);\n}\n"
REPL["is_sched_off"] = IS + [(ANCH, "#pragma push\n#pragma scheduling off\n" + ANCH), (END, END + "#pragma pop\n")]
REPL["is_peep_off"] = IS + [(ANCH, "#pragma push\n#pragma peephole off\n" + ANCH), (END, END + "#pragma pop\n")]
