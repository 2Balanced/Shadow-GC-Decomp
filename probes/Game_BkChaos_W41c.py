"""Wave 41: StretchCallback under propagation off: the index type and the yaw update."""
SCORE = ["StretchCallback__7BkChaosFP19_stMtnCallbackParam"]
H = "static void StretchCallback(_stMtnCallbackParam *param) {\n"
P0 = "#pragma opt_propagation off\n"
I0 = "    int index = param->m_id == 0x1774 ? 0 : 1;\n"
Y0 = """        pitch = angles.x;
        yaw = angles.y;
        float delta = (1.0f - scale) * (index ? 0.0012f : -0.0012f);
        yaw += delta;
"""
def v(idx, yaw=None):
    r = [(H, P0 + H), (I0, idx)]
    if yaw:
        r.append((Y0, yaw))
    return r
NE = "    u32 index = param->m_id != 0x1774;\n"
REPL = {
  "bool_idx": v("    bool index = param->m_id != 0x1774;\n"),
  "u32_ne_tern10": v("    u32 index = param->m_id != 0x1774 ? 1 : 0;\n"),
  "u32_ne_paren": v("    u32 index = (param->m_id != 0x1774);\n"),
  "u32_ne_yaw_nodelta": v(NE, "        pitch = angles.x;\n        yaw = angles.y;\n        yaw += (1.0f - scale) * (index ? 0.0012f : -0.0012f);\n"),
  "u32_ne_yaw_delta_first": v(NE, "        float delta = (1.0f - scale) * (index ? 0.0012f : -0.0012f);\n        pitch = angles.x;\n        yaw = angles.y;\n        yaw += delta;\n"),
  "u32_ne_yaw_sum": v(NE, "        pitch = angles.x;\n        float delta = (1.0f - scale) * (index ? 0.0012f : -0.0012f);\n        yaw = angles.y + delta;\n"),
  "u32_ne_yaw_eq": v(NE, "        pitch = angles.x;\n        yaw = angles.y;\n        float delta = (1.0f - scale) * (index ? 0.0012f : -0.0012f);\n        yaw = yaw + delta;\n"),
  "s32_ne": v("    s32 index = param->m_id != 0x1774;\n"),
}
