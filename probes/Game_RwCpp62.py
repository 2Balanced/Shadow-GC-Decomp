"""Wave 40 (r7): fn_8049D43C: retail compares with entries + (count-1)*36 and copies from
entries + count*36 - 36."""
NL = "\n"
SYM = "fn_8049D43C"
SYMBOLS = {SYM: "void fn_8049D43C(RwDlRequirementList *list, RwDlRequirement *entry) {"}
C = {
    "arr": "&list->entries[list->count - 1]",
    "ptr": "list->entries + list->count - 1",
    "ptr2": "list->entries + (list->count - 1)",
}
S = {
    "arr": "list->entries[list->count - 1]",
    "ptr": "*(list->entries + list->count - 1)",
    "end": "(list->entries + list->count)[-1]",
    "end2": "*(&list->entries[list->count] - 1)",
}
BODIES = {SYM: {}}
for cn, c in C.items():
    for sn, s in S.items():
        BODIES[SYM][cn + "_" + sn] = "    if (entry != " + c + ") {" + NL + "        *entry = " + s + ";" + NL + "    }" + NL + "    list->count--;" + NL
        BODIES[SYM][cn + "_" + sn + "_loc"] = "    RwDlRequirement *last = " + c + ";" + NL + "    if (entry != last) {" + NL + "        *entry = " + s + ";" + NL + "    }" + NL + "    list->count--;" + NL
