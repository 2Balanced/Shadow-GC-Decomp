"""Source shapes for EffectManager::Relocate's magic test (wave 39): retail forms the "EFFD"
address through r3 and moves the data pointer into r3 last."""
SCORE = ["Relocate__Q36Effect6System13EffectManagerFPQ36Effect6System22UnkEffectData_80425860"]
OLD = '    if (fn_803AC8DC(data->m_magic, "EFFD", 4) != 0) {\n'
F = "bool EffectManager::Relocate(UnkEffectData_80425860 *data) {"
HELP = """inline int MagicCmp_(const char *a, const char *b, unsigned long n) { return fn_803AC8DC(a, b, n); }
inline const char *MagicOf_(UnkEffectData_80425860 *d) { return d->m_magic; }
inline bool BadMagic_(UnkEffectData_80425860 *d) { return fn_803AC8DC(d->m_magic, "EFFD", 4) != 0; }
"""
REPL = {
  "wrap": [(F, HELP + F), (OLD, '    if (MagicCmp_(data->m_magic, "EFFD", 4) != 0) {\n')],
  "magic_fn": [(F, HELP + F), (OLD, '    if (fn_803AC8DC(MagicOf_(data), "EFFD", 4) != 0) {\n')],
  "bad_magic": [(F, HELP + F), (OLD, '    if (BadMagic_(data)) {\n')],
  "cast": [(OLD, '    if (fn_803AC8DC((const char *)data, "EFFD", 4) != 0) {\n')],
  "local": [(OLD, '    const char *magic = data->m_magic;\n    if (fn_803AC8DC(magic, "EFFD", 4) != 0) {\n')],
  "str_local": [(OLD, '    const char *tag = "EFFD";\n    if (fn_803AC8DC(data->m_magic, tag, 4) != 0) {\n')],
  "comma": [(OLD, '    if (fn_803AC8DC(data->m_magic, (0, "EFFD"), 4) != 0) {\n')],
  "addr0": [(OLD, '    if (fn_803AC8DC(&data->m_magic[0], "EFFD", 4) != 0) {\n')],
}
