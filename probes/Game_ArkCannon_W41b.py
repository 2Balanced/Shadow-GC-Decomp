"""Wave 41 round 2: ArkCannon::Aim with the float return, other spellings of the step and of the
angle's comparison (retail: pi f2, 180 f0, 45 f3, rounded angle f4, step f0).  33 = zero."""
SCORE = ["Aim__9ArkCannonFQ25boost21shared_ptr<8CharInfo>f"]
_F = ("double fn_8040F8FC(const Vec3f *a, const Vec3f *b);", "float fn_8040F8FC(const Vec3f *a, const Vec3f *b);")
A = "    float angle = fn_8040F8FC(&front, &direction);\n    float step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n"
T = "    if (angle < step) {\n        aimed = true;\n    }\n"
CALL = "    angle = fn_800704AC(angle, (float)step);\n"
REPL = {
    "rad_local": [(A, "    float angle = fn_8040F8FC(&front, &direction);\n    float rad = lbl_805FAD38 / 180.0f;\n    float step = 45.0f * dt * rad;\n")],
    "rad_first": [(A, "    float rad = lbl_805FAD38 / 180.0f;\n    float angle = fn_8040F8FC(&front, &direction);\n    float step = 45.0f * dt * rad;\n")],
    "const_angle_tmp": [(A + T, "    float angle = fn_8040F8FC(&front, &direction);\n    float step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n    if ((float)angle < step) {\n        aimed = true;\n    }\n")],
    "cmp_local": [(A + T, "    float angle = fn_8040F8FC(&front, &direction);\n    float diff = angle;\n    float step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n    if (diff < step) {\n        aimed = true;\n    }\n")],
    "step_first_cmp": [(A + T, "    float step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n    float angle = fn_8040F8FC(&front, &direction);\n    if (angle < step) {\n        aimed = true;\n    }\n")],
    "aimed_assign": [(A + T, A + "    aimed = angle < step;\n")],
    "dt_first": [(A, "    float angle = fn_8040F8FC(&front, &direction);\n    float step = dt * 45.0f * (lbl_805FAD38 / 180.0f);\n")],
    "step_dbl": [(A, "    float angle = fn_8040F8FC(&front, &direction);\n    float step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n"), ("    angle = fn_800704AC(angle, (float)step);\n", "    angle = fn_800704AC(angle, step);\n")],
}
REPL = {k: [_F] + v for k, v in REPL.items()}
