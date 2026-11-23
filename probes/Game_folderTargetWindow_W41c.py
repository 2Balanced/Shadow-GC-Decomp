"""Wave 41: folderTargetWindow::Update's busy loop in an inline function, declaration orders."""
SCORE = ["Update__18folderTargetWindowFv"]
H = "bool folderTargetWindow::Update() {\n"
P0 = "#pragma opt_propagation off\n"
C0 = """                    UnkPlayerManager_8006D6A8 *manager = fn_8006D6A8();
                    if (manager->m_pausePlayer == mission->m_player) {
"""
C2 = """                    UnkPlayerManager_8006D6A8 *manager = fn_8006D6A8();
                    if (manager->m_pausePlayer != mission->m_player) {
"""
L = """    int busy = 0;
    for (i = 0; i < 2; i++) {
        busy += m_panels[i].m_scene.Update(false);
    }
    return busy != 0;
"""
MPL = """                    UnkPlayerManager_8006D6A8 *manager = fn_8006D6A8();
                    s32 player = mission->m_player;
                    if (manager->m_pausePlayer OP player) {
"""
CMP = [(C0, MPL.replace("OP", "==")), (C2, MPL.replace("OP", "!="))]
def inl(body, call="    return UpdatePanels_(this) != 0;\n"):
    return [(L, call), (H, "inline int UpdatePanels_(folderTargetWindow *self) {\n" + body + "}\n" + P0 + H)] + CMP
REPL = {
  "i_first": inl("    int i;\n    int busy = 0;\n    for (i = 0; i < 2; i++) {\n        busy += self->m_panels[i].m_scene.Update(false);\n    }\n    return busy;\n"),
  "i_first_nobusyinit": inl("    int i;\n    int busy;\n    busy = 0;\n    for (i = 0; i < 2; i++) {\n        busy += self->m_panels[i].m_scene.Update(false);\n    }\n    return busy;\n"),
  "bool_ret": inl("    int busy = 0;\n    for (int i = 0; i < 2; i++) {\n        busy += self->m_panels[i].m_scene.Update(false);\n    }\n    return busy != 0;\n", "    return UpdatePanels_(this);\n"),
  "bool_ret_i_first": inl("    int i;\n    int busy = 0;\n    for (i = 0; i < 2; i++) {\n        busy += self->m_panels[i].m_scene.Update(false);\n    }\n    return busy != 0;\n", "    return UpdatePanels_(this);\n"),
  "busy_param": [(L, "    int busy = 0;\n    AddBusy_(this, busy);\n    return busy != 0;\n"), (H, "inline void AddBusy_(folderTargetWindow *self, int &busy) {\n    for (int i = 0; i < 2; i++) {\n        busy += self->m_panels[i].m_scene.Update(false);\n    }\n}\n" + P0 + H)] + CMP,
}
