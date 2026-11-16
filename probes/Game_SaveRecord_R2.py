"""Round 2 for fn_8016F4E8 of Game/SaveRecord.cpp: the stage id through inline helpers."""
import importlib.util, os
_p = os.path.join(os.path.dirname(__file__), "Game_SaveRecord.py")
_s = importlib.util.spec_from_file_location("r1", _p); _m = importlib.util.module_from_spec(_s); _s.loader.exec_module(_m)
SYMBOLS = _m.SYMBOLS
HEAD, LOOP = _m.HEAD, _m.LOOP

XTRA = {"fn_8016F4E8": """inline int StageId_() { return lbl_8057D6A0.m_stageId; }
inline int StageIndex_() { return fn_80176C04(fn_800B74B4(), lbl_8057D6A0.m_stageId); }
inline int StageIndex2_() { int id = lbl_8057D6A0.m_stageId; return fn_80176C04(fn_800B74B4(), id); }
inline int StageIndex3_(int id) { return fn_80176C04(fn_800B74B4(), id); }
inline UnkStageInfo_800CBAA4 *StageInfo_(int index) { return &fn_800CBAA4()->m_stages[index]; }
inline UnkStageInfo_800CBAA4 *StageInfo2_(int id) { return &fn_800CBAA4()->m_stages[fn_80176C04(fn_800B74B4(), id)]; }
"""}

BODIES = {
    "fn_8016F4E8": {
        "inl_id": HEAD + """    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[fn_80176C04(fn_800B74B4(), StageId_())];
""" + LOOP,
        "inl_index": HEAD + """    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[StageIndex_()];
""" + LOOP,
        "inl_index2": HEAD + """    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[StageIndex2_()];
""" + LOOP,
        "inl_index3": HEAD + """    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[StageIndex3_(lbl_8057D6A0.m_stageId)];
""" + LOOP,
        "inl_index3b": HEAD + """    int id = lbl_8057D6A0.m_stageId;
    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[StageIndex3_(id)];
""" + LOOP,
        "inl_info": HEAD + """    int id = lbl_8057D6A0.m_stageId;
    UnkStageInfo_800CBAA4 *stage = StageInfo_(fn_80176C04(fn_800B74B4(), id));
""" + LOOP,
        "inl_info2": HEAD + """    UnkStageInfo_800CBAA4 *stage = StageInfo2_(lbl_8057D6A0.m_stageId);
""" + LOOP,
        "inl_info2b": HEAD + """    int id = lbl_8057D6A0.m_stageId;
    UnkStageInfo_800CBAA4 *stage = StageInfo2_(id);
""" + LOOP,
        "inl_info_idx3": HEAD + """    UnkStageInfo_800CBAA4 *stage = StageInfo_(StageIndex3_(lbl_8057D6A0.m_stageId));
""" + LOOP,
        "inl_info_idx3_id": HEAD + """    UnkStageInfo_800CBAA4 *stage = StageInfo_(StageIndex3_(StageId_()));
""" + LOOP,
    },
}
