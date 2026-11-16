"""Source shapes for StageAction::GoalAction::SaveResult (wave 39).  Two register swaps: the rank
compare (retail: saved rank r3, result rank r0) and the all-best loop (retail: table entry r5,
save stage r6)."""
SCORE = ["SaveResult__Q211StageAction10GoalActionFv"]
ANCHOR = "void GoalAction::SaveResult() {"
HELP = """inline const UnkStageInfo_804C4A18 *StageInfoAt_(UnkObject_800B74B4 *, int i) { return &lbl_804C4A18[i]; }
inline s32 ResultRank_() { return lbl_8057D6A0.m_stageRecord.m_rank; }
inline void BestRank_(UnkMissionSave_800BB74C *m, s32 rank) { if (m->m_rank > rank) { m->m_rank = rank; } }
"""
RANK = """        if (stage->m_missions[mission].m_rank > lbl_8057D6A0.m_stageRecord.m_rank) {
            stage->m_missions[mission].m_rank = lbl_8057D6A0.m_stageRecord.m_rank;
        }
"""
INFO = """                fn_800B74B4();
                const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[i];
"""
H = (ANCHOR, HELP + ANCHOR)
REPL = {
  "info_inline": [H, (INFO, "                const UnkStageInfo_804C4A18 *info = StageInfoAt_(fn_800B74B4(), i);\n")],
  "rank_inline_get": [H, (RANK, """        if (stage->m_missions[mission].m_rank > ResultRank_()) {
            stage->m_missions[mission].m_rank = ResultRank_();
        }
""")],
  "rank_local": [H, (RANK, """        s32 rank = lbl_8057D6A0.m_stageRecord.m_rank;
        if (stage->m_missions[mission].m_rank > rank) {
            stage->m_missions[mission].m_rank = rank;
        }
""")],
  "rank_local_get": [H, (RANK, """        s32 rank = ResultRank_();
        if (stage->m_missions[mission].m_rank > rank) {
            stage->m_missions[mission].m_rank = rank;
        }
""")],
  "rank_best": [H, (RANK, "        BestRank_(&stage->m_missions[mission], lbl_8057D6A0.m_stageRecord.m_rank);\n")],
  "rank_best_get": [H, (RANK, "        BestRank_(&stage->m_missions[mission], ResultRank_());\n")],
  "rank_rev": [H, (RANK, """        if (lbl_8057D6A0.m_stageRecord.m_rank < stage->m_missions[mission].m_rank) {
            stage->m_missions[mission].m_rank = lbl_8057D6A0.m_stageRecord.m_rank;
        }
""")],
}
