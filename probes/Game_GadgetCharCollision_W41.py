"""Wave 41: GadgetCharCollision::IsAllIgnored with copy propagation off (the wave 40 lever)."""
OLD = "                    if (entry->first == hit->field_0x5C && entry->second == GetKind(hit.get())) {\n"
ANCH = "bool GadgetCharCollision::IsAllIgnored(UnkColliContacts_8001EE04 *contacts) {\n"
NEXT = "/* Wraps collider in a new handle. */\n"
PRAG = [(ANCH, "#pragma opt_propagation off\n" + ANCH), (NEXT, "#pragma opt_propagation reset\n" + NEXT)]
def v(s=None, pre=None, extra=()):
    r = []
    if s:
        r.append((OLD, s))
    r += list(extra)
    if pre:
        r.append((ANCH, pre + "\n" + ANCH))
    return r + PRAG
GETID = "inline s32 GetId(CharInfo *info) {\n    return info->field_0x5C;\n}\n"
REPL = {
 "prag": v(),
 "prag_getid": v("                    if (entry->first == GetId(hit.get()) && entry->second == GetKind(hit.get())) {\n", GETID),
 "prag_info": v("                    CharInfo *info = hit.get();\n                    if (entry->first == info->field_0x5C && entry->second == GetKind(info)) {\n"),
 "prag_e": v("                    std::pair<s32, s32> *e = entry;\n                    if (e->first == hit->field_0x5C && e->second == GetKind(hit.get())) {\n"),
 "prag_vals": v("                    s32 first = entry->first;\n                    if (first == hit->field_0x5C && entry->second == GetKind(hit.get())) {\n"),
 "prag_same": v("                    if (Same(entry->first, hit->field_0x5C) && Same(entry->second, GetKind(hit.get()))) {\n",
   "inline bool Same(s32 a, s32 b) {\n    return a == b;\n}\n"),
 "prag_nested": v("                    if (entry->first == hit->field_0x5C)\n                    if (entry->second == GetKind(hit.get())) {\n"),
 "prag_rev": v("                    if (hit->field_0x5C == entry->first && GetKind(hit.get()) == entry->second) {\n"),
}
SCORE = ["IsAllIgnored__19GadgetCharCollisionFP25UnkColliContacts_8001EE04"]
