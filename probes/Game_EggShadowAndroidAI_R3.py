"""Wave 40 (resumed agent): fn_8029E488: the result of the second test kept in locals of each type, the rate copied into a local first, copy propagation off (the winner: an int).
Written as a JOBS list (unit, name, [(old, new)], [symbols]) against the source of the time;
the REPL/SCORE lines at the end give tools/pprobe.py the same variants."""
F = 'extern "C" bool fn_8029E488(EnemyCharInfo *self, const Vec3f *target, float rate) {\n'
B0 = """    Vec3f direction;
    Vec3f offset;
    fn_800091EC(&direction,
                fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                         target, &self->m_position)));
    Vec2f angles = fn_8040F760(&direction);
    float pitch = angles.x;
    float yaw = angles.y;
    if (rate < 0.0f) {
        rate = self->field_0xB4;
    }
    rate *= fn_8019FFE0(self);
    if (pitch > self->field_0xB8) {
        pitch = self->field_0xB8;
    } else if (pitch < -self->field_0xB8) {
        pitch = -self->field_0xB8;
    }
    self->m_rotation.y = TurnAngleToward(yaw, self->m_rotation.y, rate);
    self->m_rotation.x = TurnAngleToward(pitch, self->m_rotation.x, rate);
    self->m_rotation.z = TurnAngleToward(0.0f, self->m_rotation.z, rate);
    return fn_801C455C(yaw, self->m_rotation.y) && fn_801C455C(pitch, self->m_rotation.x);
"""
RET = {
 "and": "    return fn_801C455C(yaw, self->m_rotation.y) && fn_801C455C(pitch, self->m_rotation.x);\n",
 "if_ret": "    if (!fn_801C455C(yaw, self->m_rotation.y)) {\n        return false;\n    }\n    return fn_801C455C(pitch, self->m_rotation.x);\n",
 "if_ret_ne": "    if (!fn_801C455C(yaw, self->m_rotation.y)) {\n        return false;\n    }\n    return fn_801C455C(pitch, self->m_rotation.x) != 0;\n",
 "if_ret_int": "    if (!fn_801C455C(yaw, self->m_rotation.y)) {\n        return false;\n    }\n    return (int)fn_801C455C(pitch, self->m_rotation.x);\n",
 "if_ret_bool": "    if (fn_801C455C(yaw, self->m_rotation.y) == false) {\n        return false;\n    }\n    bool facing = fn_801C455C(pitch, self->m_rotation.x);\n    return facing;\n",
 "and_ne": "    return fn_801C455C(yaw, self->m_rotation.y) != 0 && fn_801C455C(pitch, self->m_rotation.x) != 0;\n",
}
def body(ret, turn):
    b = B0.replace(RET["and"], ret)
    if turn:
        b = "    float turn = rate;\n" + b.replace("    if (rate < 0.0f) {\n        rate = self->field_0xB4;\n    }\n    rate *= fn_8019FFE0(self);\n", "    if (turn < 0.0f) {\n        turn = self->field_0xB4;\n    }\n    turn *= fn_8019FFE0(self);\n").replace(", rate);\n", ", turn);\n")
    return b
NL = chr(10)
for t in ("u8", "s8", "char", "int", "u32", "s32", "bool"):
    RET["loc_" + t] = ("    if (!fn_801C455C(yaw, self->m_rotation.y)) {" + NL + "        return false;" + NL + "    }" + NL +
                       "    %s facing = fn_801C455C(pitch, self->m_rotation.x);" % t + NL + "    return facing;" + NL)
JOBS = []
for rk, r in RET.items():
    if not rk.startswith("loc_"):
        continue
    JOBS.append(("EggShadowAndroidAI", rk, [(B0, body(r, True)), (F, "#pragma opt_propagation off" + NL + F)], ["fn_8029E488"]))

REPL = {name: pairs for (_unit, name, pairs, _syms) in JOBS}
SCORE = ['fn_8029E488']
