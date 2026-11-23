"""Wave 41: fn_801A3CE4 under propagation off, count first, first loop's index in the for: the
second part's declarations (retail: n r25, frame r26)."""
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
    if (motion != 0) {
        model->m_parts[0].SetMotion(motion);
    }
    int n = table->count;
    for (i = 1; i < n; i++) {
"""
P0 = "#pragma opt_propagation off\n"
def v(body):
    return [(H + B, P0 + H + body)]
REPL = {
  "n_top": v("""    int n;
    int count = table->count;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
    const EnemyMotionEntry *entries = table->entries;
    for (int i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
    if (motion != 0) {
        model->m_parts[0].SetMotion(motion);
    }
    int i;
    n = table->count;
    for (i = 1; i < n; i++) {
"""),
  "count_reuse": v("""    int count = table->count;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
    const EnemyMotionEntry *entries = table->entries;
    for (int i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
    if (motion != 0) {
        model->m_parts[0].SetMotion(motion);
    }
    int i;
    count = table->count;
    for (i = 1; i < count; i++) {
"""),
  "second_for_decl": v("""    int count = table->count;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
    const EnemyMotionEntry *entries = table->entries;
    for (int i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
    if (motion != 0) {
        model->m_parts[0].SetMotion(motion);
    }
    int n = table->count;
    for (int i = 1; i < n; i++) {
"""),
  "both_for_i_n_first": v("""    int count = table->count;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
    const EnemyMotionEntry *entries = table->entries;
    for (int i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
    int n = table->count;
    if (motion != 0) {
        model->m_parts[0].SetMotion(motion);
    }
    for (int i = 1; i < n; i++) {
"""),
}
