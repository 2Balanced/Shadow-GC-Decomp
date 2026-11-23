"""Wave 41: FindScenes with a named copy of this and copy propagation off (the wave 40 lever)."""
SCORE = ["FindScenes__22TPjsEventActionManagerFv"]
SRC = open("src/Game/PjsEventActionManager.cpp", encoding="utf-8").read()
s = SRC.index("void TPjsEventActionManager::FindScenes() {")
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
HEAD = "void TPjsEventActionManager::FindScenes() {\n"
BODY_SELF = """    m_sceneIndex = 0;
    m_state = 0;
    m_time = 0.0f;
    fn_802DFC78(fn_801702E4());
    m_sceneCount = 0;
    m_nextScene = 0;
    for (i = 0; i < 12; i++) {
        sprintf(fileName, FMT, lbl_805EF2A0, (char)('A' + i));
        if (FileExists(fileName)) {
            m_scenes[m_sceneCount] = i;
            m_sceneCount++;
        }
    }
    if (m_sceneCount <= 0) {
        m_state = 8;
    }
}
"""
def selfify(b):
    for m in ["m_sceneIndex", "m_state", "m_time", "m_sceneCount", "m_nextScene", "m_scenes"]:
        b = b.replace(m, "self->" + m)
    return b
F = '"event\\event%04d_scene%c.one"'
V = {}
V["self_last"] = "    char fileName[0x40];\n    int i;\n    TPjsEventActionManager *self = this;\n" + selfify(BODY_SELF).replace("FMT", F)
V["self_first"] = "    TPjsEventActionManager *self = this;\n    char fileName[0x40];\n    int i;\n" + selfify(BODY_SELF).replace("FMT", F)
V["fmt_i_self"] = "    char fileName[0x40];\n    const char *format = " + F + ";\n    int i;\n    TPjsEventActionManager *self = this;\n" + selfify(BODY_SELF).replace("FMT", "format")
V["i_fmt_self"] = "    char fileName[0x40];\n    int i;\n    const char *format = " + F + ";\n    TPjsEventActionManager *self = this;\n" + selfify(BODY_SELF).replace("FMT", "format")
V["fmt_self_i"] = "    char fileName[0x40];\n    const char *format = " + F + ";\n    TPjsEventActionManager *self = this;\n    int i;\n" + selfify(BODY_SELF).replace("FMT", "format")
V["constself_last"] = "    char fileName[0x40];\n    int i;\n    TPjsEventActionManager *const self = this;\n" + selfify(BODY_SELF).replace("FMT", F)
V["fmt_i_constself"] = "    char fileName[0x40];\n    const char *format = " + F + ";\n    int i;\n    TPjsEventActionManager *const self = this;\n" + selfify(BODY_SELF).replace("FMT", "format")
V["plain_i"] = "    char fileName[0x40];\n    int i;\n" + BODY_SELF.replace("FMT", F)
REPL = {}
for k, b in V.items():
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + HEAD + b + "#pragma opt_propagation reset\n")]
    REPL[k] = [(FN, HEAD + b)]
