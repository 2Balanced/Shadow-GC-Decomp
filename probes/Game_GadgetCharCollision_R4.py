"""Ways of reaching the character for GadgetCharCollision::IsAllIgnored (wave 39, second pass)."""
OLD = "                    if (entry->first == hit->field_0x5C && entry->second == GetKind(hit.get())) {\n"
ANCH = "bool GadgetCharCollision::IsAllIgnored(UnkColliContacts_8001EE04 *contacts) {\n"
def v(s, pre=None):
    r = [(OLD, s)]
    if pre:
        r.append((ANCH, pre + "\n" + ANCH))
    return r
REPL = {
 "px": v("                    if (entry->first == hit.px->field_0x5C && entry->second == GetKind(hit.px)) {\n"),
 "star": v("                    if (entry->first == (*hit).field_0x5C && entry->second == GetKind(&*hit)) {\n"),
 "member_kind": v("                    if (entry->first == hit->field_0x5C && entry->second == hit->GetKind()) {\n"),
 "sp_ref": v("                    if (entry->first == IdOf(hit) && entry->second == KindOf(hit)) {\n",
   "inline s32 IdOf(const boost::shared_ptr<CharInfo> &p) {\n    return p->field_0x5C;\n}\ninline s32 KindOf(const boost::shared_ptr<CharInfo> &p) {\n    return p->GetKind();\n}\n"),
 "sp_val": v("                    if (entry->first == IdOf(hit.get()) && entry->second == KindOf(hit.get())) {\n",
   "inline s32 IdOf(const CharInfo *p) {\n    return p->field_0x5C;\n}\ninline s32 KindOf(const CharInfo *p) {\n    return p->GetKind();\n}\n"),
 "get_get": v("                    if (entry->first == hit.get()->field_0x5C && entry->second == hit.get()->GetKind()) {\n"),
 "const_hit": [("if (boost::shared_ptr<CharInfo> hit = owner.lock()) {", "if (const boost::shared_ptr<CharInfo> hit = owner.lock()) {")],
 "ref_local": v("                    const s32 &id = hit->field_0x5C;\n                    if (entry->first == id && entry->second == GetKind(hit.get())) {\n"),
 "cmp_fn": v("                    if (Same(entry, hit.get())) {\n",
   "inline bool Same(std::pair<s32, s32> *e, CharInfo *info) {\n    if (e->first == info->field_0x5C && e->second == GetKind(info)) {\n        return true;\n    }\n    return false;\n}\n"),
 "cmp_fn2": v("                    if (Same(entry->first, entry->second, hit.get())) {\n",
   "inline bool Same(s32 a, s32 b, CharInfo *info) {\n    return a == info->field_0x5C && b == GetKind(info);\n}\n"),
}
SCORE = ["IsAllIgnored__19GadgetCharCollisionFP25UnkColliContacts_8001EE04"]
