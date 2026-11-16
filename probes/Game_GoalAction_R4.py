"""Round 4 for GoalAction::SaveResult (wave 39): the rank compare through inline accessors."""
SCORE = ["SaveResult__Q211StageAction10GoalActionFv"]
ANCHOR = "void GoalAction::SaveResult() {"
HELP = """inline const UnkStageInfo_804C4A18 *StageInfoAt_(UnkObject_800B74B4 *, int i) { return &lbl_804C4A18[i]; }
inline bool Greater_(s32 a, s32 b) { return a > b; }
inline s32 SavedRank_(UnkMissionSave_800BB74C *m) { return m->m_rank; }
inline s32 &SavedRankRef_(UnkMissionSave_800BB74C *m) { return m->m_rank; }
inline void SetSavedRank_(UnkMissionSave_800BB74C *m, s32 rank) { m->m_rank = rank; }
inline s32 ResultRank_() { return lbl_8057D6A0.m_stageRecord.m_rank; }
inline bool IsBetterRank_(UnkMissionSave_800BB74C *m, s32 rank) { return m->m_rank > rank; }
"""
RANK = """        if (stage->m_missions[mission].m_rank > lbl_8057D6A0.m_stageRecord.m_rank) {
            stage->m_missions[mission].m_rank = lbl_8057D6A0.m_stageRecord.m_rank;
        }
"""
INFO = """                fn_800B74B4();
                const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[i];
"""
H = (ANCHOR, HELP + ANCHOR)
I = (INFO, "                const UnkStageInfo_804C4A18 *info = StageInfoAt_(fn_800B74B4(), i);\n")
def rk(t, extra=()): return [H, I, (RANK, t)] + list(extra)
M = "stage->m_missions[mission]"
R = "lbl_8057D6A0.m_stageRecord.m_rank"
REPL = {
  "greater": rk(f"        if (Greater_({M}.m_rank, {R})) {{\n            {M}.m_rank = {R};\n        }}\n"),
  "saved_get": rk(f"        if (SavedRank_(&{M}) > {R}) {{\n            {M}.m_rank = {R};\n        }}\n"),
  "saved_get_set": rk(f"        if (SavedRank_(&{M}) > {R}) {{\n            SetSavedRank_(&{M}, {R});\n        }}\n"),
  "saved_ref": rk(f"        if (SavedRankRef_(&{M}) > {R}) {{\n            SavedRankRef_(&{M}) = {R};\n        }}\n"),
  "both_get": rk(f"        if (SavedRank_(&{M}) > ResultRank_()) {{\n            SetSavedRank_(&{M}, ResultRank_());\n        }}\n"),
  "is_better": rk(f"        if (IsBetterRank_(&{M}, {R})) {{\n            {M}.m_rank = {R};\n        }}\n"),
  "is_better_get": rk(f"        if (IsBetterRank_(&{M}, ResultRank_())) {{\n            SetSavedRank_(&{M}, ResultRank_());\n        }}\n"),
  "set_only": rk(f"        if ({M}.m_rank > {R}) {{\n            SetSavedRank_(&{M}, {R});\n        }}\n"),
  "set_get": rk(f"        if ({M}.m_rank > {R}) {{\n            SetSavedRank_(&{M}, ResultRank_());\n        }}\n"),
}
