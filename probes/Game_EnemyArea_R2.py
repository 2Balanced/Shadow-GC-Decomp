"""Wave 40: const-ness of the parameters and of this (the wave 39 lever: GC/2.7 moves a load above
the prologue's stores exactly when the object read is const in the parameter's declaration)."""
A = 'extern "C" u8 fn_801A69A0(UnkEnemyArea_8018A374 *area, CharInfo *chara) {'
C = "u8 UnkEnemyArea_8018A374::Contains(void *key) {"
H = "    u8 Contains(void *key);"
G = "inline float &MoveRadiusSquared(UnkEnemyArea_8018A374 *area) {"
RET = "Contains__21UnkEnemyArea_8018A374FPv"
SCORE = ["fn_801A69A0", (RET, RET), (RET, RET.replace("FPv","FPCv")), (RET, RET.replace("FPv","CFPv")), (RET, RET.replace("FPv","CFPCv")), (RET, RET.replace("FPv","FPC5Vec3f"))]
REPL = {
    "base": [(A, A)],
    "a_const_chara": [(A, A.replace("CharInfo *chara", "const CharInfo *chara"))],
    "a_const_both": [(A, 'extern "C" u8 fn_801A69A0(const UnkEnemyArea_8018A374 *area, const CharInfo *chara) {'),
                     (G, "inline const float &MoveRadiusSquared(const UnkEnemyArea_8018A374 *area) {\n    return *reinterpret_cast<const float *>(&area->m_shape.field_0x18[4]);\n}\n" + G)],
    "c_const_key": [(C, "u8 UnkEnemyArea_8018A374::Contains(const void *key) {")],
    "c_const_this": [(C, "u8 UnkEnemyArea_8018A374::Contains(void *key) const {")],
    "c_const_both": [(C, "u8 UnkEnemyArea_8018A374::Contains(const void *key) const {")],
    "c_vec": [(C, "u8 UnkEnemyArea_8018A374::Contains(const Vec3f *key) {")],
}
HDR = {
    "c_const_key": {"Game/Enemy.h": [(H, "    u8 Contains(const void *key);")]},
    "c_const_this": {"Game/Enemy.h": [(H, "    u8 Contains(void *key) const;")]},
    "c_const_both": {"Game/Enemy.h": [(H, "    u8 Contains(const void *key) const;")]},
    "c_vec": {"Game/Enemy.h": [(H, "    u8 Contains(const Vec3f *key);")]},
}
