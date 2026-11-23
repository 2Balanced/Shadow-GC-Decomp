"""Wave 40: SaveResult's loops in inline helpers (a helper's loop renumbered the other registers of
EggLastMechSlot's fn_80313CC0)."""
SCORE = ["SaveResult__Q211StageAction10GoalActionFv"]
OLDL = """            for (int i = 0; i < 20; i++) {
                if (lbl_8057D6A0.field_0xD4[i] != 0) {
                    fn_801E5828(&history->m_flags, i);
                } else {
                    fn_801E5810(&history->m_flags, i);
                }
            }
"""
F = "void GoalAction::SaveResult() {"
H = """inline void SaveFlags_(UnkPlayHistory_800BB74C *history) {
    for (int i = 0; i < 20; i++) {
        if (lbl_8057D6A0.field_0xD4[i] != 0) {
            fn_801E5828(&history->m_flags, i);
        } else {
            fn_801E5810(&history->m_flags, i);
        }
    }
}

"""
B = """        if (!save->m_allBest) {
            bool allBest = true;
            int count = 0;
            for (int i = 5; i <= 43; i++) {
                if (i == 27) {
                    continue;
                }
                const UnkStageInfo_804C4A18 *info = StageInfoAt(fn_800B74B4(), i);
                if (info->m_flags & 1) {
                    if (save->m_stages[i].m_missions[1].m_rank == 0) {
                        count++;
                    } else {
                        allBest = false;
                    }
                } else {
                    for (int k = 0; k < 3; k++) {
                        if (info->m_missions[k].m_id != -1) {
                            if (save->m_stages[i].m_missions[k].m_rank == 0) {
                                count++;
                            } else {
                                allBest = false;
                            }
                        }
                    }
                }
            }
            save->m_bestCount = count;
            save->m_allBest = allBest;
        }
"""
HB = """inline void CountBest_(UnkSaveData_800BB74C *save) {
    bool allBest = true;
    int count = 0;
    for (int i = 5; i <= 43; i++) {
        if (i == 27) {
            continue;
        }
        const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[i];
        fn_800B74B4();
        if (info->m_flags & 1) {
            if (save->m_stages[i].m_missions[1].m_rank == 0) {
                count++;
            } else {
                allBest = false;
            }
        } else {
            for (int k = 0; k < 3; k++) {
                if (info->m_missions[k].m_id != -1) {
                    if (save->m_stages[i].m_missions[k].m_rank == 0) {
                        count++;
                    } else {
                        allBest = false;
                    }
                }
            }
        }
    }
    save->m_bestCount = count;
    save->m_allBest = allBest;
}

"""
REPL = {
  "flags_helper": [(F, H + F), (OLDL, "            SaveFlags_(history);\n")],
  "best_helper": [(F, HB + F), (B, "        if (!save->m_allBest) {\n            CountBest_(save);\n        }\n")],
}
