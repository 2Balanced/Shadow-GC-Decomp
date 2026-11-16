"""Source shapes for fn_80193790 of Game/FlyerCommonBase.cpp (wave 39): retail loads the enemy into
r4 and forms the third argument (+0x60) before the second (+0x48)."""
SYMBOLS = {"fn_80193790": "extern \"C\" void fn_80193790(FlyerCommonBase *flyer, FlyerParam *param) {"}
XTRA = {"fn_80193790": """inline Vec3f *HomePos_(Enemy *e) { return &e->field_0x48; }
inline Vec3f *HomeRot_(Enemy *e) { return &e->field_0x60; }
"""}
REST = """        flyer->field_0x2E0 = param->MoveSpeedRatio * flyer->m_enemy->m_para->vfPara_0x20();
        flyer->field_0x2E4 = 0.0f;
        fn_8040CE60(Velocity(flyer));
        fn_801A6588(&flyer->m_path, flyer->m_enemy->GetPosition(), flyer->m_enemy->GetRotation(),
                    0.0f);
        if (flyer->vf_0x23C()) {
            flyer->field_0x2E8 = param->AttackStart * flyer->m_path.field_0x58;
            flyer->field_0x2EC = param->AttackEnd * flyer->m_path.field_0x58;
            if (flyer->field_0x2E0 > 0.1f) {
                float range = param->AttackEnd - param->AttackStart;
                flyer->field_0x2F0 = range * flyer->m_path.field_0x58 / flyer->field_0x2E0;
            } else {
                flyer->field_0x2F0 = 1.0f;
            }
        }
    } else {
        flyer->field_0x2D8 = param->m_path;
        flyer->field_0x2DC = param->FloatWidth;
    }
    flyer->field_0x2F4 = 0;
"""
def v(call): return "    if (flyer->vf_0x238()) {\n" + call + REST
BODIES = {"fn_80193790": {
    "home_rot_inline": v("""        fn_801A6688(&flyer->m_path, &flyer->m_enemy->field_0x48, flyer->m_enemy->GetHomeRotation(),
                    param->m_path, param->PathMirror != 0, 0);
"""),
    "both_inline": v("""        fn_801A6688(&flyer->m_path, HomePos_(flyer->m_enemy), HomeRot_(flyer->m_enemy),
                    param->m_path, param->PathMirror != 0, 0);
"""),
    "pos_inline": v("""        fn_801A6688(&flyer->m_path, HomePos_(flyer->m_enemy), &flyer->m_enemy->field_0x60,
                    param->m_path, param->PathMirror != 0, 0);
"""),
    "rot_local": v("""        Vec3f *rotation = &flyer->m_enemy->field_0x60;
        fn_801A6688(&flyer->m_path, &flyer->m_enemy->field_0x48, rotation,
                    param->m_path, param->PathMirror != 0, 0);
"""),
    "both_local": v("""        Vec3f *rotation = &flyer->m_enemy->field_0x60;
        Vec3f *position = &flyer->m_enemy->field_0x48;
        fn_801A6688(&flyer->m_path, position, rotation,
                    param->m_path, param->PathMirror != 0, 0);
"""),
    "enemy_local": v("""        Enemy *enemy = flyer->m_enemy;
        fn_801A6688(&flyer->m_path, &enemy->field_0x48, &enemy->field_0x60,
                    param->m_path, param->PathMirror != 0, 0);
"""),
    "enemy_local_inl": v("""        Enemy *enemy = flyer->m_enemy;
        fn_801A6688(&flyer->m_path, HomePos_(enemy), HomeRot_(enemy),
                    param->m_path, param->PathMirror != 0, 0);
"""),
}}
