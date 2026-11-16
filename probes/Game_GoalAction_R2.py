"""Round 2 for GoalAction::SaveResult (wave 39): the rank compare, with the table entry already
read through an inline lookup."""
SCORE = ["SaveResult__Q211StageAction10GoalActionFv"]
ANCHOR = "void GoalAction::SaveResult() {"
HELP = """inline const UnkStageInfo_804C4A18 *StageInfoAt_(UnkObject_800B74B4 *, int i) { return &lbl_804C4A18[i]; }
inline UnkStageRecord_8057D748 &Record_() { return lbl_8057D6A0.m_stageRecord; }
inline s32 ResultRank_() { return lbl_8057D6A0.m_stageRecord.m_rank; }
template <class T> inline const T &Min_(const T &a, const T &b) { return b < a ? b : a; }
template <class T> inline void SetMin_(T &a, const T &b) { if (a > b) { a = b; } }
template <class T> inline void SetMinV_(T &a, T b) { if (a > b) { a = b; } }
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
def rk(t): return [H, I, (RANK, t)]
REPL = {
  "rec_ref": rk("""        if (stage->m_missions[mission].m_rank > Record_().m_rank) {
            stage->m_missions[mission].m_rank = Record_().m_rank;
        }
"""),
  "setmin_ref": rk("        SetMin_(stage->m_missions[mission].m_rank, lbl_8057D6A0.m_stageRecord.m_rank);\n"),
  "setmin_val": rk("        SetMinV_(stage->m_missions[mission].m_rank, lbl_8057D6A0.m_stageRecord.m_rank);\n"),
  "min_assign": rk("        stage->m_missions[mission].m_rank = Min_(stage->m_missions[mission].m_rank, lbl_8057D6A0.m_stageRecord.m_rank);\n"),
  "ternary": rk("""        stage->m_missions[mission].m_rank = stage->m_missions[mission].m_rank > lbl_8057D6A0.m_stageRecord.m_rank ? lbl_8057D6A0.m_stageRecord.m_rank : stage->m_missions[mission].m_rank;
"""),
  "le_else": rk("""        if (stage->m_missions[mission].m_rank <= lbl_8057D6A0.m_stageRecord.m_rank) {
        } else {
            stage->m_missions[mission].m_rank = lbl_8057D6A0.m_stageRecord.m_rank;
        }
"""),
  "old_local": rk("""        s32 old = stage->m_missions[mission].m_rank;
        if (old > lbl_8057D6A0.m_stageRecord.m_rank) {
            stage->m_missions[mission].m_rank = lbl_8057D6A0.m_stageRecord.m_rank;
        }
"""),
  "both_local": rk("""        s32 old = stage->m_missions[mission].m_rank;
        s32 rank = lbl_8057D6A0.m_stageRecord.m_rank;
        if (old > rank) {
            stage->m_missions[mission].m_rank = rank;
        }
"""),
  "mission_ptr": rk("""        UnkMissionSave_800BB74C *m = &stage->m_missions[mission];
        if (m->m_rank > lbl_8057D6A0.m_stageRecord.m_rank) {
            m->m_rank = lbl_8057D6A0.m_stageRecord.m_rank;
        }
"""),
}
