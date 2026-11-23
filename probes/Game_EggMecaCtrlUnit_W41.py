"""Wave 41: FixLookCharCamera::IsEnabled with copy propagation off and named copies (the wave-40
lever): retail numbers the interpolations' constants before the difference."""
SCORE = ["IsEnabled__33EggMecaCtrlUnit_FixLookCharCameraFv"]
F = "bool EggMecaCtrlUnit_FixLookCharCamera::IsEnabled() {"
END = "void EggMecaCtrlUnit_CheckMotionAttribute::OnAttribute(u16 attribute, int value) {"
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
I1 = "                        field_0x8 = 60.0f * ((distance - 250.0f) / 100.0f) + m_owner->m_settings.m_cameraDistance;\n"
I2 = "                    field_0xC = 12.0f * (1.0f - (distance - 225.0f) / 25.0f) + m_owner->m_settings.m_cameraHeight;\n"
REPL = {
  "prag": P,
  "prag_d1": P + [(I1, "                        float d = distance;\n" + I1.replace("(distance - 250.0f)", "(d - 250.0f)"))],
  "prag_t": P + [(I1, "                        float t = (distance - 250.0f) / 100.0f;\n" + I1.replace("((distance - 250.0f) / 100.0f)", "t")),
                 (I2, "                    float t = (distance - 225.0f) / 25.0f;\n" + I2.replace("(distance - 225.0f) / 25.0f", "t"))],
  "prag_k": P + [(I1, "                        float k = 60.0f;\n" + I1.replace("60.0f *", "k *")),
                 (I2, "                    float k = 12.0f;\n" + I2.replace("12.0f *", "k *"))],
}

G = "    Vec2f distance;\n    Vec2f start(from->x, from->z);\n    Vec2f end(to->x, to->z);\n    distance = fn_8040CBF8(&start, &end);\n"
FG = "bool EggMecaCtrlUnit_IronBall::GetThrowVelocity(const Vec3f *from, const Vec3f *to,"
EG = "EggMecaCtrlUnit_Laser::EggMecaCtrlUnit_Laser(EggMecaVarParam *param, int index)"
PG = [(FG, "#pragma opt_propagation off\n" + FG), (EG, "#pragma opt_propagation reset\n" + EG)]
GV = {
  "g_tmps": "    Vec2f distance;\n    distance = fn_8040CBF8(&(const Vec2f &)Vec2f(from->x, from->z), &(const Vec2f &)Vec2f(to->x, to->z));\n",
  "g_init": "    Vec2f start(from->x, from->z);\n    Vec2f end(to->x, to->z);\n    Vec2f distance = fn_8040CBF8(&start, &end);\n",
  "g_init_tmps": "    Vec2f distance = fn_8040CBF8(&(const Vec2f &)Vec2f(from->x, from->z), &(const Vec2f &)Vec2f(to->x, to->z));\n",
  "g_end_first": "    Vec2f distance;\n    Vec2f end(to->x, to->z);\n    Vec2f start(from->x, from->z);\n    distance = fn_8040CBF8(&start, &end);\n",
  "g_dist_last": "    Vec2f start(from->x, from->z);\n    Vec2f end(to->x, to->z);\n    Vec2f distance;\n    distance = fn_8040CBF8(&start, &end);\n",
}
for k, b in GV.items():
    REPL[k] = [(G, b)]
    REPL["p" + k] = PG + [(G, b)]
REPL["pg_base"] = PG
SCORE = ["IsEnabled__33EggMecaCtrlUnit_FixLookCharCameraFv", "GetThrowVelocity__24EggMecaCtrlUnit_IronBallFPC5Vec3fPC5Vec3fP5Vec3fff"]

FD = "inline Vec2f FlatDifference_(const Vec3f *from, const Vec3f *to) {\n    Vec2f start(from->x, from->z);\n    Vec2f end(to->x, to->z);\n    return fn_8040CBF8(&start, &end);\n}\n\n"
FD2 = "inline Vec2f FlatDifference_(const Vec3f *from, const Vec3f *to) {\n    Vec2f start;\n    Vec2f end;\n    start.x = from->x;\n    start.y = from->z;\n    end.x = to->x;\n    end.y = to->z;\n    return fn_8040CBF8(&start, &end);\n}\n\n"
FD3 = "inline Vec2f FlatDifference_(const Vec3f *from, const Vec3f *to) {\n    Vec2f start;\n    Vec2f end;\n    end.x = to->x;\n    end.y = to->z;\n    start.x = from->x;\n    start.y = from->z;\n    return fn_8040CBF8(&start, &end);\n}\n\n"
CMT = "/* The launch velocity that throws a body from from to to through height under gravity (name\n * inferred). */\n"
for name, fd in (("fd1", FD), ("fd2", FD2), ("fd3", FD3)):
    REPL["g_" + name] = [(CMT, fd + CMT), (G, "    Vec2f distance;\n    distance = FlatDifference_(from, to);\n")]
    REPL["g_" + name + "_init"] = [(CMT, fd + CMT), (G, "    Vec2f distance = FlatDifference_(from, to);\n")]
