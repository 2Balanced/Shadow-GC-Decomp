"""Wave 40: declaration order of the state and the handling-table reference, with the table read
directly or through Param()."""
P = "Q27Vehicle15TVehicleCarMove"
SCORE = ["Move__%sFf" % P, "UpdateBoost__%sFf" % P, "UpdateMotion__%sFf" % P]
M = "void TVehicleCarMove::UpdateMotion(float dt) {\n    const CarParam &param = s_params[m_type];\n    State &state = m_states[0];\n"
B = "void TVehicleCarMove::UpdateBoost(float dt) {\n    const CarParam &param = Param();\n    State &state = m_states[0];\n"
V = "void TVehicleCarMove::Move(float dt) {\n    State &state = m_states[0];\n    State &previous = m_states[1];\n    const CarParam &param = s_params[m_type];\n"
D = "    const CarParam &param = s_params[m_type];\n"; I = "    const CarParam &param = Param();\n"
S = "    State &state = m_states[0];\n"; PR = "    State &previous = m_states[1];\n"
hm = "void TVehicleCarMove::UpdateMotion(float dt) {\n"; hb = "void TVehicleCarMove::UpdateBoost(float dt) {\n"; hv = "void TVehicleCarMove::Move(float dt) {\n"
REPL = {
  "sd": [(M, hm + S + D), (B, hb + S + D), (V, hv + D + S + PR)],
  "si": [(M, hm + S + I), (B, hb + S + I), (V, hv + I + S + PR)],
  "ds": [(B, hb + D + S), (V, hv + S + D + PR)],
  "is": [(M, hm + I + S), (V, hv + S + I + PR)],
  "v_spi": [(V, hv + S + PR + I)],
  "v_psd": [(V, hv + PR + S + D)],
  "v_pds": [(V, hv + PR + D + S)],
  "v_dps": [(V, hv + D + PR + S)],
}
