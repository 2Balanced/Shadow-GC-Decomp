"""Wave 41: GadgetModelInfo::fn_80162FB0, retail this r30 and the collider iterator r31 (GC/2.7 this
r31): a copy of this and the iterator declared first, with copy propagation off."""
import re
SCORE = ["fn_80162FB0__15GadgetModelInfoFf"]
_src = open("src/Game/GadgetModelInfo.cpp", encoding="utf-8").read()
HEAD = "void GadgetModelInfo::fn_80162FB0(float dt) {\n"
_s = _src.index(HEAD)
_e = _src.index("\n}\n", _s) + 3
FN = _src[_s:_e]
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"


def selfify(fn):
    b = re.sub(r"(?<![\w>.])m_", "self->m_", fn)
    b = b.replace("reinterpret_cast<s32>(this)", "reinterpret_cast<s32>(self)")
    b = b.replace("(this)", "(self)").replace("(this, ", "(self, ")
    b = b.replace("StopBlinking();", "self->StopBlinking();")
    return b


SELF = "    GadgetModelInfo *self = this;\n"
ITDECL = "    ColliderIter it;\n"
FOR = "        for (ColliderIter it = "
FOR2 = "        for (it = "
S1 = selfify(FN).replace(HEAD, HEAD + SELF, 1)
S2 = selfify(FN).replace(HEAD, HEAD + ITDECL + SELF, 1).replace(FOR, FOR2, 1)
S3 = FN.replace(HEAD, HEAD + ITDECL, 1).replace(FOR, FOR2, 1)
REPL = {
    "prag_self": [(FN, P0 + S1 + P1)],
    "prag_it_self": [(FN, P0 + S2 + P1)],
    "it_self": [(FN, S2)],
    "prag_it": [(FN, P0 + S3 + P1)],
    "it": [(FN, S3)],
}
