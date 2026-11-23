"""Wave 41: SaveResult's rank swap under '#pragma opt_propagation off' with named copies."""
SCORE = ["SaveResult__Q211StageAction10GoalActionFv"]
H = "void GoalAction::SaveResult() {\n"
P0 = "#pragma opt_propagation off\n"
RANK = """        if (stage->m_missions[mission].m_rank > lbl_8057D6A0.m_stageRecord.m_rank) {
            stage->m_missions[mission].m_rank = lbl_8057D6A0.m_stageRecord.m_rank;
        }
"""
M = "stage->m_missions[mission]"
R = "lbl_8057D6A0.m_stageRecord.m_rank"
def rk(new):
    return [(H, P0 + H), (RANK, new)]
REPL = {
  "prag": [(H, P0 + H)],
  "old": rk(f"        s32 old = {M}.m_rank;\n        if (old > {R}) {{\n            {M}.m_rank = {R};\n        }}\n"),
  "rank": rk(f"        s32 rank = {R};\n        if ({M}.m_rank > rank) {{\n            {M}.m_rank = rank;\n        }}\n"),
  "old_rank": rk(f"        s32 old = {M}.m_rank;\n        s32 rank = {R};\n        if (old > rank) {{\n            {M}.m_rank = rank;\n        }}\n"),
  "rank_old": rk(f"        s32 rank = {R};\n        s32 old = {M}.m_rank;\n        if (old > rank) {{\n            {M}.m_rank = rank;\n        }}\n"),
  "mptr": rk(f"        UnkMissionSave_800BB74C *m = &{M};\n        if (m->m_rank > {R}) {{\n            m->m_rank = {R};\n        }}\n"),
  "old_rank2": rk(f"        s32 old = {M}.m_rank;\n        s32 rank = {R};\n        if (old > rank) {{\n            {M}.m_rank = {R};\n        }}\n"),
}
