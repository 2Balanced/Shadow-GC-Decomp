"""Round 2 for EffectManager::Relocate's magic test (wave 39): the tag as the argument of an inline
function, so that it is formed before the data pointer is moved."""
SCORE = ["Relocate__Q36Effect6System13EffectManagerFPQ36Effect6System22UnkEffectData_80425860"]
OLD = '    if (fn_803AC8DC(data->m_magic, "EFFD", 4) != 0) {\n'
F = "bool EffectManager::Relocate(UnkEffectData_80425860 *data) {"
HELP = """inline int CmpMagicA_(UnkEffectData_80425860 *d, const char *tag) { return fn_803AC8DC(d->m_magic, tag, 4); }
inline int CmpMagicB_(const char *tag, UnkEffectData_80425860 *d) { return fn_803AC8DC(d->m_magic, tag, 4); }
inline int CmpMagicC_(UnkEffectData_80425860 *d, const char *tag, u32 n) { return fn_803AC8DC(d->m_magic, tag, n); }
inline int CmpMagicD_(const char *magic, const char *tag) { return fn_803AC8DC(magic, tag, 4); }
inline int CmpRev_(const char *tag, const char *magic, u32 n) { return fn_803AC8DC(magic, tag, n); }
"""
H = (F, HELP + F)
REPL = {
  "A": [H, (OLD, '    if (CmpMagicA_(data, "EFFD") != 0) {\n')],
  "B": [H, (OLD, '    if (CmpMagicB_("EFFD", data) != 0) {\n')],
  "C": [H, (OLD, '    if (CmpMagicC_(data, "EFFD", 4) != 0) {\n')],
  "D": [H, (OLD, '    if (CmpMagicD_(data->m_magic, "EFFD") != 0) {\n')],
  "Rev": [H, (OLD, '    if (CmpRev_("EFFD", data->m_magic, 4) != 0) {\n')],
}
