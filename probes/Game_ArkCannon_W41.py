"""Wave 41: ArkCannon::Aim with copy propagation off (the wave-40 lever), double and float returns
of fn_8040F8FC, and named copies of dt and of the angle.  Scores count the bl rows too (33 of
them in every variant), so 33 means zero."""
SCORE = ["Aim__9ArkCannonFQ25boost21shared_ptr<8CharInfo>f"]
_F = ("double fn_8040F8FC(const Vec3f *a, const Vec3f *b);", "float fn_8040F8FC(const Vec3f *a, const Vec3f *b);")
F = "bool ArkCannon::Aim(boost::shared_ptr<CharInfo> enemy, float dt) {"
END = "void ArkCannon::Fire(u32 index, boost::shared_ptr<CharInfo> enemy) {"
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
A = "    float angle = fn_8040F8FC(&front, &direction);\n    float step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n"
T = "    if (angle < step) {\n        aimed = true;\n    }\n"
CALL = "    angle = fn_800704AC(angle, (float)step);\n"
REPL = {
    "base": [],
    "prag": P,
    "prag_f": P + [_F],
    "prag_dtcopy": P + [(F, F + "\n    float time = dt;"), ("45.0f * dt *", "45.0f * time *")],
    "prag_f_dtcopy": P + [_F, (F, F + "\n    float time = dt;"), ("45.0f * dt *", "45.0f * time *")],
    "prag_nocast": P + [(CALL, "    angle = fn_800704AC(angle, step);\n")],
    "prag_f_nocast": P + [_F, (CALL, "    angle = fn_800704AC(angle, step);\n")],
    "nocast": [(CALL, "    angle = fn_800704AC(angle, step);\n")],
    "f_nocast": [_F, (CALL, "    angle = fn_800704AC(angle, step);\n")],
    "prag_tmp": P + [(A, "    float angle;\n    float step;\n    angle = fn_8040F8FC(&front, &direction);\n    step = 45.0f * dt * (lbl_805FAD38 / 180.0f);\n")],
}
