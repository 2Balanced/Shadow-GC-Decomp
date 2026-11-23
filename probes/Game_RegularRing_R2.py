"""Wave 40: Ring::LayoutRings, the liner step in f30 (GC/2.7: f31)."""
SCORE = ["LayoutRings__Q211RegularRing4RingFPCQ211RegularRing10RingParams"]
C1 = "    case 1: {\n        float step = params->param0 / m_count;\n"
C3 = "        float angleStep;\n        float step = params->param0 / m_count;\n        angleStep = (180.0f / m_count) * (lbl_805FAD38 / 180.0f);\n        float height = params->param1;\n"
SW = "    switch (params->type) {\n    case 0:\n        for (int i = 0; i < m_count; i++) {\n            if (m_elements[i].m_flags.Test(1)) {\n                fn_800091D0(&m_elements[i].m_position, &m_chara->m_position);"
REPL = {
  "dummy": [(C1, "    case 1: {\n        float angleStep;\n        float step = params->param0 / m_count;\n")],
  "dummy_init": [(C1, "    case 1: {\n        float angleStep = 0.0f;\n        float step = params->param0 / m_count;\n")],
  "top_step": [(SW, "    float angleStep;\n    float step;\n" + SW), (C1, "    case 1: {\n        step = params->param0 / m_count;\n"), (C3, "        step = params->param0 / m_count;\n        angleStep = (180.0f / m_count) * (lbl_805FAD38 / 180.0f);\n        float height = params->param1;\n")],
  "top_step1": [(SW, "    float angleStep;\n    float step;\n" + SW), (C1, "    case 1: {\n        step = params->param0 / m_count;\n")],
  "top_all": [(SW, "    float angleStep;\n    float step;\n    float height;\n" + SW), (C1, "    case 1: {\n        step = params->param0 / m_count;\n"), (C3, "        step = params->param0 / m_count;\n        angleStep = (180.0f / m_count) * (lbl_805FAD38 / 180.0f);\n        height = params->param1;\n")],
  "neg_local": [(C1, "    case 1: {\n        float step = params->param0 / m_count;\n        float back = -step;\n"), ("                offset.z = -step * i;", "                offset.z = back * i;")],
  "neg_first": [(C1, "    case 1: {\n        float back;\n        float step = params->param0 / m_count;\n        back = -step;\n"), ("                offset.z = -step * i;", "                offset.z = back * i;")],
  "neg_div": [(C1, "    case 1: {\n        float step = -(params->param0 / m_count);\n"), ("                offset.z = -step * i;", "                offset.z = step * i;")],
  "length": [(C1, "    case 1: {\n        float length = params->param0;\n        float step = length / m_count;\n")],
  "count": [(C1, "    case 1: {\n        float count = m_count;\n        float step = params->param0 / count;\n")],
  "both": [(C1, "    case 1: {\n        float length = params->param0;\n        float count = m_count;\n        float step = length / count;\n")],
  "step_late": [(C1, "    case 1: {\n        float step;\n"), ("        fn_8040B80C(&matrix, m_chara->m_rotation.z);\n        Vec3f offset(0.0f, 0.0f, 0.0f);\n        for (int i = 0; i < m_count; i++) {\n            if (m_elements[i].m_flags.Test(1)) {\n                offset.z = -step * i;", "        fn_8040B80C(&matrix, m_chara->m_rotation.z);\n        Vec3f offset(0.0f, 0.0f, 0.0f);\n        step = params->param0 / m_count;\n        for (int i = 0; i < m_count; i++) {\n            if (m_elements[i].m_flags.Test(1)) {\n                offset.z = -step * i;")],
  "z_local": [("                offset.z = -step * i;", "                float z = -step * i;\n                offset.z = z;")],
  "step_i": [("                offset.z = -step * i;", "                offset.z = -(step * i);")],
  "inl_step": [("void Ring::LayoutRings(const RingParams *params) {", "inline float StepOf_(float length, int count) { return length / count; }\nvoid Ring::LayoutRings(const RingParams *params) {"), (C1, "    case 1: {\n        float step = StepOf_(params->param0, m_count);\n")],
  "share_radius": [(SW, "    float length;\n" + SW), (C1, "    case 1: {\n        length = params->param0 / m_count;\n"), ("                offset.z = -step * i;", "                offset.z = -length * i;"), ("        float radius = params->param0;\n", "        length = params->param0;\n"), ("        Vec3f offset(0.0f, 0.0f, radius);", "        Vec3f offset(0.0f, 0.0f, length);")],
  "share_angle3": [(SW, "    float value;\n" + SW), (C1, "    case 1: {\n        value = params->param0 / m_count;\n"), ("                offset.z = -step * i;", "                offset.z = -value * i;"), (C3, "        float step = params->param0 / m_count;\n        value = (180.0f / m_count) * (lbl_805FAD38 / 180.0f);\n        float height = params->param1;\n"), ("                offset.x = height * std::sin(angleStep * i);", "                offset.x = height * std::sin(value * i);")],
  "share_step3": [(SW, "    float step;\n" + SW), (C1, "    case 1: {\n        step = params->param0 / m_count;\n"), (C3, "        float angleStep;\n        step = params->param0 / m_count;\n        angleStep = (180.0f / m_count) * (lbl_805FAD38 / 180.0f);\n        float height = params->param1;\n")],
}
