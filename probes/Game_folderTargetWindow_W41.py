"""Wave 41: folderTargetWindow's constructor (retail: li r28,0 then mr r31/r29/r30 from it) with
the panel loop in an inline function (the DiablonExecuter lever) and/or propagation off."""
SCORE = ["__ct__18folderTargetWindowFPQ34Chao3CSD8CProject"]
H = "folderTargetWindow::folderTargetWindow(Chao::CSD::CProject *project) {\n"
B = """    InitScenes(project);
    for (int i = 0; i < 2; i++) {
        m_panels[i].m_scene.Reset();
        m_panels[i].m_state = 0;
        m_panels[i].m_selected = 0;
    }
}
"""
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"
F1 = """inline void ResetPanels(folderTargetWindow *self) {
    for (int i = 0; i < 2; i++) {
        self->m_panels[i].m_scene.Reset();
        self->m_panels[i].m_state = 0;
        self->m_panels[i].m_selected = 0;
    }
}
"""
F2 = """inline void ResetPanels(folderTargetWindow::Panel *panels) {
    for (int i = 0; i < 2; i++) {
        panels[i].m_scene.Reset();
        panels[i].m_state = 0;
        panels[i].m_selected = 0;
    }
}
"""
F3 = """inline void ResetPanel(folderTargetWindow::Panel *panel) {
    panel->m_scene.Reset();
    panel->m_state = 0;
    panel->m_selected = 0;
}
"""
REPL = {
  "prag": [(H + B, P0 + H + B + P1)],
  "inl_self": [(H + B, F1 + H + "    InitScenes(project);\n    ResetPanels(this);\n}\n")],
  "inl_panels": [(H + B, F2 + H + "    InitScenes(project);\n    ResetPanels(m_panels);\n}\n")],
  "inl_panel": [(H + B, F3 + H + "    InitScenes(project);\n    for (int i = 0; i < 2; i++) {\n        ResetPanel(&m_panels[i]);\n    }\n}\n")],
  "prag_inl_self": [(H + B, F1 + P0 + H + "    InitScenes(project);\n    ResetPanels(this);\n}\n" + P1)],
  "prag_inl_panels": [(H + B, F2 + P0 + H + "    InitScenes(project);\n    ResetPanels(m_panels);\n}\n" + P1)],
}
