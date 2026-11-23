"""Wave 40: ArkCannon::Aim with fn_8040F8FC declared as returning float (as Game/Helicopter.cpp;
kept), then the order of the angle's and the step's declarations: retail keeps the rounded angle
in f4 and the step in f0.  The score counts every bl row too (the symbol's name confuses the
masking), so compare variants with one another: 33 is the count of the calls alone."""
SCORE = ["Aim__9ArkCannonFQ25boost21shared_ptr<8CharInfo>f"]
# every variant assumes the float declaration:
_F = ("double fn_8040F8FC(const Vec3f *a, const Vec3f *b);", "float fn_8040F8FC(const Vec3f *a, const Vec3f *b);")
A = "    float angle = fn_8040F8FC(&front, &direction);\n    float step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n"
T = "    if (angle < step) {\n        aimed = true;\n    }\n"
REPL = {
    "base": [(A, A)],
    "step_decl_first": [(A, "    float step;\n    float angle = fn_8040F8FC(&front, &direction);\n    step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n")],
    "both_decl": [(A, "    float angle;\n    float step;\n    angle = fn_8040F8FC(&front, &direction);\n    step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n")],
    "both_decl_rev": [(A, "    float step;\n    float angle;\n    angle = fn_8040F8FC(&front, &direction);\n    step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n")],
    "not_ge": [(A + T, A + "    if (!(angle >= step)) {\n        aimed = true;\n    }\n")],
    "gt": [(A + T, A + "    if (step > angle) {\n        aimed = true;\n    }\n")],
    "const_step": [(A, "    float angle = fn_8040F8FC(&front, &direction);\n    const float step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n")],
    "degtorad": [(A, "    float angle = fn_8040F8FC(&front, &direction);\n    float step = (45.0f * dt) * (lbl_805FAD38 / 180.0f);\n")],
    "step_rate": [(A, "    float angle = fn_8040F8FC(&front, &direction);\n    float rate = 45.0f * dt;\n    float step = rate * (lbl_805FAD38 / 180.0f);\n")],
}
REPL = {k: [_F] + v for k, v in REPL.items()}
