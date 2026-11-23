"""Wave 41: fn_801A3CE4 under propagation off: the new[] count copy."""
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
TAIL_I = "    int i;\n"
def v(first):
    return [(H + B, P0 + H + first + """    const EnemyMotionEntry *entries = table->entries;
    for (int i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
    int i;
""")]
REPL = {
  "parts_local": v("""    int count = table->count;
    model->m_frameCount = count;
    UnkModel_801A3E9C::Part *parts = new UnkModel_801A3E9C::Part[count];
    model->m_parts.reset(parts);
"""),
  "u32_count": v("""    u32 count = table->count;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
"""),
  "new_first": v("""    int count = table->count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
    model->m_frameCount = count;
"""),
  "count_long": v("""    long count = table->count;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
"""),
  "count_sizet": v("""    int count = table->count;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[(u32)count]);
"""),
}
