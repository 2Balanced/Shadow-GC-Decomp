"""More shapes for the ignore test of GadgetCharCollision::IsAllIgnored (wave 39, second pass)."""
OLD = """                    if (entry->first == hit->field_0x5C && entry->second == GetKind(hit.get())) {
                        found = true;
                    }
"""
def v(s):
    return [(OLD, s)]
REPL = {
 "locals": v("""                    s32 id = hit->field_0x5C;
                    s32 kind = GetKind(hit.get());
                    if (entry->first == id && entry->second == kind) {
                        found = true;
                    }
"""),
 "info_local": v("""                    CharInfo *info = hit.get();
                    if (entry->first == info->field_0x5C && entry->second == GetKind(info)) {
                        found = true;
                    }
"""),
 "elocals": v("""                    s32 first = entry->first;
                    s32 second = entry->second;
                    if (first == hit->field_0x5C && second == GetKind(hit.get())) {
                        found = true;
                    }
"""),
 "efirst": v("""                    s32 first = entry->first;
                    if (first == hit->field_0x5C && entry->second == GetKind(hit.get())) {
                        found = true;
                    }
"""),
 "ne": v("""                    if (entry->first != hit->field_0x5C || entry->second != GetKind(hit.get())) {
                    } else {
                        found = true;
                    }
"""),
 "cont": v("""                    if (entry->first != hit->field_0x5C) {
                        continue;
                    }
                    if (entry->second != GetKind(hit.get())) {
                        continue;
                    }
                    found = true;
"""),
 "copy": v("""                    std::pair<s32, s32> e = *entry;
                    if (e.first == hit->field_0x5C && e.second == GetKind(hit.get())) {
                        found = true;
                    }
"""),
 "cref": v("""                    const std::pair<s32, s32> &e = *entry;
                    if (e.first == hit->field_0x5C && e.second == GetKind(hit.get())) {
                        found = true;
                    }
"""),
 "sub": v("""                    if (entry->first - hit->field_0x5C == 0 && entry->second - GetKind(hit.get()) == 0) {
                        found = true;
                    }
"""),
 "kind_first": v("""                    s32 kind = GetKind(hit.get());
                    if (entry->first == hit->field_0x5C && entry->second == kind) {
                        found = true;
                    }
"""),
 "citer": [("for (std::pair<s32, s32> *entry = m_ignore.begin();", "for (const std::pair<s32, s32> *entry = m_ignore.begin();")],
}
SCORE = ["IsAllIgnored__19GadgetCharCollisionFP25UnkColliContacts_8001EE04"]
