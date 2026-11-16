"""Impl::Update of Game/EffBelt.cpp, declarations at the top of the function (wave 39)."""
A = "bool EffBelt::Impl::Update() {\n"
T1 = "    int alpha = m_color.m_a;\n"
T2 = "    s32 count = m_ring.m_count;\n"
T3 = "    BeltPoint *point = m_ring.Get(0);\n"
T4 = "    int i;\n"
T5 = "    int fade = 0;\n"
def top(order, extra=()):
    d = {"a": "    int alpha;\n", "c": "    s32 count;\n", "p": "    BeltPoint *point;\n", "i": "    int i;\n", "f": "    int fade;\n"}
    return [(A, A + "".join(d[k] for k in order)), (T1, "    alpha = m_color.m_a;\n"), (T2, "    count = m_ring.m_count;\n"), (T3, "    point = m_ring.Get(0);\n"), (T4, ""), (T5, "    fade = 0;\n")] + list(extra)
REPL = {}
for o in ["acpif", "acipf", "aicpf", "iacpf", "caipf", "acfip", "ifacp", "pacif", "aifcp", "ciapf", "icapf", "acifp"]:
    REPL["t_" + o] = top(o)
SCORE = ["Update__Q34Test7EffBelt4ImplFv"]
