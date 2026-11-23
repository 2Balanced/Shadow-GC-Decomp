"""Wave 41: EnemyParamCommon's float slots with copy propagation off and named copies (the wave-40
lever): retail keeps the enemy in r4 and builds the table's address in r3."""
SCORE = ["vfPara_0x24__16EnemyParamCommonFv"]
F = "float EnemyParamCommon::vfPara_0x24() {"
END = "float EnemyParamCommon::vfPara_0x28() {"
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
A = "    return lbl_804CC424[m_enemy->m_type].field_0x4;\n"
def v(body, prag=True):
    return (P if prag else []) + [(F + "\n" + A, F + "\n" + body)]
REPL = {
  "prag": v(A),
  "prag_enemy": v("    Enemy *enemy = m_enemy;\n    return lbl_804CC424[enemy->m_type].field_0x4;\n"),
  "prag_type": v("    s32 type = m_enemy->m_type;\n    return lbl_804CC424[type].field_0x4;\n"),
  "prag_move": v("    const EnemyMoveParam *move = lbl_804CC424;\n    move += m_enemy->m_type;\n    return move->field_0x4;\n"),
  "prag_move2": v("    const EnemyMoveParam *move = lbl_804CC424;\n    return move[m_enemy->m_type].field_0x4;\n"),
  "prag_enemy_move": v("    Enemy *enemy = m_enemy;\n    const EnemyMoveParam *move = lbl_804CC424;\n    return move[enemy->m_type].field_0x4;\n"),
  "prag_move_enemy": v("    const EnemyMoveParam *move = lbl_804CC424;\n    Enemy *enemy = m_enemy;\n    return move[enemy->m_type].field_0x4;\n"),
  "prag_self": v("    EnemyParamCommon *self = this;\n    return lbl_804CC424[self->m_enemy->m_type].field_0x4;\n"),
  "prag_moveptr": v("    const EnemyMoveParam *move = &lbl_804CC424[m_enemy->m_type];\n    return move->field_0x4;\n"),
  "prag_enemy_moveptr": v("    Enemy *enemy = m_enemy;\n    const EnemyMoveParam *move = &lbl_804CC424[enemy->m_type];\n    return move->field_0x4;\n"),
}
