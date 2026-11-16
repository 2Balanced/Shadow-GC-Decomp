"""Source shapes for folderTargetWindow's constructor (wave 39): retail zeroes the counter first
(li r28,0) and copies it into the offset and the two stored zeroes; GC/2.7 zeroed the offset
first and the counter separately."""
SYMBOLS = {"__ct__18folderTargetWindowFPQ34Chao3CSD8CProject": "folderTargetWindow::folderTargetWindow(Chao::CSD::CProject *project) {"}
B = "        m_panels[i].m_scene.Reset();\n        m_panels[i].m_state = 0;\n        m_panels[i].m_selected = 0;\n    }\n"
BODIES = {"__ct__18folderTargetWindowFPQ34Chao3CSD8CProject": {
    "i_stmt": "    InitScenes(project);\n    int i = 0;\n    for (; i < 2; i++) {\n" + B,
    "i_decl_assign": "    InitScenes(project);\n    int i;\n    i = 0;\n    for (; i < 2; i++) {\n" + B,
    "while": "    InitScenes(project);\n    int i = 0;\n    while (i < 2) {\n" + B.replace("    }\n", "        i++;\n    }\n"),
    "do_while": "    InitScenes(project);\n    int i = 0;\n    do {\n" + B.replace("    }\n", "        i++;\n    } while (i < 2);\n"),
    "i_top": "    int i;\n    InitScenes(project);\n    for (i = 0; i < 2; i++) {\n" + B,
    "panel_ptr": "    InitScenes(project);\n    for (int i = 0; i < 2; i++) {\n        Panel *panel = &m_panels[i];\n        panel->m_scene.Reset();\n        panel->m_state = 0;\n        panel->m_selected = 0;\n    }\n",
    "long_i": "    InitScenes(project);\n    for (s32 i = 0; i < 2; i++) {\n" + B,
    "zero_from_i": "    InitScenes(project);\n    for (int i = 0; i < 2; i++) {\n        m_panels[i].m_scene.Reset();\n        m_panels[i].m_state = 0;\n        m_panels[i].m_selected = false;\n    }\n",
}}
