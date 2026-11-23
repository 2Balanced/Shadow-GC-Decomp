"""fn_801ABDB8 (wave 40): the stage indices read in loops of one pass."""
O = """        int index = path->m_stages[k];
        line.m_from = StageInfo(index).m_id;
        index = path->m_stages[k + 1];
        line.m_to = StageInfo(index).m_id;
"""
W = "for (int n = 0; n < 1; n++) {\n"
REPL = {
 "both": [(O, "        int index;\n        " + W + "            index = path->m_stages[k];\n        }\n        line.m_from = StageInfo(index).m_id;\n        " + W + "            index = path->m_stages[k + 1];\n        }\n        line.m_to = StageInfo(index).m_id;\n")],
 "first": [(O, "        int index;\n        " + W + "            index = path->m_stages[k];\n        }\n        line.m_from = StageInfo(index).m_id;\n        index = path->m_stages[k + 1];\n        line.m_to = StageInfo(index).m_id;\n")],
 "second": [(O, "        int index = path->m_stages[k];\n        line.m_from = StageInfo(index).m_id;\n        " + W + "            index = path->m_stages[k + 1];\n        }\n        line.m_to = StageInfo(index).m_id;\n")],
 "two": [(O, "        int from;\n        int to;\n        " + W + "            from = path->m_stages[k];\n        }\n        line.m_from = StageInfo(from).m_id;\n        " + W + "            to = path->m_stages[k + 1];\n        }\n        line.m_to = StageInfo(to).m_id;\n")],
 "second_new": [(O, "        line.m_from = StageInfo(path->m_stages[k]).m_id;\n        int to;\n        " + W + "            to = path->m_stages[k + 1];\n        }\n        line.m_to = StageInfo(to).m_id;\n")],
 "first_new": [(O, "        int from;\n        " + W + "            from = path->m_stages[k];\n        }\n        line.m_from = StageInfo(from).m_id;\n        line.m_to = StageInfo(path->m_stages[k + 1]).m_id;\n")],
 "loop2": [(O, "        s32 *ids = &line.m_from;\n        for (int n = 0; n < 2; n++) {\n            ids[n] = StageInfo(path->m_stages[k + n]).m_id;\n        }\n")],
 "wrap": [(O, "        " + W + O.replace("        ", "            ") + "        }\n")],
}
SCORE = ["fn_801ABDB8"]
