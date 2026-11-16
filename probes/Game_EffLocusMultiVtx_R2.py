"""Declaration orders of Impl::Draw's four locals (wave 39, second pass): V vertices, c count,
p pointCount, v vertexCount; all declared first, assigned in the original order."""
import itertools
D = """        RwIm3DVertex *vertices = m_history.Vertices();
        int count = m_history.m_count;
        int pointCount = m_history.m_pointCount;
        int vertexCount = count * 2;
"""
DECL = {"V": "        RwIm3DVertex *vertices;\n", "c": "        int count;\n", "p": "        int pointCount;\n", "v": "        int vertexCount;\n"}
ASG = "        vertices = m_history.Vertices();\n        count = m_history.m_count;\n        pointCount = m_history.m_pointCount;\n        vertexCount = count * 2;\n"
REPL = {}
for perm in itertools.permutations("Vcpv"):
    REPL["o_" + "".join(perm)] = [(D, "".join(DECL[k] for k in perm) + ASG)]
SCORE = ["Draw__Q34Test16EffLocusMultiVtx4ImplFv"]
