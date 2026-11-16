"""Round 4 for folderPause's constructor (wave 39): InitScenes with an unused third argument (the
header declares `void InitScenes(Chao::CSD::CProject *project, s32 player)` for this round)."""
SYMBOLS = {
    "__ct__11folderPauseFPQ34Chao3CSD8CProject": "folderPause::folderPause(Chao::CSD::CProject *project) {",
}
TAIL = """    m_mission.Reset();
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
    "player_local": H + """    s32 player = manager->m_player;
    m_player = player;
    m_cursor = manager->m_cursor;
    InitScenes(project, player);
""" + TAIL,
    "both_locals": H + """    s32 player = manager->m_player;
    s32 cursor = manager->m_cursor;
    m_player = player;
    m_cursor = cursor;
    InitScenes(project, player);
""" + TAIL,
    "mgr_arg": H + """    m_player = manager->m_player;
    m_cursor = manager->m_cursor;
    InitScenes(project, manager->m_player);
""" + TAIL,
    "chain": H + """    s32 player;
    m_player = player = manager->m_player;
    m_cursor = manager->m_cursor;
    InitScenes(project, player);
""" + TAIL,
}}
