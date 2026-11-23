"""Wave 41: PsychoObjectControl::Tick, retail keeps the object total in r31 and this in r30 (GC/2.7
the other way round): a named copy of this with copy propagation off (the wave 40 lever)."""
import re
SCORE = ["Tick__37DevilDoomCtrlUnit_PsychoObjectControlFf"]
_src = open("src/Game/DevilDoomCtrlUnitPsychoObject.cpp", encoding="utf-8").read()
_start = _src.index("int DevilDoomCtrlUnit_PsychoObjectControl::Tick(float dt) {\n")
_end = _src.index("\n}\n", _start) + 3
FN = _src[_start:_end]
NEXT = _src[_end:_end + 200].split("\n")[1] + "\n" if False else None


def selfify(fn, decl_after=None):
    body = fn
    body = re.sub(r"(?<![\w>.])m_owner", "self->m_owner", body)
    body = body.replace("field_0xC[0]", "self->field_0xC[0]")
    body = body.replace("    UpdateAngles();", "    self->UpdateAngles();")
    body = body.replace("0x8136, 0, this, 2", "0x8136, 0, self, 2")
    return body


HEAD = "int DevilDoomCtrlUnit_PsychoObjectControl::Tick(float dt) {\n"
SELF = "    DevilDoomCtrlUnit_PsychoObjectControl *self = this;\n"
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"
A = selfify(FN).replace(HEAD, HEAD + SELF, 1)
B = selfify(FN).replace(HEAD, HEAD + "    int total;\n" + SELF, 1).replace("        int total = 0;\n", "        total = 0;\n", 1)
C = FN.replace(HEAD, HEAD + "    int total;\n", 1).replace("        int total = 0;\n", "        total = 0;\n", 1)
REPL = {
    "prag_self": [(FN, P0 + A + P1)],
    "self_noprag": [(FN, A)],
    "prag_total_self": [(FN, P0 + B + P1)],
    "total_top": [(FN, C)],
    "prag_total_top": [(FN, P0 + C + P1)],
    "prag": [(FN, P0 + FN + P1)],
}
