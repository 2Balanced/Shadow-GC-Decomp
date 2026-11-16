"""Shapes for the ignore test of GadgetCharCollision::IsAllIgnored (wave 39, second pass)."""
OLD = "                    if (entry->first == hit->field_0x5C && entry->second == GetKind(hit.get())) {\n"
ANCH = "bool GadgetCharCollision::IsAllIgnored(UnkColliContacts_8001EE04 *contacts) {\n"
def v(s, pre=None):
    r = [(OLD, s)]
    if pre:
        r.append((ANCH, pre + "\n" + ANCH))
    return r
REPL = {
 "inl_same_val": v("                    if (Same(*entry, hit->field_0x5C, GetKind(hit.get()))) {\n",
   "inline bool Same(const std::pair<s32, s32> &e, s32 a, s32 b) {\n    return e.first == a && e.second == b;\n}\n"),
 "inl_same_info": v("                    if (Same(*entry, hit.get())) {\n",
   "inline bool Same(const std::pair<s32, s32> &e, CharInfo *info) {\n    return e.first == info->field_0x5C && e.second == GetKind(info);\n}\n"),
 "inl_getid": v("                    if (entry->first == GetId(hit.get()) && entry->second == GetKind(hit.get())) {\n",
   "inline s32 GetId(CharInfo *info) {\n    return info->field_0x5C;\n}\n"),
 "inl_both_rev": v("                    if (GetId(hit.get()) == entry->first && GetKind(hit.get()) == entry->second) {\n",
   "inline s32 GetId(CharInfo *info) {\n    return info->field_0x5C;\n}\n"),
 "arrow": v("                    if (entry->first == hit->field_0x5C && entry->second == GetKind(&*hit)) {\n"),
 "kind_raw": v("                    if (entry->first == hit->field_0x5C && entry->second == *(s32 *)&hit->field_0x3C[0x50 - 0x3C]) {\n"),
 "rev2": v("                    if (entry->first == hit->field_0x5C && GetKind(hit.get()) == entry->second) {\n"),
 "rev1": v("                    if (hit->field_0x5C == entry->first && entry->second == GetKind(hit.get())) {\n"),
 "nested": v("                    if (entry->first == hit->field_0x5C)\n                    if (entry->second == GetKind(hit.get())) {\n"),
 "pair_eq": v("                    if (*entry == std::pair<s32, s32>(hit->field_0x5C, GetKind(hit.get()))) {\n"),
 "getkind_ref": v("                    if (entry->first == hit->field_0x5C && entry->second == GetKind2(*hit)) {\n",
   "inline s32 GetKind2(CharInfo &info) {\n    return *(s32 *)&info.field_0x3C[0x50 - 0x3C];\n}\n"),
 "inl_entry": v("                    if (First(entry) == hit->field_0x5C && entry->second == GetKind(hit.get())) {\n",
   "inline s32 First(std::pair<s32, s32> *e) {\n    return e->first;\n}\n"),
}
SCORE = ["IsAllIgnored__19GadgetCharCollisionFP25UnkColliContacts_8001EE04"]
