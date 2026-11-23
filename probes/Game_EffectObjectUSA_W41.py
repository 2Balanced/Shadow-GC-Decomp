"""Wave 41: the colour conversions' stack slots. GC/2.7 gives each argument's conversion the next
slot in the order it evaluates them (right to left), except that the first argument goes ahead of
the second; retail has them strictly right to left. Shapes that fix the slot order."""
SRC = open("src/Game/EffectObjectUSA.cpp", encoding="utf-8").read()
SCORE = ["__ct__Q412SonicteamUSA6System6Effect15EffectObjectUSAFPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager",
         "Update__Q412SonicteamUSA6System6Effect15EffectObjectUSAFf"]
CALL = """        SetFadeColor(&m_fade, ((m_color[3] >> 22) & 0xFF) * scale,
                     ((m_color[0] >> 22) & 0xFF) * scale, ((m_color[1] >> 22) & 0xFF) * scale,
                     ((m_color[2] >> 22) & 0xFF) * scale);
"""
BODY = """    fade->m_color[0] = r;
    fade->m_color[1] = g;
    fade->m_color[2] = b;
    fade->m_color[3] = a;
"""
BODY_A = """    fade->m_color[3] = a;
    fade->m_color[0] = r;
    fade->m_color[1] = g;
    fade->m_color[2] = b;
"""
CALL_C3 = """        u32 alpha = m_color[3];
        SetFadeColor(&m_fade, ((alpha >> 22) & 0xFF) * scale,
                     ((m_color[0] >> 22) & 0xFF) * scale, ((m_color[1] >> 22) & 0xFF) * scale,
                     ((m_color[2] >> 22) & 0xFF) * scale);
"""
assert SRC.count(CALL) == 2
REPL = {
  "body_a": [(BODY, BODY_A)],
  "c3": [(CALL, CALL_C3), (CALL, CALL_C3)],
}
def call(decl, aexpr):
    return decl + "        SetFadeColor(&m_fade, " + aexpr + """,
                     ((m_color[0] >> 22) & 0xFF) * scale, ((m_color[1] >> 22) & 0xFF) * scale,
                     ((m_color[2] >> 22) & 0xFF) * scale);
"""
VAR = {
  "c3_masked": call("        u32 alpha = (m_color[3] >> 22) & 0xFF;\n", "alpha * scale"),
  "c3_shift": call("        u32 alpha = m_color[3] >> 22;\n", "(alpha & 0xFF) * scale"),
  "c3_float": call("        float alpha = ((m_color[3] >> 22) & 0xFF) * scale;\n", "alpha"),
  "c3_const": call("        const u32 alpha = m_color[3];\n", "((alpha >> 22) & 0xFF) * scale"),
  "c3_ptr": call("        const u32 *color = m_color;\n", "((color[3] >> 22) & 0xFF) * scale"),
  "c3_before_scale": None,
}
for k, v in VAR.items():
    if v is None:
        continue
    REPL[k] = [(CALL, v), (CALL, v)]
SC = "        float scale = 1.0f / 255.0f;\n"
REPL["c3_before_scale"] = [(SC + CALL, "        u32 alpha = m_color[3];\n" + SC + CALL_C3.replace("        u32 alpha = m_color[3];\n", "")), (SC + CALL, "        u32 alpha = m_color[3];\n" + SC + CALL_C3.replace("        u32 alpha = m_color[3];\n", ""))]
CM = VAR["c3_masked"]
BLK = """            char path[0x24];
            const ::Effect::System::UnkEffectModel_801D1238 *model = desc->m_model;
            m_drawPass = 0;
            strcpy(path, model->m_name);
"""
assert BLK in SRC
B = {
  "m1": """            char path[0x24];
            m_drawPass = 0;
            const ::Effect::System::UnkEffectModel_801D1238 *model = desc->m_model;
            strcpy(path, model->m_name);
""",
  "m2": """            char path[0x24];
            m_drawPass = 0;
            strcpy(path, desc->m_model->m_name);
""",
  "m3": """            const ::Effect::System::UnkEffectModel_801D1238 *model = desc->m_model;
            char path[0x24];
            m_drawPass = 0;
            strcpy(path, model->m_name);
""",
  "m4": """            char path[0x24];
            const char *name = desc->m_model->m_name;
            m_drawPass = 0;
            strcpy(path, name);
""",
}
for k, v in B.items():
    REPL["cm_" + k] = [(CALL, CM), (CALL, CM), (BLK, v)]
HDR = {"cm_nc": {"Game/EffectManagerPJS.h": [("    EffectObjectUSA(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,", "    EffectObjectUSA(::Effect::System::UnkEffectDesc_801D0AFC *desc,")]}}
REPL["cm_nc"] = [(CALL, CM), (CALL, CM), ("EffectObjectUSA::EffectObjectUSA(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,", "EffectObjectUSA::EffectObjectUSA(UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,")]
SCORE = [(SCORE[0], SCORE[0].replace("FPCQ36Effect6System22", "FPQ36Effect6System22")) if False else SCORE[0], SCORE[1]]
