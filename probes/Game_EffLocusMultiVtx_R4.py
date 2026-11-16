"""Impl::Draw: forms of the vertex count on the p, c, V and c, p, V declaration orders (wave 39)."""
D = """        RwIm3DVertex *vertices = m_history.Vertices();
        int count = m_history.m_count;
        int pointCount = m_history.m_pointCount;
        int vertexCount = count * 2;
"""
OLDC = "fn_8049A82C(vertices, vertexCount, 0, 0x18)"
ASG = "        vertices = m_history.Vertices();\n        count = m_history.m_count;\n        pointCount = m_history.m_pointCount;\n"
B = {"pcV": "        int pointCount;\n        int count;\n        RwIm3DVertex *vertices;\n" + ASG,
     "cpV": "        int count;\n        int pointCount;\n        RwIm3DVertex *vertices;\n" + ASG,
     "Vcp": "        RwIm3DVertex *vertices = m_history.Vertices();\n        int count = m_history.m_count;\n        int pointCount = m_history.m_pointCount;\n"}
E = {"mem": "m_history.m_count * 2", "shl": "count << 1", "add": "count + count", "u": "(u32)count * 2", "memadd": "count + m_history.m_count"}
REPL = {}
for b in B:
    for e in E:
        REPL[b + "_" + e] = [(D, B[b]), (OLDC, "fn_8049A82C(vertices, " + E[e] + ", 0, 0x18)")]
    REPL[b + "_vlate"] = [(D, B[b]), ("            if (i > 0) {\n                if (" + OLDC, "            if (i > 0) {\n                int vertexCount = count * 2;\n                if (" + OLDC)]
    REPL[b + "_ucount"] = [(D, B[b].replace("int count", "u32 count") + "        int vertexCount = count * 2;\n")]
SCORE = ["Draw__Q34Test16EffLocusMultiVtx4ImplFv"]
