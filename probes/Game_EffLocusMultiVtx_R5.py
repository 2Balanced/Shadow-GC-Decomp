"""Impl::Draw: the history's counts read through inline getters (wave 39, second pass)."""
import itertools
D = """        RwIm3DVertex *vertices = m_history.Vertices();
        int count = m_history.m_count;
        int pointCount = m_history.m_pointCount;
        int vertexCount = count * 2;
"""
G = ("    float &Alpha(int age) {\n", "    int Count() const {\n        return m_count;\n    }\n    int PointCount() const {\n        return m_pointCount;\n    }\n    int VertexCount() const {\n        return m_count * 2;\n    }\n    float &Alpha(int age) {\n")
L = {"V": "        RwIm3DVertex *vertices = m_history.Vertices();\n", "c": "        int count = m_history.Count();\n",
     "p": "        int pointCount = m_history.PointCount();\n", "v": "        int vertexCount = count * 2;\n"}
REPL = {}
for perm in ["Vcpv", "Vcvp", "cVpv", "cpVv", "cpvV", "cvpV", "cvVp", "pcvV", "pcVv", "cVvp"]:
    REPL["g_" + perm] = [G, (D, "".join(L[k] for k in perm))]
REPL["g_vc_Vcp"] = [G, (D, L["V"] + L["c"] + L["p"] + "        int vertexCount = m_history.VertexCount();\n")]
REPL["g_vc_cpvV"] = [G, (D, L["c"] + L["p"] + "        int vertexCount = m_history.VertexCount();\n" + L["V"])]
REPL["g_vc_Vvcp"] = [G, (D, L["V"] + "        int vertexCount = m_history.VertexCount();\n" + L["c"] + L["p"])]
SCORE = ["Draw__Q34Test16EffLocusMultiVtx4ImplFv"]
