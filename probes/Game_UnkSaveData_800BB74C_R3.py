"""Wave 40: fn_801E5040's loops in inline helpers (retail numbers the volatile registers table r9,
routes r10, row r11, entry value r12; GC/2.7 gives the value r9)."""
SCORE = ["fn_801E5040"]
OLD = """    for (int i = 0; i < 0x146; i++) {
        const UnkRouteInfo_801E515C *route = &lbl_804D1690[i];
        for (int j = 0; j < 7; j++) {
            bool found = false;
            for (int k = 0; k <= history->m_count; k++) {
                if (lbl_8053D9B0[j][route->m_choices[j]] == history->m_route.m_entries[k]) {
                    if (history->m_route.m_entries[k] == 0x27) {
                        if (history->m_result == 3 && route->m_choices[j] == 4) {
                            found = true;
                            break;
                        }
                        if (history->m_result == 5 && route->m_choices[j] == 5) {
                            found = true;
                            break;
                        }
                    } else {
                        found = true;
                        break;
                    }
                }
            }
            if (!found) {
                goto next;
            }
        }
        return i;
    next:;
    }
    return -1;
"""
F = 'extern "C" int fn_801E5040(UnkSaveData_800BB74C *save) {'
HK = """inline bool Visited_(UnkPlayHistory_800BB74C *history, const UnkRouteInfo_801E515C *route, int j) {
    bool found = false;
    for (int k = 0; k <= history->m_count; k++) {
        if (lbl_8053D9B0[j][route->m_choices[j]] == history->m_route.m_entries[k]) {
            if (history->m_route.m_entries[k] == 0x27) {
                if (history->m_result == 3 && route->m_choices[j] == 4) {
                    found = true;
                    break;
                }
                if (history->m_result == 5 && route->m_choices[j] == 5) {
                    found = true;
                    break;
                }
            } else {
                found = true;
                break;
            }
        }
    }
    return found;
}

"""
NEWK = """    for (int i = 0; i < 0x146; i++) {
        const UnkRouteInfo_801E515C *route = &lbl_804D1690[i];
        for (int j = 0; j < 7; j++) {
            if (!Visited_(history, route, j)) {
                goto next;
            }
        }
        return i;
    next:;
    }
    return -1;
"""
REPL = {
  "k_helper": [(F, HK + F), (OLD, NEWK)],
}
