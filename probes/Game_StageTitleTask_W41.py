"""Wave 41: UnkObject_801E67EC::Load with copy propagation off and a named copy of this / stage (the
wave-40 lever): retail keeps the string pool base in r31 and this in r30."""
import re
SCORE = ["Load__18UnkObject_801E67ECFi"]
F = "bool UnkObject_801E67EC::Load(int stage) {"
END = "bool UnkObject_801E67EC::Update() {"
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
BODY = """    char path[0x80];
    if (m_state != 0) {
        return false;
    }
    m_state = 4;
    if (lbl_8057D6A0.field_0xB1 != 0) {
        return false;
    }
    sprintf(path, "csdFiles/stageTitle/StageTitle%04d%s.txd", stage,
            lbl_8051D528[lbl_8057D6A0.m_language]);
    if (!FileExists(path)) {
        return false;
    }
    if (fn_8003F938(m_csd, "csdFiles/stageTitle/StageTitle", path)) {
        strcpy(m_csd.m_sceneName, "StageTitle");
        fn_8003E414(m_csd, 1, 1);
        fn_8003E414(m_csd, 2, 0);
        fn_8003FB10(m_csd);
        m_state = 1;
        fn_8003FE2C(m_csd);
        return true;
    }
    return false;
"""
def selfify(body, name="self"):
    return re.sub(r"(?<![.>])\bm_", name + "->m_", body)
S1 = "    UnkObject_801E67EC *self = this;\n"
S2 = "    UnkObject_801E67EC *const self = this;\n"
B2 = BODY.replace("    char path[0x80];\n", "")
REPL = {
  "prag": P,
  "prag_self_first": P + [(BODY, S1 + selfify(BODY))],
  "prag_self_after": P + [(BODY, "    char path[0x80];\n" + S1 + selfify(B2))],
  "prag_cself_first": P + [(BODY, S2 + selfify(BODY))],
  "prag_cself_after": P + [(BODY, "    char path[0x80];\n" + S2 + selfify(B2))],
  "prag_stage": P + [(BODY, "    char path[0x80];\n    int n = stage;\n" + B2.replace("stage,\n", "n,\n"))],
  "prag_self_stage": P + [(BODY, "    char path[0x80];\n" + S1 + "    int n = stage;\n" + selfify(B2).replace("stage,\n", "n,\n"))],
  "prag_stage_self": P + [(BODY, "    char path[0x80];\n    int n = stage;\n" + S1 + selfify(B2).replace("stage,\n", "n,\n"))],
}

FMT = '    sprintf(path, "csdFiles/stageTitle/StageTitle%04d%s.txd", stage,\n'
DIR = '    if (fn_8003F938(m_csd, "csdFiles/stageTitle/StageTitle", path)) {\n'
SCN = '        strcpy(m_csd.m_sceneName, "StageTitle");\n'
REPL.update({
  "s_fmt": [(FMT, '    const char *format = "csdFiles/stageTitle/StageTitle%04d%s.txd";\n    sprintf(path, format, stage,\n')],
  "s_dir": [(DIR, '    const char *dir = "csdFiles/stageTitle/StageTitle";\n    if (fn_8003F938(m_csd, dir, path)) {\n')],
  "s_scn": [(SCN, '        const char *scene = "StageTitle";\n        strcpy(m_csd.m_sceneName, scene);\n')],
  "ps_fmt": P + [(FMT, '    const char *format = "csdFiles/stageTitle/StageTitle%04d%s.txd";\n    sprintf(path, format, stage,\n')],
  "ps_dir": P + [(DIR, '    const char *dir = "csdFiles/stageTitle/StageTitle";\n    if (fn_8003F938(m_csd, dir, path)) {\n')],
  "ps_scn": P + [(SCN, '        const char *scene = "StageTitle";\n        strcpy(m_csd.m_sceneName, scene);\n')],
})
