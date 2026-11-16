"""Source shapes for fn_8029EB5C of Game/EggShadowAndroidAI.cpp (wave 39): retail keeps the rate in
f31 and the yaw in f30.  GC/2.7 hands the saved registers to locals in declaration order and to
parameters after them, so the rate is a local of its own declared first."""
SCORE = ["fn_8029EB5C"]
OLD = """    Vec3f direction;
    Vec3f offset;
    fn_800091EC(&direction,
                fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                         target, &self->m_position)));
    float yaw = fn_80016260(-direction.x, -direction.z);
    if (rate < 0.0f) {
        rate = self->field_0xB4;
    }
    self->m_rotation.y = TurnAngleToward(yaw, self->m_rotation.y, rate * fn_8019FFE0(self));
"""
def v(top, yawdecl="    float yaw = fn_80016260(-direction.x, -direction.z);\n"):
    return [(OLD, top + """    Vec3f direction;
    Vec3f offset;
    fn_800091EC(&direction,
                fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                         target, &self->m_position)));
""" + yawdecl + """    if (turn < 0.0f) {
        turn = self->field_0xB4;
    }
    self->m_rotation.y = TurnAngleToward(yaw, self->m_rotation.y, turn * fn_8019FFE0(self));
""")]
REPL = {
  "turn_local_first": v("    float turn = rate;\n"),
  "turn_decl_first": v("    float turn;\n    float yaw;\n", "    yaw = fn_80016260(-direction.x, -direction.z);\n    turn = rate;\n"),
  "turn_decl_first2": v("    float turn;\n", "    float yaw = fn_80016260(-direction.x, -direction.z);\n    turn = rate;\n"),
}
