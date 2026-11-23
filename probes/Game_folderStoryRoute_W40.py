"""Wave 40: folderStoryRoute's constructor reading the pause manager through a pointer to const."""
F = "__ct__16folderStoryRouteFPQ34Chao3CSD8CProject"
SCORE = [F]
L = "    TPauseManager *manager = fn_800BADE8();\n    m_managerState = manager->m_state;\n    m_player = manager->m_player;\n    m_cursor = manager->m_cursor;\n    InitScenes(project);\n    m_route.Reset();"
def v(new): return [(L, new + "\n    InitScenes(project);\n    m_route.Reset();")]
REPL = {
    "const_local": v("    const TPauseManager *manager = fn_800BADE8();\n    m_managerState = manager->m_state;\n    m_player = manager->m_player;\n    m_cursor = manager->m_cursor;"),
    "inline_const": [("folderStoryRoute::folderStoryRoute(Chao::CSD::CProject *project) : m_check(0) {", "inline void folderStoryRoute::CopyManager(const TPauseManager *manager) {\n    m_managerState = manager->m_state;\n    m_player = manager->m_player;\n    m_cursor = manager->m_cursor;\n}\n\nfolderStoryRoute::folderStoryRoute(Chao::CSD::CProject *project) : m_check(0) {")] + v("    CopyManager(fn_800BADE8());"),
    "inline_const_ref": [("folderStoryRoute::folderStoryRoute(Chao::CSD::CProject *project) : m_check(0) {", "inline void folderStoryRoute::CopyManager(const TPauseManager &manager) {\n    m_managerState = manager.m_state;\n    m_player = manager.m_player;\n    m_cursor = manager.m_cursor;\n}\n\nfolderStoryRoute::folderStoryRoute(Chao::CSD::CProject *project) : m_check(0) {")] + v("    CopyManager(*fn_800BADE8());"),
}

_H = "    void InitScenes(Chao::CSD::CProject *project);"
_hs = open("include/Game/TGiPause.h", encoding="utf-8").read()
_i = _hs.index("folderStoryRoute")
_seg = _hs[_i:]
_k = _i + _seg.index(_H)
# anchor unique to folderStoryRoute's declaration: the text from the class name to its InitScenes
_ANCH = _hs[_k - 120:_k + len(_H)]
HDR = {
    "inline_const": {"Game/TGiPause.h": [(_ANCH, _ANCH + chr(10) + "    void CopyManager(const TPauseManager *manager);")]},
    "inline_const_ref": {"Game/TGiPause.h": [(_ANCH, _ANCH + chr(10) + "    void CopyManager(const TPauseManager &manager);")]},
    "ret_const": {"Game/TGiPause.h": [('extern "C" TPauseManager *fn_800BADE8(void);', 'extern "C" const TPauseManager *fn_800BADE8(void);')]},
}
REPL["ret_const"] = REPL["const_local"]
