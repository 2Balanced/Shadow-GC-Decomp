"""Round 3 for folderPause's constructor (wave 39): the player and cursor as one struct."""
SYMBOLS = {
    "__ct__11folderPauseFPQ34Chao3CSD8CProject": "folderPause::folderPause(Chao::CSD::CProject *project) {",
}
XTRA = {"__ct__11folderPauseFPQ34Chao3CSD8CProject": """struct UnkPausePair_ { s32 a; s32 b; };
struct UnkPauseMgrView_ { u8 pad[0x34]; UnkPausePair_ pair; UnkPausePair_ Get() const { return pair; } const UnkPausePair_ &Ref() const { return pair; } };
struct UnkPauseSelfView_ { u8 pad[0xC4]; UnkPausePair_ pair; void Set(UnkPausePair_ p) { pair = p; } void SetRef(const UnkPausePair_ &p) { pair = p; } };
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
    "view_assign": H + "    ((UnkPauseSelfView_ *)this)->pair = ((UnkPauseMgrView_ *)manager)->pair;\n" + TAIL,
    "view_get": H + "    ((UnkPauseSelfView_ *)this)->pair = ((UnkPauseMgrView_ *)manager)->Get();\n" + TAIL,
    "view_set_get": H + "    ((UnkPauseSelfView_ *)this)->Set(((UnkPauseMgrView_ *)manager)->Get());\n" + TAIL,
    "view_set": H + "    ((UnkPauseSelfView_ *)this)->Set(((UnkPauseMgrView_ *)manager)->pair);\n" + TAIL,
    "view_setref": H + "    ((UnkPauseSelfView_ *)this)->SetRef(((UnkPauseMgrView_ *)manager)->pair);\n" + TAIL,
    "view_setref_get": H + "    ((UnkPauseSelfView_ *)this)->SetRef(((UnkPauseMgrView_ *)manager)->Get());\n" + TAIL,
    "local_members": H + """    UnkPausePair_ p = ((UnkPauseMgrView_ *)manager)->pair;
    m_player = p.a;
    m_cursor = p.b;
""" + TAIL,
    "local_get_members": H + """    UnkPausePair_ p = ((UnkPauseMgrView_ *)manager)->Get();
    m_player = p.a;
    m_cursor = p.b;
""" + TAIL,
}}
