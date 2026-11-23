"""Wave 41: fn_80049778, retail schedules y * z after the first store (GC/2.7: before it)."""
SCORE = ["fn_80049778"]
F = "extern \"C\" void fn_80049778(Mtx34f *m, const Vec3f *axis, float angle) {\n"
T = "    float t = 1.0f - c;\n    m->values[0][0]"
R12 = "    m->values[1][2] = t * (y * z) - x * s;\n"
R21 = "    m->values[2][1] = t * (y * z) + x * s;\n"
REPL = {
    "yz_top": [(T, "    float t = 1.0f - c;\n    float yz = y * z;\n    m->values[0][0]"),
               (R12, "    m->values[1][2] = t * yz - x * s;\n"), (R21, "    m->values[2][1] = t * yz + x * s;\n")],
    "prods_top": [(T, "    float t = 1.0f - c;\n    float xy = x * y;\n    float yz = y * z;\n    float zx = z * x;\n    m->values[0][0]"),
                  ("    m->values[0][1] = t * (x * y) - z * s;\n", "    m->values[0][1] = t * xy - z * s;\n"),
                  ("    m->values[0][2] = t * (z * x) + y * s;\n", "    m->values[0][2] = t * zx + y * s;\n"),
                  ("    m->values[1][0] = t * (x * y) + z * s;\n", "    m->values[1][0] = t * xy + z * s;\n"),
                  (R12, "    m->values[1][2] = t * yz - x * s;\n"), (R21, "    m->values[2][1] = t * yz + x * s;\n"),
                  ("    m->values[2][0] = t * (z * x) - y * s;\n", "    m->values[2][0] = t * zx - y * s;\n")],
    "zy21": [(R21, "    m->values[2][1] = t * (z * y) + x * s;\n")],
    "zy12": [(R12, "    m->values[1][2] = t * (z * y) - x * s;\n")],
    "yz_t": [(R12, "    m->values[1][2] = (y * z) * t - x * s;\n"), (R21, "    m->values[2][1] = (y * z) * t + x * s;\n")],
    "ty_z": [(R12, "    m->values[1][2] = t * y * z - x * s;\n"), (R21, "    m->values[2][1] = t * y * z + x * s;\n")],
    "sc_order": [("    float s = std::sin(angle);\n    float c = std::cos(angle);\n", "    float c;\n    float s = std::sin(angle);\n    c = std::cos(angle);\n")],
    "xyz_late": [("    float x = axis->x;\n    float y = axis->y;\n    float z = axis->z;\n    float s = std::sin(angle);\n    float c = std::cos(angle);\n",
                  "    float s = std::sin(angle);\n    float c = std::cos(angle);\n    float x = axis->x;\n    float y = axis->y;\n    float z = axis->z;\n")],
    "xs_names": [(T, "    float t = 1.0f - c;\n    float xs = x * s;\n    float ys = y * s;\n    float zs = z * s;\n    m->values[0][0]"),
                 ("    m->values[0][1] = t * (x * y) - z * s;\n", "    m->values[0][1] = t * (x * y) - zs;\n"),
                 ("    m->values[0][2] = t * (z * x) + y * s;\n", "    m->values[0][2] = t * (z * x) + ys;\n"),
                 ("    m->values[1][0] = t * (x * y) + z * s;\n", "    m->values[1][0] = t * (x * y) + zs;\n"),
                 (R12, "    m->values[1][2] = t * (y * z) - xs;\n"), (R21, "    m->values[2][1] = t * (y * z) + xs;\n"),
                 ("    m->values[2][0] = t * (z * x) - y * s;\n", "    m->values[2][0] = t * (z * x) - ys;\n")],
}
