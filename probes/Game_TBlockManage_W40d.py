"""TBlockManage::UpdateLoads (wave 40): loops of one pass."""
W = "for (int n = 0; n < 1; n++) {\n"
A = "        id = i;\n"
REPL = {
 "for_id": [(A, "        " + W + "            id = i;\n        }\n")],
 "for_init": [("    bool anyShown = false;\n    s32 loaded = 0;\n    m_loading = 0;\n", "    bool anyShown = false;\n    s32 loaded = 0;\n    " + W + "        m_loading = 0;\n    }\n")],
 "for_init2": [("    bool anyShown = false;\n    s32 loaded = 0;\n    m_loading = 0;\n", "    bool anyShown;\n    s32 loaded;\n    " + W + "        anyShown = false;\n        loaded = 0;\n        m_loading = 0;\n    }\n")],
 "id_decl": [("    int id;\n    s16 i;\n", "    s16 i;\n"), (A, "        int id;\n        " + W + "            id = i;\n        }\n")],
 "id_init_loop": [("    int id;\n    s16 i;\n", "    s16 i;\n"), (A, "        int id = i;\n        " + W + "        }\n")],
}
SCORE = ["UpdateLoads__12TBlockManageFv"]
