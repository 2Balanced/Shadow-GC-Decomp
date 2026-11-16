"""Round 3 for GoalAction::SaveResult (wave 39): more shapes of the rank compare."""
SCORE = ["SaveResult__Q211StageAction10GoalActionFv"]
ANCHOR = "void GoalAction::SaveResult() {"
HELP = """inline const UnkStageInfo_804C4A18 *StageInfoAt_(UnkObject_800B74B4 *, int i) { return &lbl_804C4A18[i]; }
"""
RANK = """        if (stage->m_missions[mission].m_rank > lbl_8057D6A0.m_stageRecord.m_rank) {
            stage->m_missions[mission].m_rank = lbl_8057D6A0.m_stageRecord.m_rank;
        }
"""
PLAYS = "        stage->m_plays++;\n"
INFO = """                fn_800B74B4();
                const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[i];
"""
H = (ANCHOR, HELP + ANCHOR)
I = (INFO, "                const UnkStageInfo_804C4A18 *info = StageInfoAt_(fn_800B74B4(), i);\n")
def rk(t, extra=()): return [H, I, (RANK, t)] + list(extra)
REPL = {
  "assign_in_cond": rk("""        s32 rank;
        if (stage->m_missions[mission].m_rank > (rank = lbl_8057D6A0.m_stageRecord.m_rank)) {
            stage->m_missions[mission].m_rank = rank;
        }
"""),
  "plays_pre": rk(RANK, [(PLAYS, "        ++stage->m_plays;\n")]),
  "plays_sum": rk(RANK, [(PLAYS, "        stage->m_plays = stage->m_plays + 1;\n")]),
  "plays_pluseq": rk(RANK, [(PLAYS, "        stage->m_plays += 1;\n")]),
  "rank_int_cast": rk("""        if (stage->m_missions[mission].m_rank > (int)lbl_8057D6A0.m_stageRecord.m_rank) {
            stage->m_missions[mission].m_rank = (int)lbl_8057D6A0.m_stageRecord.m_rank;
        }
"""),
  "not_le": rk("""        if (!(stage->m_missions[mission].m_rank <= lbl_8057D6A0.m_stageRecord.m_rank)) {
            stage->m_missions[mission].m_rank = lbl_8057D6A0.m_stageRecord.m_rank;
        }
"""),
  "rec_ptr": rk("""        if (stage->m_missions[mission].m_rank > rec->m_rank) {
            stage->m_missions[mission].m_rank = rec->m_rank;
        }
""", [("        UnkStageSave_800BB74C *stage = &save->m_stages[lbl_8057D6A0.m_stageRecord.m_index];", "        UnkStageRecord_8057D748 *rec = &lbl_8057D6A0.m_stageRecord;\n        UnkStageSave_800BB74C *stage = &save->m_stages[rec->m_index];")]),
  "block_local": rk("""        {
            s32 rank = lbl_8057D6A0.m_stageRecord.m_rank;
            if (stage->m_missions[mission].m_rank > rank) {
                stage->m_missions[mission].m_rank = rank;
            }
        }
"""),
  "const_local": rk("""        const s32 rank = lbl_8057D6A0.m_stageRecord.m_rank;
        if (stage->m_missions[mission].m_rank > rank) {
            stage->m_missions[mission].m_rank = rank;
        }
"""),
  "rank_before_plays": rk("", [(PLAYS, PLAYS.replace("stage->m_plays++;", "stage->m_plays++;") )]),
}
del REPL["rank_before_plays"]
