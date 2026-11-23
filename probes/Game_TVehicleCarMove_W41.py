"""Wave 41: ReadInput's Smooth, retail adds rate + 1.0f with the rate first."""
SCORE = ["ReadInput__Q27Vehicle15TVehicleCarMoveFf"]
OLD = "        float d = value - target;\n        value = target + (d * rate) / (rate + 1.0f);\n"
REPL = {
    "one_named": [(OLD, "        float one = 1.0f;\n        float d = value - target;\n        value = target + (d * rate) / (rate + one);\n")],
    "one_swapped": [(OLD, "        float d = value - target;\n        value = target + (d * rate) / (1.0f + rate);\n")],
    "denom": [(OLD, "        float d = value - target;\n        float denom = rate + 1.0f;\n        value = target + (d * rate) / denom;\n")],
    "denom_first": [(OLD, "        float denom = rate + 1.0f;\n        float d = value - target;\n        value = target + (d * rate) / denom;\n")],
    "one_const": [(OLD, "        const float one = 1.0f;\n        float d = value - target;\n        value = target + (d * rate) / (rate + one);\n")],
    "one_named_after": [(OLD, "        float d = value - target;\n        float one = 1.0f;\n        value = target + (d * rate) / (rate + one);\n")],
}
ONE = (OLD, "        float one = 1.0f;\n        float d = value - target;\n        value = target + (d * rate) / (rate + one);\n")
ST = "    Input &input = m_states[0].m_input;\n    float stick = Pad(m_pad).m_stickX;\n    Smooth(input.m_steer, stick, s_steerRate);\n"
REPL.update({
    "one_nostick": [ONE, (ST, "    Input &input = m_states[0].m_input;\n    Smooth(input.m_steer, Pad(m_pad).m_stickX, s_steerRate);\n")],
    "one_stick_first": [ONE, (ST, "    float stick = Pad(m_pad).m_stickX;\n    Input &input = m_states[0].m_input;\n    Smooth(input.m_steer, stick, s_steerRate);\n")],
    "one_rate_local": [ONE, (ST, "    Input &input = m_states[0].m_input;\n    float stick = Pad(m_pad).m_stickX;\n    float rate = s_steerRate;\n    Smooth(input.m_steer, stick, rate);\n")],
    "one_d_after": [(OLD, "        float one = 1.0f;\n        value = target + ((value - target) * rate) / (rate + one);\n")],
})
