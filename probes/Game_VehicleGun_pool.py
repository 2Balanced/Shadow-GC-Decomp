"""Game/VehicleGun.cpp (wave 39): what puts FindAim's 0.5 before MoveSlide's -50, -2 and 15 in
.sdata2 (retail: -1, 0.5, -50, -2, 15, 0.866, 300)."""
SCORE = ["MoveSlide__FP19_stMtnCallbackParam"]
K = "        float speed = -50.0f;\n"
TOP = "    UnkGunSlide *slide = (UnkGunSlide *)param->m_data;\n    Mtx34f matrix;\n    fn_80016370(&matrix, param->m_frame);\n    if (slide->m_kick != 0) {\n"
REPL = {
  "dead_init": [(TOP, TOP.replace("    Mtx34f matrix;", "    float unused = 0.5f;\n    Mtx34f matrix;"))],
  "folded": [(K, "        float speed = -100.0f * 0.5f;\n")],
  "dead_if": [(TOP, TOP.replace("    if (slide->m_kick != 0) {", "    if (0.5f < 0.0f) {\n        return;\n    }\n    if (slide->m_kick != 0) {"))],
  "base": [],
}
