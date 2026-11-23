"""Wave 40: Event::Tick (Game/EggLastMechAIState.cpp). Retail stores the camera's angle unrounded
(stfs f1, before the two zeroes) and rounds a second copy for the subtraction."""
SCORE = ["Tick__Q311EggLastMech7AIState5EventFPQ211EggLastMech11EggLastMech"]
L1 = "                    Vec3f rotation(0.0f, fn_80016260(player->m_position.x, player->m_position.z), 0.0f);\n"
L2 = "                    rotation.y = fn_8040E99C(rotation.y - DegToRad(45.0f));\n"
CALL = "fn_80016260(player->m_position.x, player->m_position.z)"
I = "                    "
def v(a, b): return [(L1 + L2, a + b)]
REPL = {
    "dbl_cast": v(I + "double angle = " + CALL + ";\n" + I + "Vec3f rotation(0.0f, angle, 0.0f);\n", I + "rotation.y = fn_8040E99C((float)angle - DegToRad(45.0f));\n"),
    "dbl_float2": v(I + "double angle = " + CALL + ";\n" + I + "Vec3f rotation(0.0f, angle, 0.0f);\n", I + "float y = angle;\n" + I + "rotation.y = fn_8040E99C(y - DegToRad(45.0f));\n"),
    "members_yxz": v(I + "Vec3f rotation;\n" + I + "rotation.y = " + CALL + ";\n" + I + "rotation.x = 0.0f;\n" + I + "rotation.z = 0.0f;\n", L2),
    "members_dbl": v(I + "double angle = " + CALL + ";\n" + I + "Vec3f rotation;\n" + I + "rotation.y = angle;\n" + I + "rotation.x = 0.0f;\n" + I + "rotation.z = 0.0f;\n", I + "rotation.y = fn_8040E99C((float)angle - DegToRad(45.0f));\n"),
    "ctor_dbl_sub": v(I + "double angle = " + CALL + ";\n" + I + "Vec3f rotation(0.0f, angle, 0.0f);\n", I + "rotation.y = fn_8040E99C((float)(angle) - DegToRad(45.0f));\n" ),
    "float_local_ctor": v(I + "float angle = " + CALL + ";\n" + I + "Vec3f rotation(0.0f, " + CALL.replace(CALL, "angle") + ", 0.0f);\n", I + "rotation.y = fn_8040E99C(angle - DegToRad(45.0f));\n"),
    "ret_float_decl": [("double fn_80016260(float x, float z);", "float fn_80016260(float x, float z);")],
    "ret_float_dblconv": [("double fn_80016260(float x, float z);", "float fn_80016260(float x, float z);")] + v(I + "double angle = " + CALL + ";\n" + I + "Vec3f rotation(0.0f, angle, 0.0f);\n", I + "rotation.y = fn_8040E99C((float)angle - DegToRad(45.0f));\n"),
    "members_y_first_ctor": v(I + "Vec3f rotation;\n" + I + "rotation.y = " + CALL + ";\n" + I + "rotation.z = rotation.x = 0.0f;\n", L2),
}
