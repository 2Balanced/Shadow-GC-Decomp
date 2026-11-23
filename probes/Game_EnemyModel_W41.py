"""Wave 41: fn_801A3CE4 under '#pragma opt_propagation off' (which gives the parameters retail's
r27..r30) with the locals in other declaration orders (retail: entries r31, count r24, i r23)."""
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
P0 = "#pragma opt_propagation off\n"
def v(body):
    return [(H + B, P0 + H + body)]
REPL = {
  "prag": v(B),
  "count_first": v(B.replace("    int i;\n    int count = table->count;\n", "    int count = table->count;\n    int i;\n")),
  "count_first_entries_decl": v("""    int count = table->count;
    const EnemyMotionEntry *entries;
    int i;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
    entries = table->entries;
    for (i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
"""),
  "entries_count_i": v("""    const EnemyMotionEntry *entries;
    int count = table->count;
    int i;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
    entries = table->entries;
    for (i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
"""),
  "count_first_fc": v("""    int count = table->count;
    int i;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[model->m_frameCount = count]);
    const EnemyMotionEntry *entries = table->entries;
    for (i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
"""),
  "count_first_loopi": v("""    int count = table->count;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
    const EnemyMotionEntry *entries = table->entries;
    for (int i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
    int i;
"""),
}
