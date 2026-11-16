"""Impl::Draw without a local for the vertex count (wave 39, second pass)."""
import itertools
D = """        RwIm3DVertex *vertices = m_history.Vertices();
        int count = m_history.m_count;
        int pointCount = m_history.m_pointCount;
        int vertexCount = count * 2;
"""
CALL = ("fn_8049A82C(vertices, vertexCount, 0, 0x18)", "fn_8049A82C(vertices, count * 2, 0, 0x18)")
DECL = {"V": "        RwIm3DVertex *vertices;\n", "c": "        int count;\n", "p": "        int pointCount;\n"}
ASG = "        vertices = m_history.Vertices();\n        count = m_history.m_count;\n        pointCount = m_history.m_pointCount;\n"
REPL = {"n_init": [(D, "        RwIm3DVertex *vertices = m_history.Vertices();\n        int count = m_history.m_count;\n        int pointCount = m_history.m_pointCount;\n"), CALL]}
for perm in itertools.permutations("Vcp"):
    REPL["n_" + "".join(perm)] = [(D, "".join(DECL[k] for k in perm) + ASG), CALL]
SCORE = ["Draw__Q34Test16EffLocusMultiVtx4ImplFv"]
