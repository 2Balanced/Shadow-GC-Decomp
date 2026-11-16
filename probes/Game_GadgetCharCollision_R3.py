"""Inline comparison helpers for GadgetCharCollision::IsAllIgnored (wave 39, second pass)."""
OLD = "                    if (entry->first == hit->field_0x5C && entry->second == GetKind(hit.get())) {\n"
ANCH = "bool GadgetCharCollision::IsAllIgnored(UnkColliContacts_8001EE04 *contacts) {\n"
def v(s, pre=None):
    r = [(OLD, s)]
    if pre:
        r.append((ANCH, pre + "\n" + ANCH))
    return r
EQ = "inline bool Eq(s32 a, s32 b) {\n    return a == b;\n}\n"
EQR = "inline bool Eq(const s32 &a, const s32 &b) {\n    return a == b;\n}\n"
EQB = "inline bool Eq(s32 a, s32 b) {\n    return b == a;\n}\n"
REPL = {
 "eq_ec": v("                    if (Eq(entry->first, hit->field_0x5C) && Eq(entry->second, GetKind(hit.get()))) {\n", EQ),
 "eq_ce": v("                    if (Eq(hit->field_0x5C, entry->first) && Eq(GetKind(hit.get()), entry->second)) {\n", EQ),
 "eqr_ec": v("                    if (Eq(entry->first, hit->field_0x5C) && Eq(entry->second, GetKind(hit.get()))) {\n", EQR),
 "eqr_ce": v("                    if (Eq(hit->field_0x5C, entry->first) && Eq(GetKind(hit.get()), entry->second)) {\n", EQR),
 "eqb_ec": v("                    if (Eq(entry->first, hit->field_0x5C) && Eq(entry->second, GetKind(hit.get()))) {\n", EQB),
 "eqb_ce": v("                    if (Eq(hit->field_0x5C, entry->first) && Eq(GetKind(hit.get()), entry->second)) {\n", EQB),
 "notne": v("                    if (!(entry->first != hit->field_0x5C) && !(entry->second != GetKind(hit.get()))) {\n"),
 "xor": v("                    if ((entry->first ^ hit->field_0x5C) == 0 && (entry->second ^ GetKind(hit.get())) == 0) {\n"),
 "notsub": v("                    if (!(entry->first - hit->field_0x5C) && !(entry->second - GetKind(hit.get()))) {\n"),
 "int_long": v("                    if ((int)entry->first == hit->field_0x5C && (int)entry->second == GetKind(hit.get())) {\n"),
 "u_cast": v("                    if ((s32)(u32)entry->first == hit->field_0x5C && (s32)(u32)entry->second == GetKind(hit.get())) {\n"),
 "eq_mixed": v("                    if (Eq(entry->first, hit->field_0x5C) && entry->second == GetKind(hit.get())) {\n", EQ),
}
SCORE = ["IsAllIgnored__19GadgetCharCollisionFP25UnkColliContacts_8001EE04"]
