"""Wave 40: the types of the two ranks (SaveResult's last register swap)."""
SCORE = ["SaveResult__Q211StageAction10GoalActionFv"]
S = "    /* 0x04 */ s32 m_rank; /* 0 is the best rank */"
T = "    /* 0x0BC */ s32 m_rank;"
RANK = """        if (stage->m_missions[mission].m_rank > lbl_8057D6A0.m_stageRecord.m_rank) {
            stage->m_missions[mission].m_rank = lbl_8057D6A0.m_stageRecord.m_rank;
        }
"""
E_ = "enum Rank_ { RANK_0_, RANK_BIG_ = 0x7FFFFFFF };\n"
TOP = "/* The save data fn_800BB74C returns"
REPL = {
  "save_long": [(S, "    long m_rank;")],
  "save_enum": [(TOP, E_ + TOP), (S, "    Rank_ m_rank;")],
  "save_uint_cast": [(S, "    u32 m_rank;"), (RANK, RANK.replace("stage->m_missions[mission].m_rank >", "static_cast<s32>(stage->m_missions[mission].m_rank) >"))],
  "rec_long": [],
  "both_long": [(S, "    long m_rank;")],
  "lt": [(RANK, RANK.replace("stage->m_missions[mission].m_rank > lbl_8057D6A0.m_stageRecord.m_rank", "lbl_8057D6A0.m_stageRecord.m_rank < stage->m_missions[mission].m_rank"))],
  "long_lt": [(S, "    long m_rank;"), (RANK, RANK.replace("stage->m_missions[mission].m_rank > lbl_8057D6A0.m_stageRecord.m_rank", "lbl_8057D6A0.m_stageRecord.m_rank < stage->m_missions[mission].m_rank"))],
  "rec_long_lt": [(RANK, RANK.replace("stage->m_missions[mission].m_rank > lbl_8057D6A0.m_stageRecord.m_rank", "lbl_8057D6A0.m_stageRecord.m_rank < stage->m_missions[mission].m_rank"))],
  "s8cmp": [(RANK, RANK.replace("stage->m_missions[mission].m_rank > lbl_8057D6A0.m_stageRecord.m_rank", "(stage->m_missions[mission].m_rank - lbl_8057D6A0.m_stageRecord.m_rank) > 0"))],
}
HDR = {k: {"Game/TGi.h": [(T, "    /* 0x0BC */ long m_rank;")]} for k in ("rec_long", "both_long", "rec_long_lt")}
