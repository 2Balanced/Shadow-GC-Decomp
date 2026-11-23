"""Wave 40: Move's saved registers (retail: the handling table r31, the state r30, the state before
r29, this r28, then the two addresses)."""
P = "Q27Vehicle15TVehicleCarMove"
SCORE = ["Move__%sFf" % P]
D = "    const CarParam &param = s_params[m_type];\n"; I = "    const CarParam &param = Param();\n"
S = "    State &state = m_states[0];\n"; PR = "    State &previous = m_states[1];\n"
hv = "void TVehicleCarMove::Move(float dt) {\n"
V = hv + PR + S + D
REPL = {
  "dsp": [(V, hv + D + S + PR)],
  "isp": [(V, hv + I + S + PR)],
  "ptr_dsp": [(V, hv + "    const CarParam *paramP = &s_params[m_type];\n#define param (*paramP)\n" + S + PR)],
  "ptr_psd": [(V, hv + PR + S + "    const CarParam *paramP = &s_params[m_type];\n#define param (*paramP)\n")],
  "states": [(V, hv + D + "    State *states = m_states;\n    State &state = states[0];\n    State &previous = states[1];\n")],
}

import os, re
_src = open(os.path.join(os.path.dirname(__file__), "..", "src", "Game", "TVehicleCarMove.cpp"), encoding="utf-8").read()
_i = _src.index(hv); _j = _src.index("\n}\n", _i) + 3
_fn = _src[_i:_j]
def ptr(order):
    b = _fn.replace(PR + S + D, order)
    b = re.sub(r"\bparam\.", "param->", b)
    return [(_fn, b)]
DP = "    const CarParam *param = &s_params[m_type];\n"
IP = "    const CarParam *param = &Param();\n"
REPL = {
  "p_dsp": ptr(DP + S + PR),
  "p_psd": ptr(PR + S + DP),
  "p_isp": ptr(IP + S + PR),
  "p_spd": ptr(S + PR + DP),
  "p_dps": ptr(DP + PR + S),
}
