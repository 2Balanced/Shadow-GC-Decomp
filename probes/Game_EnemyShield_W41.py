"""UnkEnemyShield_801A7E1C::Setup (wave 41): retail keeps the table parameter in r4 and loads the
slot into r0; GC/2.7 moves the table to r0 at the entry. Shapes with a named local copy plus
'#pragma opt_propagation off' scoped to the function (the wave 40 lever)."""
SCORE = ["Setup__23UnkEnemyShield_801A7E1CFRC16EnemyMotionTablei"]
OLD = """void UnkEnemyShield_801A7E1C::Setup(const EnemyMotionTable &table, int bone) {
    if (m_enemy != 0) {
        LoadModel(&m_model, m_enemy->m_motionBase, table);
        UnkModel_801A3E9C *body = reinterpret_cast<UnkModel_801A3E9C *>(m_enemy->m_disp->vfDisp_0x18());
        if (body != 0) {
            RwFrame *frame = fn_801A3704(body, bone);
            if (frame != 0) {
                fn_801A3814(&m_model, frame);
            }
        }
    }
}
"""
HEAD = "void UnkEnemyShield_801A7E1C::Setup(const EnemyMotionTable &table, int bone) {\n"
V = {}
V["self"] = """    UnkEnemyShield_801A7E1C *self = this;
    if (self->m_enemy != 0) {
        LoadModel(&self->m_model, self->m_enemy->m_motionBase, table);
        UnkModel_801A3E9C *body = reinterpret_cast<UnkModel_801A3E9C *>(self->m_enemy->m_disp->vfDisp_0x18());
        if (body != 0) {
            RwFrame *frame = fn_801A3704(body, bone);
            if (frame != 0) {
                fn_801A3814(&self->m_model, frame);
            }
        }
    }
}
"""
V["tptr"] = """    const EnemyMotionTable *t = &table;
    if (m_enemy != 0) {
        LoadModel(&m_model, m_enemy->m_motionBase, *t);
        UnkModel_801A3E9C *body = reinterpret_cast<UnkModel_801A3E9C *>(m_enemy->m_disp->vfDisp_0x18());
        if (body != 0) {
            RwFrame *frame = fn_801A3704(body, bone);
            if (frame != 0) {
                fn_801A3814(&m_model, frame);
            }
        }
    }
}
"""
V["tref"] = """    const EnemyMotionTable &t = table;
    if (m_enemy != 0) {
        LoadModel(&m_model, m_enemy->m_motionBase, t);
        UnkModel_801A3E9C *body = reinterpret_cast<UnkModel_801A3E9C *>(m_enemy->m_disp->vfDisp_0x18());
        if (body != 0) {
            RwFrame *frame = fn_801A3704(body, bone);
            if (frame != 0) {
                fn_801A3814(&m_model, frame);
            }
        }
    }
}
"""
V["bone"] = """    int b = bone;
    if (m_enemy != 0) {
        LoadModel(&m_model, m_enemy->m_motionBase, table);
        UnkModel_801A3E9C *body = reinterpret_cast<UnkModel_801A3E9C *>(m_enemy->m_disp->vfDisp_0x18());
        if (body != 0) {
            RwFrame *frame = fn_801A3704(body, b);
            if (frame != 0) {
                fn_801A3814(&m_model, frame);
            }
        }
    }
}
"""
V["enemy"] = """    Enemy *enemy = m_enemy;
    if (enemy != 0) {
        LoadModel(&m_model, enemy->m_motionBase, table);
        UnkModel_801A3E9C *body = reinterpret_cast<UnkModel_801A3E9C *>(enemy->m_disp->vfDisp_0x18());
        if (body != 0) {
            RwFrame *frame = fn_801A3704(body, bone);
            if (frame != 0) {
                fn_801A3814(&m_model, frame);
            }
        }
    }
}
"""
V["plain"] = OLD[len(HEAD):]
V["slot"] = """    if (m_enemy != 0) {
        int slot = m_enemy->m_motionBase;
        LoadModel(&m_model, slot, table);
        UnkModel_801A3E9C *body = reinterpret_cast<UnkModel_801A3E9C *>(m_enemy->m_disp->vfDisp_0x18());
        if (body != 0) {
            RwFrame *frame = fn_801A3704(body, bone);
            if (frame != 0) {
                fn_801A3814(&m_model, frame);
            }
        }
    }
}
"""
V["selftptr"] = """    UnkEnemyShield_801A7E1C *self = this;
    const EnemyMotionTable *t = &table;
    if (self->m_enemy != 0) {
        LoadModel(&self->m_model, self->m_enemy->m_motionBase, *t);
        UnkModel_801A3E9C *body = reinterpret_cast<UnkModel_801A3E9C *>(self->m_enemy->m_disp->vfDisp_0x18());
        if (body != 0) {
            RwFrame *frame = fn_801A3704(body, bone);
            if (frame != 0) {
                fn_801A3814(&self->m_model, frame);
            }
        }
    }
}
"""
V["direct_tptr"] = """    const EnemyMotionTable *t = &table;
    if (m_enemy != 0) {
        fn_801A3CE4(&m_model, m_enemy->m_motionBase, t, 0);
        UnkModel_801A3E9C *body = reinterpret_cast<UnkModel_801A3E9C *>(m_enemy->m_disp->vfDisp_0x18());
        if (body != 0) {
            RwFrame *frame = fn_801A3704(body, bone);
            if (frame != 0) {
                fn_801A3814(&m_model, frame);
            }
        }
    }
}
"""
REPL = {}
for k, body in V.items():
    REPL["prag_" + k] = [(OLD, "#pragma opt_propagation off\n" + HEAD + body + "#pragma opt_propagation reset\n")]
