"""Wave 41: fn_801A3CE4: where inside the body the propagation pragma takes effect, and the new[]
count written without the copy that propagation-off keeps."""
SCORE = ["fn_801A3CE4"]
H = """extern "C" void fn_801A3CE4(UnkModel_801A3E9C *model, int slot, const EnemyMotionTable *table,
                            _stMtnPlayWork *motion) {
"""
B = """    int i;
    int count = table->count;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
    const EnemyMotionEntry *entries = table->entries;
    for (i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
"""
B2 = """    int count = table->count;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
    const EnemyMotionEntry *entries = table->entries;
    for (int i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
    int i;
"""
P0 = "#pragma opt_propagation off\n"
P1 = "#pragma opt_propagation reset\n"
REPL = {
  "inside_after_new": [(H + B, P0 + H + B2.replace("    const EnemyMotionEntry *entries", P1 + "    const EnemyMotionEntry *entries"))],
  "inside_start_off": [(H + B, H + P0 + B2)],
  "on_then_off_after_new": [(H + B, H + B2.replace("    const EnemyMotionEntry *entries", P0 + "    const EnemyMotionEntry *entries"))],
  "new_via_frameCount": [(H + B, P0 + H + B2.replace("new UnkModel_801A3E9C::Part[count]", "new UnkModel_801A3E9C::Part[model->m_frameCount]"))],
  "new_assign_in": [(H + B, P0 + H + """    int count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[model->m_frameCount = count = table->count]);
    const EnemyMotionEntry *entries = table->entries;
    for (int i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
    int i;
""")],
  "const_count": [(H + B, P0 + H + B2.replace("    int count = table->count;", "    const int count = table->count;"))],
}
