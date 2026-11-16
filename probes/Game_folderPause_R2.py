"""Round 2 for folderPause's constructor (wave 39): inline setters and getters."""
SYMBOLS = {
    "__ct__11folderPauseFPQ34Chao3CSD8CProject": "folderPause::folderPause(Chao::CSD::CProject *project) {",
}
XTRA = {"__ct__11folderPauseFPQ34Chao3CSD8CProject": """inline void SetA_(s32 *dst, s32 player, s32 cursor) { dst[0] = player; dst[1] = cursor; }
inline void SetB_(s32 *dst, s32 cursor, s32 player) { dst[0] = player; dst[1] = cursor; }
inline void SetC_(s32 *dst, s32 player, s32 cursor) { dst[1] = cursor; dst[0] = player; }
inline s32 GetPlayer_(TPauseManager *m) { return m->m_player; }
inline s32 GetCursor_(TPauseManager *m) { return m->m_cursor; }
inline s32 GetState_(TPauseManager *m) { return m->m_state; }
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
    "setA": H + "    SetA_(&m_player, manager->m_player, manager->m_cursor);\n" + TAIL,
    "setB": H + "    SetB_(&m_player, manager->m_cursor, manager->m_player);\n" + TAIL,
    "setC": H + "    SetC_(&m_player, manager->m_player, manager->m_cursor);\n" + TAIL,
    "setA_get": H + "    SetA_(&m_player, GetPlayer_(manager), GetCursor_(manager));\n" + TAIL,
    "setB_get": H + "    SetB_(&m_player, GetCursor_(manager), GetPlayer_(manager));\n" + TAIL,
    "get_locals": H + """    s32 player = GetPlayer_(manager);
    s32 cursor = GetCursor_(manager);
    m_player = player;
    m_cursor = cursor;
""" + TAIL,
    "get_locals_rev": H + """    s32 cursor = GetCursor_(manager);
    s32 player = GetPlayer_(manager);
    m_player = player;
    m_cursor = cursor;
""" + TAIL,
    "get_direct": H + """    m_player = GetPlayer_(manager);
    m_cursor = GetCursor_(manager);
""" + TAIL,
    "one_local_cursor": H + """    s32 cursor = manager->m_cursor;
    m_player = manager->m_player;
    m_cursor = cursor;
""" + TAIL,
    "one_local_player": H + """    s32 player = manager->m_player;
    m_cursor = manager->m_cursor;
    m_player = player;
""" + TAIL,
    "stores_rev": H + """    s32 player = manager->m_player;
    s32 cursor = manager->m_cursor;
    m_cursor = cursor;
    m_player = player;
""" + TAIL,
    "all_three": """    TPauseManager *manager = fn_800BADE8();
    s32 state = manager->m_state;
    s32 player = manager->m_player;
    s32 cursor = manager->m_cursor;
    m_managerState = state;
    m_player = player;
    m_cursor = cursor;
""" + TAIL,
    "chain": H + """    s32 player, cursor;
    m_player = player = manager->m_player;
    m_cursor = cursor = manager->m_cursor;
""" + TAIL,
}}
