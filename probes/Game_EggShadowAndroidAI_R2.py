"""Wave 40: the two turn helpers of Game/EggShadowAndroidAI.cpp, the angles read through inline
functions (the lever of AdvReal::Option::Impl::UpdateList)."""
SCORE = ["fn_8029EB5C", "fn_8029E488"]
A = "extern \"C\" bool fn_8029EB5C(EnemyCharInfo *self, const Vec3f *target, float rate) {"
H = """inline float YawOf_(const Vec3f &d) { return fn_80016260(-d.x, -d.z); }
inline float YawOfP_(const Vec3f *d) { return fn_80016260(-d->x, -d->z); }
inline float RateOf_(EnemyCharInfo *self, float rate) { if (rate < 0.0f) { rate = self->field_0xB4; } return rate; }
inline float First_(const Vec2f &a) { return a.x; }
inline float Second_(const Vec2f &a) { return a.y; }
""" + A
Y = "    float yaw = fn_80016260(-direction.x, -direction.z);\n"
RT = "    if (rate < 0.0f) {\n        rate = self->field_0xB4;\n    }\n    self->m_rotation.y = TurnAngleToward(yaw, self->m_rotation.y, rate * fn_8019FFE0(self));\n"
P = "    float pitch = angles.x;\n    float yaw = angles.y;\n"
REPL = {
    "yaw_inl": [(A, H), (Y, "    float yaw = YawOf_(direction);\n")],
    "yaw_inlp": [(A, H), (Y, "    float yaw = YawOfP_(&direction);\n")],
    "rate_inl": [(A, H), (RT, "    rate = RateOf_(self, rate);\n    self->m_rotation.y = TurnAngleToward(yaw, self->m_rotation.y, rate * fn_8019FFE0(self));\n")],
    "rate_local": [(A, H), (RT, "    float turn = RateOf_(self, rate);\n    self->m_rotation.y = TurnAngleToward(yaw, self->m_rotation.y, turn * fn_8019FFE0(self));\n")],
    "both_inl": [(A, H), (Y, "    float yaw = YawOf_(direction);\n"), (RT, "    float turn = RateOf_(self, rate);\n    self->m_rotation.y = TurnAngleToward(yaw, self->m_rotation.y, turn * fn_8019FFE0(self));\n")],
    "angles_inl": [(A, H), (P, "    float pitch = First_(angles);\n    float yaw = Second_(angles);\n")],
    "angles_inl_rev": [(A, H), (P, "    float yaw = Second_(angles);\n    float pitch = First_(angles);\n")],
    "angles_rev": [(P, "    float yaw = angles.y;\n    float pitch = angles.x;\n")],
}
