"""Source shapes for the two unmatched functions of Game/EnemyArea.cpp.

Both differ from retail by scheduling only: retail issues the point's `lfs`
loads above the prologue's spill stores (stw r0,36(r1) / stw r31,28(r1)), we
keep them below.  Everything else in both functions matches instruction for
instruction, including the frame size and the saved-register set.
"""

SYMBOLS = {
    "Contains__21UnkEnemyArea_8018A374FPv":
        "u8 UnkEnemyArea_8018A374::Contains(void *key) {",
    "fn_801A69A0":
        "extern \"C\" u8 fn_801A69A0(UnkEnemyArea_8018A374 *area, CharInfo *chara) {",
}

CAST = "    const Vec3f *point = static_cast<const Vec3f *>(key);"

BODIES = {
    "Contains__21UnkEnemyArea_8018A374FPv": {
        "committed": CAST + """
    if (std::fabs(point->y - m_bounds.field_0xC.y) < m_bounds.field_0x18.z) {
        Vec3f flat(point->x, 0.0f, point->z);
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
        "named_dy": CAST + """
    float dy = point->y - m_bounds.field_0xC.y;
    if (std::fabs(dy) < m_bounds.field_0x18.z) {
        Vec3f flat(point->x, 0.0f, point->z);
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
        "ref_point": """    const Vec3f &point = *static_cast<const Vec3f *>(key);
    if (std::fabs(point.y - m_bounds.field_0xC.y) < m_bounds.field_0x18.z) {
        Vec3f flat(point.x, 0.0f, point.z);
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
        "hoisted_limit": CAST + """
    const float limit = m_bounds.field_0x18.z;
    if (std::fabs(point->y - m_bounds.field_0xC.y) < limit) {
        Vec3f flat(point->x, 0.0f, point->z);
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
        "both_temps_first": CAST + """
    float dy = point->y - m_bounds.field_0xC.y;
    float limit = m_bounds.field_0x18.z;
    if (std::fabs(dy) < limit) {
        Vec3f flat(point->x, 0.0f, point->z);
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
        "flat_stmts": CAST + """
    if (std::fabs(point->y - m_bounds.field_0xC.y) < m_bounds.field_0x18.z) {
        Vec3f flat;
        flat.x = point->x;
        flat.y = 0.0f;
        flat.z = point->z;
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
        "flat_stmts_zxy": CAST + """
    if (std::fabs(point->y - m_bounds.field_0xC.y) < m_bounds.field_0x18.z) {
        Vec3f flat;
        flat.z = point->z;
        flat.x = point->x;
        flat.y = 0.0f;
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
        "flat_copy_then_y": CAST + """
    if (std::fabs(point->y - m_bounds.field_0xC.y) < m_bounds.field_0x18.z) {
        Vec3f flat = *point;
        flat.y = 0.0f;
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
        "flat_init_list": CAST + """
    if (std::fabs(point->y - m_bounds.field_0xC.y) < m_bounds.field_0x18.z) {
        Vec3f flat = { point->x, 0.0f, point->z };
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
        "returns_1_0": CAST + """
    if (std::fabs(point->y - m_bounds.field_0xC.y) < m_bounds.field_0x18.z) {
        Vec3f flat(point->x, 0.0f, point->z);
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return 1;
        }
    }
    return 0;
""",
        "float_star_key": """    const float *v = static_cast<const float *>(key);
    if (std::fabs(v[1] - m_bounds.field_0xC.y) < m_bounds.field_0x18.z) {
        Vec3f flat(v[0], 0.0f, v[2]);
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
        "this_qualified": CAST + """
    if (std::fabs(point->y - this->m_bounds.field_0xC.y) < this->m_bounds.field_0x18.z) {
        Vec3f flat(point->x, 0.0f, point->z);
        if (fn_8040D180(&flat, &this->m_bounds.field_0x0) < this->m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
        "early_flat": CAST + """
    Vec3f flat(point->x, 0.0f, point->z);
    if (std::fabs(point->y - m_bounds.field_0xC.y) < m_bounds.field_0x18.z) {
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
""",
    },
    "fn_801A69A0": {
        "committed": """    Vec3f position(chara->m_position.x, chara->m_position.y, chara->m_position.z);
    return fn_8040D180(&position, &area->m_shape.field_0xC) < MoveRadiusSquared(area);
""",
        "copy_init": """    Vec3f position = chara->m_position;
    return fn_8040D180(&position, &area->m_shape.field_0xC) < MoveRadiusSquared(area);
""",
        "assign_after_decl": """    Vec3f position;
    position = chara->m_position;
    return fn_8040D180(&position, &area->m_shape.field_0xC) < MoveRadiusSquared(area);
""",
        "stmt_members_yxz": """    Vec3f position;
    position.y = chara->m_position.y;
    position.x = chara->m_position.x;
    position.z = chara->m_position.z;
    return fn_8040D180(&position, &area->m_shape.field_0xC) < MoveRadiusSquared(area);
""",
        "stmt_members_zyx": """    Vec3f position;
    position.z = chara->m_position.z;
    position.y = chara->m_position.y;
    position.x = chara->m_position.x;
    return fn_8040D180(&position, &area->m_shape.field_0xC) < MoveRadiusSquared(area);
""",
        "stmt_members_xyz": """    Vec3f position;
    position.x = chara->m_position.x;
    position.y = chara->m_position.y;
    position.z = chara->m_position.z;
    return fn_8040D180(&position, &area->m_shape.field_0xC) < MoveRadiusSquared(area);
""",
        "ref_then_ctor": """    const Vec3f &src = chara->m_position;
    Vec3f position(src.x, src.y, src.z);
    return fn_8040D180(&position, &area->m_shape.field_0xC) < MoveRadiusSquared(area);
""",
        "hoisted_limit": """    Vec3f position(chara->m_position.x, chara->m_position.y, chara->m_position.z);
    float limit = MoveRadiusSquared(area);
    return fn_8040D180(&position, &area->m_shape.field_0xC) < limit;
""",
        "limit_first": """    float limit = MoveRadiusSquared(area);
    Vec3f position(chara->m_position.x, chara->m_position.y, chara->m_position.z);
    return fn_8040D180(&position, &area->m_shape.field_0xC) < limit;
""",
        "direct_field": """    Vec3f position(chara->m_position.x, chara->m_position.y, chara->m_position.z);
    return fn_8040D180(&position, &area->m_shape.field_0xC) < area->m_shape.field_0x18[4];
""",
        "copy_call_no_temp": """    return fn_8040D180(&chara->m_position, &area->m_shape.field_0xC)
           < MoveRadiusSquared(area);
""",
        "cast_temp": """    return fn_8040D180(&(const Vec3f &)chara->m_position, &area->m_shape.field_0xC)
           < MoveRadiusSquared(area);
""",
        "float_star_position": """    const float *c = reinterpret_cast<const float *>(&chara->m_position);
    Vec3f position(c[0], c[1], c[2]);
    return fn_8040D180(&position, &area->m_shape.field_0xC) < MoveRadiusSquared(area);
""",
    },
}
