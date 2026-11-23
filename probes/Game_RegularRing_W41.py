"""Wave 41: Ring::LayoutRings, the liner step in f30 (GC/2.7: f31), with copy propagation off."""
SCORE = ["LayoutRings__Q211RegularRing4RingFPCQ211RegularRing10RingParams"]
F = "void Ring::LayoutRings(const RingParams *params) {\n"
N = "void Ring::CheckContacts() {\n"
PRAG = [(F, "#pragma opt_propagation off\n" + F), (N, "#pragma opt_propagation reset\n" + N)]
C1 = "    case 1: {\n        float step = params->param0 / m_count;\n"
Z = "                offset.z = -step * i;"


def v(*extra, prag=True):
    return list(extra) + (PRAG if prag else [])


REPL = {
    "prag": v(),
    "prag_back": v((C1, C1 + "        float back = -step;\n"), (Z, "                offset.z = back * i;")),
    "prag_back_first": v((C1, "    case 1: {\n        float back;\n        float step = params->param0 / m_count;\n        back = -step;\n"),
                         (Z, "                offset.z = back * i;")),
    "prag_params": v((F, F + "    const RingParams *p = params;\n")),
    "prag_self": v((F, F + "    Ring *self = this;\n")),
    "prag_len": v((C1, "    case 1: {\n        float length = params->param0;\n        float step = length / m_count;\n")),
    "prag_cnt": v((C1, "    case 1: {\n        float count = m_count;\n        float step = params->param0 / count;\n")),
    "prag_copy": v((C1, C1 + "        float s = step;\n"), (Z, "                offset.z = -s * i;")),
    "prag_copy_first": v((C1, "    case 1: {\n        float s;\n        float step = params->param0 / m_count;\n        s = step;\n"),
                         (Z, "                offset.z = -s * i;")),
}
