"""Wave 41 (f1): EffectRay::Generate's saved registers (retail: ray r31, shape r30, desc r29, count
r28, i r27; GC/2.7 with the wave 40 source: count r30, i r28, shape r27) and float registers
(retail: spread f31, velocity f30), under cflags_effect_nopool."""
SCORE = ["Generate__Q36Effect6System9EffectRayFf"]
HEAD = """void EffectRay::Generate(float dt) {
    int count = 0;
    float quantity = m_rate * dt;
    const UnkRayDesc_804298D4 *desc = reinterpret_cast<const UnkRayDesc_804298D4 *>(m_desc);
"""
SH = """        const UnkRayShape_804298D4 *shape = desc->m_shape;
"""
SH2 = """        shape = desc->m_shape;
"""
LOOP = "    for (int i = 0; i < count; i++) {\n"
LOOP2 = "    for (i = 0; i < count; i++) {\n"
VEL = "        float velocity = desc->m_speedRange * RandomFloat() + desc->m_speed;\n"
def head(*decls, desc_first=False):
    s = "void EffectRay::Generate(float dt) {\n"
    for d in decls:
        s += "    " + d + "\n"
    if desc_first:
        s += "    int count = 0;\n    float quantity = m_rate * dt;\n    desc = reinterpret_cast<const UnkRayDesc_804298D4 *>(m_desc);\n"
    else:
        s += "    int count = 0;\n    float quantity = m_rate * dt;\n    const UnkRayDesc_804298D4 *desc = reinterpret_cast<const UnkRayDesc_804298D4 *>(m_desc);\n"
    return s
SHD = "const UnkRayShape_804298D4 *shape;"
DD = "const UnkRayDesc_804298D4 *desc;"
REPL = {
    "base": [],
    "sh_top": [(HEAD, head(SHD)), (SH, SH2)],
    "sh_desc_top": [(HEAD, head(SHD, DD, desc_first=True)), (SH, SH2)],
    "sh_loop": [(VEL, "        const UnkRayShape_804298D4 *shape;\n" + VEL), (SH, SH2)],
    "ray_top": [(HEAD, head("EffectRayExecUnit *ray;")), ("        EffectRayExecUnit *ray = new", "        ray = new")],
    "ray_sh_top": [(HEAD, head("EffectRayExecUnit *ray;", SHD)), ("        EffectRayExecUnit *ray = new", "        ray = new"), (SH, SH2)],
}
# count and i declared after shape and desc
HEAD_C = """void EffectRay::Generate(float dt) {
    const UnkRayShape_804298D4 *shape;
    const UnkRayDesc_804298D4 *desc;
    int count;
    int i;
    count = 0;
    float quantity = m_rate * dt;
    desc = reinterpret_cast<const UnkRayDesc_804298D4 *>(m_desc);
"""
REPL["sdci"] = [(HEAD, HEAD_C), (SH, SH2), (LOOP, LOOP2)]
HEAD_D = """void EffectRay::Generate(float dt) {
    const UnkRayShape_804298D4 *shape;
    int count = 0;
    float quantity = m_rate * dt;
    const UnkRayDesc_804298D4 *desc = reinterpret_cast<const UnkRayDesc_804298D4 *>(m_desc);
    int i;
"""
REPL["s_c_d_i"] = [(HEAD, HEAD_D), (SH, SH2), (LOOP, LOOP2)]

# Round 2: on top of sh_desc_top (GPRs match), the float registers. Retail case 0: spread f31,
# velocity f30, angle f29, radius f27, tilt f26, s f28, c f31, height f29, ts f29; case 1: spread
# f26, angle f28, radius f26, tilt f27, ts f29, tc f31, x f27.
BASE2 = REPL["sh_desc_top"]
C0_SPREAD = "            float spread = shape->m_spread;\n            float angle;\n            if ((m_config & 0x40)"
C1_SPREAD = "            float spread = shape->m_spread;\n            float angle = spread * RandomFloat() - shape->m_spread;\n"
REPL["sp_looptop"] = BASE2 + [(VEL, "        float spread;\n" + VEL),
                              (C0_SPREAD, "            spread = shape->m_spread;\n            float angle;\n            if ((m_config & 0x40)")]
REPL["sp_looptop_both"] = BASE2 + [(VEL, "        float spread;\n" + VEL),
                              (C0_SPREAD, "            spread = shape->m_spread;\n            float angle;\n            if ((m_config & 0x40)"),
                              (C1_SPREAD, "            spread = shape->m_spread;\n            float angle = spread * RandomFloat() - shape->m_spread;\n")]
REPL["sp_angle_looptop"] = BASE2 + [(VEL, "        float spread;\n        float angle;\n" + VEL),
                              (C0_SPREAD, "            spread = shape->m_spread;\n            if ((m_config & 0x40)"),
                              (C1_SPREAD, "            spread = shape->m_spread;\n            angle = spread * RandomFloat() - shape->m_spread;\n")]
# velocity declared late: as a function-top float assigned in the loop (after shape decl)
REPL["vel_fntop"] = [(p[0], p[1]) for p in BASE2] + [(VEL, "        velocity = desc->m_speedRange * RandomFloat() + desc->m_speed;\n"),
                     ("    int count = 0;\n    float quantity", "    float velocity;\n    int count = 0;\n    float quantity")]
