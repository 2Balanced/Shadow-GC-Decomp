"""Wave 41: fn_801E5040 with copy propagation off (retail numbers the volatile registers table r9,
routes r10, row r11, entry value r12; GC/2.7 gives the value r9)."""
SCORE = ["fn_801E5040"]
F = "extern \"C\" int fn_801E5040(UnkSaveData_800BB74C *save) {\n"
N = "/* The number of sound test entries. */\n"
PRAG = [(F, "#pragma opt_propagation off\n" + F), (N, "#pragma opt_propagation reset\n" + N)]
CMP = "                if (lbl_8053D9B0[j][route->m_choices[j]] == history->m_route.m_entries[k]) {\n                    if (history->m_route.m_entries[k] == 0x27) {\n"
HIST = "    UnkPlayHistory_800BB74C *history = &save->m_history;\n"


def v(*extra, prag=True):
    return list(extra) + (PRAG if prag else [])


REPL = {
    "prag": v(),
    "prag_entry": v((CMP, "                s32 entry = history->m_route.m_entries[k];\n                if (lbl_8053D9B0[j][route->m_choices[j]] == entry) {\n                    if (entry == 0x27) {\n")),
    "prag_save": v((HIST, "    UnkSaveData_800BB74C *self = save;\n    UnkPlayHistory_800BB74C *history = &self->m_history;\n")),
    "prag_tables": v((HIST, HIST + "    const UnkRouteInfo_801E515C *routes = lbl_804D1690;\n"),
                     ("        const UnkRouteInfo_801E515C *route = &lbl_804D1690[i];\n", "        const UnkRouteInfo_801E515C *route = &routes[i];\n")),
    "prag_row": v((CMP, "                if (fn_801E5140(j, route->m_choices[j]) == history->m_route.m_entries[k]) {\n                    if (history->m_route.m_entries[k] == 0x27) {\n")),
    "prag_rowinl": v((F, "inline s32 StageOf(int stage, int choice) {\n    return lbl_8053D9B0[stage][choice];\n}\n\n" + F),
                     (CMP, "                if (StageOf(j, route->m_choices[j]) == history->m_route.m_entries[k]) {\n                    if (history->m_route.m_entries[k] == 0x27) {\n")),
    "prag_rowinl_rev": v((F, "inline s32 StageOf(int stage, int choice) {\n    return lbl_8053D9B0[stage][choice];\n}\n\n" + F),
                         (CMP, "                if (history->m_route.m_entries[k] == StageOf(j, route->m_choices[j])) {\n                    if (history->m_route.m_entries[k] == 0x27) {\n")),
    "prag_entryinl": v((F, "inline s32 EntryOf(UnkPlayHistory_800BB74C *h, int k) {\n    return h->m_route.m_entries[k];\n}\n\n" + F),
                       (CMP, "                if (lbl_8053D9B0[j][route->m_choices[j]] == EntryOf(history, k)) {\n                    if (EntryOf(history, k) == 0x27) {\n")),
}
EI = "inline s32 EntryOf(UnkPlayHistory_800BB74C *h, int k) {\n    return h->m_route.m_entries[k];\n}\n\n"
REPL.update({
    "prag_entryinl_rev": v((F, EI + F),
                           (CMP, "                if (EntryOf(history, k) == lbl_8053D9B0[j][route->m_choices[j]]) {\n                    if (EntryOf(history, k) == 0x27) {\n")),
    "prag_entryinl_rev_rowinl": v((F, EI + "inline s32 StageOf(int stage, int choice) {\n    return lbl_8053D9B0[stage][choice];\n}\n\n" + F),
                           (CMP, "                if (EntryOf(history, k) == StageOf(j, route->m_choices[j])) {\n                    if (EntryOf(history, k) == 0x27) {\n")),
})
EIC = "inline s32 EntryOf(const UnkPlayHistory_800BB74C *h, int k) {\n    return h->m_route.m_entries[k];\n}\n\n"
MEM = ("    /* 0x00 */ s32 m_entries[24];\n};", "    s32 Get(int k) const {\n        return m_entries[k];\n    }\n\n    /* 0x00 */ s32 m_entries[24];\n};")
MEM2 = ("    /* 0x00 */ s32 m_entries[24];\n};", "    s32 Get(int k) {\n        return m_entries[k];\n    }\n\n    /* 0x00 */ s32 m_entries[24];\n};")
REPL.update({
    "c_entryinl": v((F, EIC + F),
                    (CMP, "                if (lbl_8053D9B0[j][route->m_choices[j]] == EntryOf(history, k)) {\n                    if (EntryOf(history, k) == 0x27) {\n")),
    "mem_get": v(MEM, (CMP, "                if (lbl_8053D9B0[j][route->m_choices[j]] == history->m_route.Get(k)) {\n                    if (history->m_route.Get(k) == 0x27) {\n")),
    "mem_get_rev": v(MEM, (CMP, "                if (history->m_route.Get(k) == lbl_8053D9B0[j][route->m_choices[j]]) {\n                    if (history->m_route.Get(k) == 0x27) {\n")),
    "mem2_get": v(MEM2, (CMP, "                if (lbl_8053D9B0[j][route->m_choices[j]] == history->m_route.Get(k)) {\n                    if (history->m_route.Get(k) == 0x27) {\n")),
    "ne_cont": v((F, EI + F),
                 (CMP, "                if (EntryOf(history, k) != lbl_8053D9B0[j][route->m_choices[j]]) {\n                    continue;\n                }\n                {\n                    if (EntryOf(history, k) == 0x27) {\n")),
    "noprag_entryinl": v((F, EI + F),
                       (CMP, "                if (lbl_8053D9B0[j][route->m_choices[j]] == EntryOf(history, k)) {\n                    if (EntryOf(history, k) == 0x27) {\n"), prag=False),
    "mem_get_noprag": v(MEM, (CMP, "                if (lbl_8053D9B0[j][route->m_choices[j]] == history->m_route.Get(k)) {\n                    if (history->m_route.Get(k) == 0x27) {\n"), prag=False),
})
REPL.update({
    "stage_local": v((F, EI + F),
                     (CMP, "                s32 stage = lbl_8053D9B0[j][route->m_choices[j]];\n                if (EntryOf(history, k) == stage) {\n                    if (EntryOf(history, k) == 0x27) {\n")),
    "stage_local_np": v((F, EI + F),
                     (CMP, "                s32 stage = lbl_8053D9B0[j][route->m_choices[j]];\n                if (EntryOf(history, k) == stage) {\n                    if (EntryOf(history, k) == 0x27) {\n"), prag=False),
    "stage_local2": v((F, EI + F),
                     (CMP, "                s32 stage = lbl_8053D9B0[j][route->m_choices[j]];\n                if (stage == EntryOf(history, k)) {\n                    if (EntryOf(history, k) == 0x27) {\n")),
    "entry_local_inl": v((F, EI + F),
                     (CMP, "                s32 entry = EntryOf(history, k);\n                if (entry == lbl_8053D9B0[j][route->m_choices[j]]) {\n                    if (entry == 0x27) {\n")),
    "entry_local_inl_np": v((F, EI + F),
                     (CMP, "                s32 entry = EntryOf(history, k);\n                if (entry == lbl_8053D9B0[j][route->m_choices[j]]) {\n                    if (entry == 0x27) {\n"), prag=False),
    "cast_both": v((F, EI + F),
                 (CMP, "                if ((s32)EntryOf(history, k) == (s32)lbl_8053D9B0[j][route->m_choices[j]]) {\n                    if (EntryOf(history, k) == 0x27) {\n")),
    "sub": v((F, EI + F),
                 (CMP, "                if (!(EntryOf(history, k) - lbl_8053D9B0[j][route->m_choices[j]])) {\n                    if (EntryOf(history, k) == 0x27) {\n")),
})
REPL.update({
    "st_direct": v((CMP, "                s32 stage = lbl_8053D9B0[j][route->m_choices[j]];\n                if (history->m_route.m_entries[k] == stage) {\n                    if (history->m_route.m_entries[k] == 0x27) {\n")),
    "st_direct2": v((CMP, "                s32 stage = lbl_8053D9B0[j][route->m_choices[j]];\n                if (stage == history->m_route.m_entries[k]) {\n                    if (history->m_route.m_entries[k] == 0x27) {\n")),
    "st_inl_direct": v((F, "inline s32 StageOf(int stage, int choice) {\n    return lbl_8053D9B0[stage][choice];\n}\n\n" + F),
                       (CMP, "                if (history->m_route.m_entries[k] == StageOf(j, route->m_choices[j])) {\n                    if (EntryOf2(history, k) == 0x27) {\n"),
                       (F, "inline s32 EntryOf2(UnkPlayHistory_800BB74C *h, int k) {\n    return h->m_route.m_entries[k];\n}\n\n" + F)),
    "both_inl_e2": v((F, EI + "inline s32 StageOf(int stage, int choice) {\n    return lbl_8053D9B0[stage][choice];\n}\n\n" + F),
                     (CMP, "                if (EntryOf(history, k) == StageOf(j, route->m_choices[j])) {\n                    if (history->m_route.m_entries[k] == 0x27) {\n")),
    "both_inl_np": v((F, EI + "inline s32 StageOf(int stage, int choice) {\n    return lbl_8053D9B0[stage][choice];\n}\n\n" + F),
                     (CMP, "                if (EntryOf(history, k) == StageOf(j, route->m_choices[j])) {\n                    if (EntryOf(history, k) == 0x27) {\n"), prag=False),
    "both_inl_row": v((F, EI + "inline const s32 *RowOf(int stage) {\n    return lbl_8053D9B0[stage];\n}\n\n" + F),
                     (CMP, "                if (EntryOf(history, k) == RowOf(j)[route->m_choices[j]]) {\n                    if (EntryOf(history, k) == 0x27) {\n")),
})
