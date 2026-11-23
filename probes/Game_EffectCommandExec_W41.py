"""Wave 41: EffCmdPtnAnimExec's constructor (retail: the member into r0, the converted random into
r3 for the add) with copy propagation off and named values."""
P = "__ct__Q36Effect6System17EffCmdPtnAnimExecFPCUcPUlP5Vec2fP5Vec2f"
SCORE = [P]
HEAD = "EffCmdPtnAnimExec::EffCmdPtnAnimExec(const u8 *data, u32 *state,\n"
NEXT = "bool EffCmdPtnAnimExec::Exec(float dt) {\n"
OFF = [(HEAD, "#pragma opt_propagation off\n" + HEAD), (NEXT, "#pragma opt_propagation reset\n" + NEXT)]
OLD = "        m_cell += (int)(c->m_firstCellRange * EffectRandom());\n"
REPL = {
    "off": OFF,
    "off_add": OFF + [(OLD, "        int add = (int)(c->m_firstCellRange * EffectRandom());\n        m_cell += add;\n")],
    "off_cell": OFF + [(OLD, "        u8 cell = m_cell;\n        m_cell = cell + (int)(c->m_firstCellRange * EffectRandom());\n")],
    "off_both": OFF + [(OLD, "        u8 cell = m_cell;\n        int add = (int)(c->m_firstCellRange * EffectRandom());\n        m_cell = cell + add;\n")],
    "off_both2": OFF + [(OLD, "        int add = (int)(c->m_firstCellRange * EffectRandom());\n        u8 cell = m_cell;\n        m_cell = cell + add;\n")],
    "off_rand": OFF + [(OLD, "        float random = EffectRandom();\n        m_cell += (int)(c->m_firstCellRange * random);\n")],
}

A = "__ct__Q36Effect6System18EffCmdMatAlphaExecFPCUcPUlPUl"
C = "__ct__Q36Effect6System18EffCmdMatColorExecFPCUcPUlPUl"
SCORE = [P, A, C]
AH = "EffCmdMatAlphaExec::EffCmdMatAlphaExec(const u8 *data, u32 *state, u32 *target)\n"
CH = "EffCmdMatColorExec::EffCmdMatColorExec(const u8 *data, u32 *state, u32 *target)\n"
AOLD = """    m_delta = c->m_base;
    m_delta += (int)(c->m_range * EffectRandom());
    if (c->m_range != 0) {
        m_delta += (int)(c->m_range * EffectRandom());
    }
"""
ANEW = """    m_delta = c->m_base;
    int add = (int)(c->m_range * EffectRandom());
    m_delta += add;
    if (c->m_range != 0) {
        int more = (int)(c->m_range * EffectRandom());
        m_delta += more;
    }
"""
def coff(head, nxt):
    return [(head, "#pragma opt_propagation off\n" + head), (nxt, "#pragma opt_propagation reset\n" + nxt)]
AOFF = coff(AH, "bool EffCmdMatAlphaExec::Exec(float dt) {\n")
COFF = coff(CH, "bool EffCmdMatColorExec::Exec(float dt) {\n")
COLD = """        m_delta[0] = c->m_base[0];
        if (c->m_range[0] != 0) {
            m_delta[0] += (int)(c->m_range[0] * EffectRandom());
        }
        m_delta[1] = c->m_base[1];
        if (c->m_range[1] != 0) {
            m_delta[1] += (int)(c->m_range[1] * EffectRandom());
        }
        m_delta[2] = c->m_base[2];
        if (c->m_range[2] != 0) {
            m_delta[2] += (int)(c->m_range[2] * EffectRandom());
        }
    }
"""
CNEW = """        m_delta[0] = c->m_base[0];
        if (c->m_range[0] != 0) {
            int add = (int)(c->m_range[0] * EffectRandom());
            m_delta[0] += add;
        }
        m_delta[1] = c->m_base[1];
        if (c->m_range[1] != 0) {
            int add = (int)(c->m_range[1] * EffectRandom());
            m_delta[1] += add;
        }
        m_delta[2] = c->m_base[2];
        if (c->m_range[2] != 0) {
            int add = (int)(c->m_range[2] * EffectRandom());
            m_delta[2] += add;
        }
    }
"""
REPL.update({
    "alpha_off_add": AOFF + [(AOLD, ANEW)],
    "color_off_add": COFF + [(COLD, CNEW)],
})

# GetSpeed's out-of-line copy: retail emits it after EffCmdSingleExec's constructor (the last own
# function), GC/2.7 after the launch command's Exec. Define it after its callers.
GS_OLD = """    float GetSpeed() const {
        return m_speed;
    }
"""
GS_NEW = """#ifdef EFFECT_ELEMENT_GETSPEED_LATE
    inline float GetSpeed() const;
#else
    float GetSpeed() const {
        return m_speed;
    }
#endif
"""
TAIL = "EffCmdRequestStopExec::~EffCmdRequestStopExec() {}\n"
REPL.update({
    "gs_end": [("#define EFFECT_COMMAND_LAUNCH\n", "#define EFFECT_COMMAND_LAUNCH\n#define EFFECT_ELEMENT_GETSPEED_LATE\n"),
               (TAIL, TAIL + "\ninline float EffectElement::GetSpeed() const {\n    return m_speed;\n}\n")],
    "gs_first": [("#define EFFECT_COMMAND_LAUNCH\n", "#define EFFECT_COMMAND_LAUNCH\n#define EFFECT_ELEMENT_GETSPEED_LATE\n"),
               ("EffCmdSingleExec::EffCmdSingleExec(", "inline float EffectElement::GetSpeed() const {\n    return m_speed;\n}\n\nEffCmdSingleExec::EffCmdSingleExec(")],
})
HDR = {
    "gs_end": {"Effect/System/EffectElement.h": [(GS_OLD, GS_NEW)]},
    "gs_first": {"Effect/System/EffectElement.h": [(GS_OLD, GS_NEW)]},
}
GS_DECL = """    float GetSpeed() const;
"""
REPL.update({
    "gs_ool_first": [("EffCmdSingleExec::EffCmdSingleExec(", "float EffectElement::GetSpeed() const {\n    return m_speed;\n}\n\nEffCmdSingleExec::EffCmdSingleExec(")],
})
HDR["gs_ool_first"] = {"Effect/System/EffectElement.h": [(GS_OLD, GS_DECL)]}
