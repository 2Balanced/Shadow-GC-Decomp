"""Source shapes for EnemyParamCommon's slots (wave 39).  Two residuals: the slots that call out
load the virtual base pointer above the store of LR in retail, and the float slots past a record's
first value keep the enemy in r4 and the table in r3 in retail."""
SCORE = ["vfPara_0x38__16EnemyParamCommonFv", "vfPara_0x68__16EnemyParamCommonFv"]
A38 = "    return fn_801A04C0(m_enemy->m_type);\n"
A68 = "    return lbl_804CC424[m_enemy->m_type].m_gravity;\n"
F = "int EnemyParamCommon::vfPara_0x08() {"
HELP = """inline s32 TypeA_(Enemy *e) { return e->m_type; }
inline s32 TypeB_(const EnemyReferer *r) { return r->m_enemy->m_type; }
inline const EnemyMoveParam &MoveOf_(s32 type) { return lbl_804CC424[type]; }
inline const EnemyMoveParam *MoveOfP_(s32 type) { return &lbl_804CC424[type]; }
"""
H = (F, HELP + F)
REPL = {
  "const_self": [(A38, "    const EnemyParamCommon *self = this;\n    return fn_801A04C0(self->m_enemy->m_type);\n"), (A68, "    const EnemyParamCommon *self = this;\n    return lbl_804CC424[self->m_enemy->m_type].m_gravity;\n")],
  "const_ref": [(A38, "    const EnemyReferer *ref = this;\n    return fn_801A04C0(ref->m_enemy->m_type);\n"), (A68, "    const EnemyReferer *ref = this;\n    return lbl_804CC424[ref->m_enemy->m_type].m_gravity;\n")],
  "type_local": [(A38, "    s32 type = m_enemy->m_type;\n    return fn_801A04C0(type);\n"), (A68, "    s32 type = m_enemy->m_type;\n    return lbl_804CC424[type].m_gravity;\n")],
  "typeA": [H, (A38, "    return fn_801A04C0(TypeA_(m_enemy));\n"), (A68, "    return lbl_804CC424[TypeA_(m_enemy)].m_gravity;\n")],
  "typeB": [H, (A38, "    return fn_801A04C0(TypeB_(this));\n"), (A68, "    return lbl_804CC424[TypeB_(this)].m_gravity;\n")],
  "move_ref": [H, (A68, "    return MoveOf_(m_enemy->m_type).m_gravity;\n")],
  "move_ptr": [H, (A68, "    return MoveOfP_(m_enemy->m_type)->m_gravity;\n")],
  "move_local": [(A68, "    const EnemyMoveParam *move = &lbl_804CC424[m_enemy->m_type];\n    return move->m_gravity;\n")],
  "table_local": [(A68, "    const EnemyMoveParam *table = lbl_804CC424;\n    return table[m_enemy->m_type].m_gravity;\n")],
  "enemy_local": [(A68, "    Enemy *enemy = m_enemy;\n    return lbl_804CC424[enemy->m_type].m_gravity;\n")],
}
