"""Wave 41: fn_801A3CE4's body in an inline function (its parameters become caller locals)."""
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
        int id = entries[i].value;
        if (id >= 0) {
            RwFrame *frame = model->m_parts[0].FindFrame(id);
            if (frame != 0) {
                model->GetPart(i)->Attach(frame);
            }
        } else if (motion != 0) {
            model->m_parts[i].SetMotion(motion);
        }
    }
}
"""
INL = """inline void LoadParts_(UnkModel_801A3E9C *model, int slot, const EnemyMotionTable *table,
                            _stMtnPlayWork *motion) {
""" + B
CALL = H + "    LoadParts_(model, slot, table, motion);\n}\n"
B2 = B.replace("    int i;\n    int count = table->count;\n", "    int count = table->count;\n    int i;\n")
INL2 = INL.replace("    int i;\n    int count = table->count;\n", "    int count = table->count;\n    int i;\n")
REPL = {
  "inline_body": [(H + B, INL + CALL)],
  "inline_body_countfirst": [(H + B, INL2 + CALL)],
  "prag_inline_body": [(H + B, INL + "#pragma opt_propagation off\n" + CALL)],
  "inline_rev_args": [(H + B, INL.replace("_stMtnPlayWork *motion) {", "_stMtnPlayWork *motion) {") + CALL)],
}
