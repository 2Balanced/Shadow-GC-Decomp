"""Source shapes for folderPause's constructor (wave 39).

Retail loads the manager's player and cursor into r5 and r0 before storing either.
"""
SYMBOLS = {
    "__ct__11folderPauseFPQ34Chao3CSD8CProject": "folderPause::folderPause(Chao::CSD::CProject *project) {",
}
XTRA = {"__ct__11folderPauseFPQ34Chao3CSD8CProject": """struct UnkPausePair_ { s32 a; s32 b; };
struct UnkPauseTriple_ { s32 s; s32 a; s32 b; };
inline void CopyPair_(s32 *dst, const s32 *src) { dst[0] = src[0]; dst[1] = src[1]; }
inline void SetPair_(s32 *dst, s32 a, s32 b) { dst[0] = a; dst[1] = b; }
"""}
TAIL = """    InitScenes(project);
    m_mission.Reset();
    m_heroIcon.Reset();
    m_normalIcon.Reset();
    m_darkIcon.Reset();
    m_missionBoard.Reset();
    m_changeMission.Reset();
    m_pause.Reset();
    m_text.Reset();
    m_storyRouteText.Reset();
    m_state = 0;
"""
H = """    TPauseManager *manager = fn_800BADE8();
    m_managerState = manager->m_state;
"""
BODIES = {"__ct__11folderPauseFPQ34Chao3CSD8CProject": {
    "pair_cast": H + """    *(UnkPausePair_ *)&m_player = *(UnkPausePair_ *)&manager->m_player;
""" + TAIL,
    "pair_local": H + """    UnkPausePair_ pair = *(UnkPausePair_ *)&manager->m_player;
    *(UnkPausePair_ *)&m_player = pair;
""" + TAIL,
    "copy_inline": H + """    CopyPair_(&m_player, &manager->m_player);
""" + TAIL,
    "set_inline": H + """    SetPair_(&m_player, manager->m_player, manager->m_cursor);
""" + TAIL,
    "swap_decl": H + """    s32 cursor = manager->m_cursor;
    s32 player = manager->m_player;
    m_player = player;
    m_cursor = cursor;
""" + TAIL,
    "direct": H + """    m_player = manager->m_player;
    m_cursor = manager->m_cursor;
""" + TAIL,
    "triple_cast": """    TPauseManager *manager = fn_800BADE8();
    *(UnkPauseTriple_ *)&m_managerState = *(UnkPauseTriple_ *)&manager->m_state;
""" + TAIL,
}}
