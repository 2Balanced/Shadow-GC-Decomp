"""Wave 41: GetNodeMatrix (1 row: retail copies the zeroed counter before it copies node)."""
SRC = open("src/Game/PlayerShadowMotionHand.cpp", encoding="utf-8").read()
SCORE = ["GetNodeMatrix__Q36Player6Shadow10MotionHandFi"]
s = SRC.index("/* The first hand animation that has node (name inferred). */")
e = SRC.index("Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    return FindNodeMatrix(m_impl.get(), node);\n}\n") + len("Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    return FindNodeMatrix(m_impl.get(), node);\n}\n")
FN = SRC[s:e]
def helper(sig, body_impl="impl", extra=""):
    return ("inline Mtx34f *FindNodeMatrix(" + sig + ") {\n" + extra +
            "    for (u32 i = 0; i < 5; i++) {\n        if (" + body_impl + "->m_anims[i] != 0) {\n"
            "            Mtx34f *matrix = fn_80415E70(" + body_impl + "->m_anims[i], node);\n"
            "            if (matrix != 0) {\n                return matrix;\n            }\n        }\n    }\n    return 0;\n}\n\n")
GET = "Mtx34f *MotionHand::GetNodeMatrix(int node) {\n    return FindNodeMatrix(m_impl.get(), node);\n}\n"
V = {
 "constimpl": helper("const MotionHandImpl *impl, int node") + GET,
 "noderef": helper("MotionHandImpl *impl, const int &node") + GET,
 "nodeu32": helper("MotionHandImpl *impl, u32 node") + GET,
 "direct_prag": "#pragma opt_propagation off\nMtx34f *MotionHand::GetNodeMatrix(int node) {\n    MotionHandImpl *impl = m_impl.get();\n    for (u32 i = 0; i < 5; i++) {\n        if (impl->m_anims[i] != 0) {\n            Mtx34f *matrix = fn_80415E70(impl->m_anims[i], node);\n            if (matrix != 0) {\n                return matrix;\n            }\n        }\n    }\n    return 0;\n}\n#pragma opt_propagation reset\n",
 "helper_prag": "#pragma opt_propagation off\n" + helper("MotionHandImpl *impl, int node") + GET + "#pragma opt_propagation reset\n",
 "impl_in_loop": helper("MotionHandImpl *impl, int node") + GET.replace("return FindNodeMatrix(m_impl.get(), node);", "MotionHandImpl *impl = m_impl.get();\n    return FindNodeMatrix(impl, node);"),
 "member_get": helper("MotionHandImpl *impl, int node") + GET.replace("m_impl.get()", "&*m_impl"),
}
REPL = {k: [(FN, v)] for k, v in V.items()}
