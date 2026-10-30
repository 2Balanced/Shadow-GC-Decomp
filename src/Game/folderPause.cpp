/* folderPause (class name from the RTTI of its shared_ptr control block in Game/TGiPause.cpp): the
 * pause menu proper. It plays the scenes of the scene list Root/Pause (the mission board, the
 * three players' icons, the cursor and the resume/restart/quit text), follows the pause manager's
 * cursor and player, and prints the mission's text through the hint message source. The file name
 * and the names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#define TGI_NO_NUMBER_COLORS
#define TGI_HANDLE_ASSIGN
#include "Game/CsdRCPtr.h"
#include "Game/CsdSceneCtrl.h"
#include "Game/StageTable.h"
#include "Game/TGi.h"
#include "Game/TGiPause.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* The message source (class name unknown; see Game/StageManager.cpp). */
class UnkHintTarget_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x64];
    /* 0x64 */ float m_scale;
};

/* A node's position (struct name unknown; see Game/SavePointSelect.cpp). */
struct UnkCsdPosition_8036AE98 {
    float x;
    float y;
};

/* The empty node pointer (as lbl_805EE110 is the empty texture list). */
extern RCPtr<CNode> lbl_805EE11C;

extern "C" {
/* The scene list at path of project, and a scene of a scene list by name (behaviour inferred; see
 * Game/csdmProject.cpp). */
UnkCsdHandle_80042DF0 fn_8036D2FC(Chao::CSD::CProject *project, const char *path, void *factory);
RCPtr<CScene> fn_8036F470(void *object, const char *name);
/* Whether a scene's queued motions have all been played (see Game/CsdSceneSet.h). */
bool fn_8003DBB0(UnkCsdSceneCtrl_8003DD20 *scene);
/* Nodes: the position, moving, and hiding (behaviour inferred). */
UnkCsdPosition_8036AE98 fn_8036AE98(CNode *node);
void fn_8036AE70(CNode *node, const UnkCsdPosition_8036AE98 *position);
void fn_8036AD78(CNode *node, int hidden);
/* The stage's message source, the console's region, and printing a message (see
 * Game/GoalAction.cpp, Game/SaveCycleMessage.cpp). */
boost::shared_ptr<UnkHintTarget_801FB180> fn_80177C80(void);
s32 fn_80305BE8(void);
void *fn_80049BC0(UnkHintTarget_801FB180 *target, s32 id, s32 branch, s32 index, float x, float y,
                  s32 *voice, u8 changed);
}

/* Handle comparison (name inferred; as in Game/ResultTask.cpp). */
inline bool operator!=(const UnkCsdHandle_80042DF0 &a, const UnkCsdHandle_80042DF0 &b) {
    void *pa = a.m_object != 0 ? *(void **)a.m_object : 0;
    void *pb = b.m_object != 0 ? *(void **)b.m_object : 0;
    if (pa != pb) {
        return true;
    }
    return false;
}

/* Node comparison (name inferred; as the handle comparison below). */
inline bool operator!=(const RCPtr<CNode> &a, const RCPtr<CNode> &b) {
    CNode *pa = a.m_object != 0 ? a.m_object->m_object : 0;
    CNode *pb = b.m_object != 0 ? b.m_object->m_object : 0;
    if (pa != pb) {
        return true;
    }
    return false;
}

/* The mission of player in a stage entry (name inferred). */
inline const UnkStageMission_804C4A18 *GetMission(const UnkStageInfo_804C4A18 *info) {
    return info->m_missions;
}

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (the
 * linker drops the table; the literals below reuse these copies). The .rodata is kept whole, so it
 * also holds the RTTI names of the CSD and boost headers' inline classes. */
static const char *s_rodataPool[] = {
    "MissionClose",
    "HeroClose",
    "NormalClose",
    "DarkClose",
    "MBoardClose",
    "CMissionClose",
    "PauseClose",
    "TextClose",
    "arrowLend",
    "arrowRend",
    "DarkSelect",
    "NormalStationary",
    "HeroStationary",
    "DarkStationary",
    "NormalSelect",
    "HeroSelect",
    "MissionOpen",
    "HeroOpen",
    "NormalOpen",
    "DarkOpen",
    "MBoardOpen",
    "CMissionOpen",
    "PauseOpen",
    "TextOpen",
    "nrestart",
    "nresume2",
    "Not inititalized m_eDealloctor\n",
    "Root/Pause",
    "MIconHero",
    "MIconNormal",
    "MIconDark",
    "Mission_Board",
    "ChangeMission",
    "PauseText",
    "csdRCObject.h",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>",
    "boost::detail::sp_counted_base",
};

/* The position and size of the mission's text (meaning inferred). */
static float s_textLayout[3] = {0.63f, 0.25f, 0.036f};

extern "C" TPauseManager *fn_800BADE8(void) {
    return lbl_805EF9F4;
}

/* NonMatching (99.8%): retail loads the manager's player and cursor into two registers before
 * storing either; GC/2.7 stores the player before loading the cursor. Locals for the two values,
 * their order, struct copies and inline helpers did not reproduce it. */
folderPause::folderPause(Chao::CSD::CProject *project) {
    TPauseManager *manager = fn_800BADE8();
    m_managerState = manager->m_state;
    s32 player = manager->m_player;
    s32 cursor = manager->m_cursor;
    m_player = player;
    m_cursor = cursor;
    InitScenes(project);
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
}

folderPause::~folderPause() {}

/* Retail has SetScene here, between the destructor and InitScenes. It was probably an inline of
 * the scene control's header whose body called other inline functions (so it stayed out of line);
 * outside the CSD library this unit sees RCPtr's assignment only as a declaration, so a plain
 * definition stands in for it. */
void UnkCsdSceneCtrl_8003DD20::SetScene(const RCPtr<CScene> &scene) {
    m_scene = scene;
}

void folderPause::InitScenes(Chao::CSD::CProject *project) {
    m_sceneList = fn_8036D2FC(project, "Root/Pause", 0);
    if (m_sceneList != UnkCsdHandle_80042DF0(0)) {
        m_mission.SetScene(fn_8036F470(m_sceneList.Get(), "Mission"));
        m_heroIcon.SetScene(fn_8036F470(m_sceneList.Get(), "MIconHero"));
        m_normalIcon.SetScene(fn_8036F470(m_sceneList.Get(), "MIconNormal"));
        m_darkIcon.SetScene(fn_8036F470(m_sceneList.Get(), "MIconDark"));
        m_missionBoard.SetScene(fn_8036F470(m_sceneList.Get(), "Mission_Board"));
        m_changeMission.SetScene(fn_8036F470(m_sceneList.Get(), "ChangeMission"));
        m_pause.SetScene(fn_8036F470(m_sceneList.Get(), "Pause"));
        m_text.SetScene(fn_8036F470(m_sceneList.Get(), "PauseText"));
        m_storyRouteText.SetScene(fn_8036F470(m_sceneList.Get(), "SRText"));
    }
    if (lbl_8057D6A0.field_0xCC != 1 || (fn_80176C3C(fn_800B74B4(), -1)->m_flags & 0x10)) {
        m_showStoryRoute = 0;
    } else {
        m_showStoryRoute = 1;
    }
    UnkStageInfo_804C4A18 *info = fn_80176C3C(fn_800B74B4(), -1);
    if (info != 0) {
        for (int i = 0; i < 3; i++) {
            if (info->m_missions[i].m_id == -1) {
                RCPtr<CNode> node;
                switch (i) {
                case 0:
                    node = fn_803666B4(m_darkIcon.GetScene().Get(), "dark");
                    break;
                case 1:
                    node = fn_803666B4(m_normalIcon.GetScene().Get(), "normal");
                    break;
                case 2:
                    node = fn_803666B4(m_heroIcon.GetScene().Get(), "hero");
                    break;
                }
                if (node != lbl_805EE11C) {
                    fn_8036AD14(node.Get(), 0);
                }
            }
        }
    }
}

void folderPause::Open() {
    m_mission.Play("MissionOpen", 1);
    m_heroIcon.Play("HeroOpen", 1);
    m_normalIcon.Play("NormalOpen", 1);
    m_darkIcon.Play("DarkOpen", 1);
    m_missionBoard.Play("MBoardOpen", 1);
    m_changeMission.Play("CMissionOpen", 1);
    m_pause.Play("PauseOpen", 1);
    m_text.Play("TextOpen", 1);
    {
    RCPtr<CNode> resume = fn_803666B4(m_text.GetScene().Get(), "resume");
    RCPtr<CNode> restart = fn_803666B4(m_text.GetScene().Get(), "restart");
    RCPtr<CNode> quit = fn_803666B4(m_text.GetScene().Get(), "quit");
    if (lbl_805EF9F4->m_canRestart) {
        RCPtr<CNode> resumePos = fn_803666B4(m_text.GetScene().Get(), "nresume");
        RCPtr<CNode> restartPos = fn_803666B4(m_text.GetScene().Get(), "nrestart");
        RCPtr<CNode> quitPos = fn_803666B4(m_text.GetScene().Get(), "nquit");
        UnkCsdPosition_8036AE98 position = fn_8036AE98(resumePos.Get());
        fn_8036AE70(resume.Get(), &position);
        UnkCsdPosition_8036AE98 position2 = fn_8036AE98(restartPos.Get());
        fn_8036AE70(restart.Get(), &position2);
        UnkCsdPosition_8036AE98 position3 = fn_8036AE98(quitPos.Get());
        fn_8036AE70(quit.Get(), &position3);
        fn_8036AD78(restart.Get(), 0);
    } else {
        RCPtr<CNode> resumePos = fn_803666B4(m_text.GetScene().Get(), "nresume2");
        RCPtr<CNode> quitPos = fn_803666B4(m_text.GetScene().Get(), "nquit2");
        UnkCsdPosition_8036AE98 position = fn_8036AE98(resumePos.Get());
        fn_8036AE70(resume.Get(), &position);
        UnkCsdPosition_8036AE98 position2 = fn_8036AE98(quitPos.Get());
        fn_8036AE70(quit.Get(), &position2);
        fn_8036AD78(restart.Get(), 1);
    }
    }
    if (m_showStoryRoute) {
        m_storyRouteText.Play("SROpen", 1);
    }
    UpdateCursor();
    UpdatePlayer();
    m_state = 1;
}

void folderPause::UpdateCursor() {
    switch (lbl_805EF9F4->m_cursor) {
    case 0:
        m_text.Play("Resume", 1);
        break;
    case 1:
        m_text.Play("Restart", 1);
        break;
    case 2:
        m_text.Play("Quit", 1);
        break;
    }
    m_cursor = lbl_805EF9F4->m_cursor;
}

void folderPause::UpdatePlayer() {
    switch (lbl_805EF9F4->m_player) {
    case 0:
        m_changeMission.Play("arrowLend", 1);
        break;
    case 1:
        m_changeMission.Play("arrowM", 1);
        break;
    case 2:
        m_changeMission.Play("arrowRend", 1);
        break;
    }
    switch (lbl_805EF9F4->m_player) {
    case 0:
        m_darkIcon.Play("DarkSelect", 1);
        m_normalIcon.Play("NormalStationary", 1);
        m_heroIcon.Play("HeroStationary", 1);
        break;
    case 1:
        m_darkIcon.Play("DarkStationary", 1);
        m_normalIcon.Play("NormalSelect", 1);
        m_heroIcon.Play("HeroStationary", 1);
        break;
    case 2:
        m_darkIcon.Play("DarkStationary", 1);
        m_normalIcon.Play("NormalStationary", 1);
        m_heroIcon.Play("HeroSelect", 1);
        break;
    }
    m_player = lbl_805EF9F4->m_player;
}

bool folderPause::Update() {
    switch (m_state) {
    case 0:
        if (lbl_805EF9F4->m_state == 2 || lbl_805EF9F4->m_state == 3) {
            m_managerState = lbl_805EF9F4->m_state;
            Open();
        }
        break;
    case 1:
        if (fn_8003DBB0(&m_mission)) {
            m_state = 2;
        }
        break;
    case 2:
        if (m_player != lbl_805EF9F4->m_player) {
            int player = lbl_805EF9F4->m_player;
            if (player < m_player) {
                m_changeMission.Play("arrowL", 1);
            } else if (player > m_player) {
                m_changeMission.Play("arrowR", 1);
            }
            UpdatePlayer();
        }
        if (m_cursor != lbl_805EF9F4->m_cursor) {
            UpdateCursor();
        }
        if (lbl_805EF9F4->m_state != 2 && lbl_805EF9F4->m_state != 3) {
            m_mission.Play("MissionClose", 1);
            m_heroIcon.Play("HeroClose", 1);
            m_normalIcon.Play("NormalClose", 1);
            m_darkIcon.Play("DarkClose", 1);
            m_missionBoard.Play("MBoardClose", 1);
            m_changeMission.Play("CMissionClose", 1);
            m_pause.Play("PauseClose", 1);
            m_text.Play("TextClose", 1);
            if (m_showStoryRoute) {
                m_storyRouteText.Play("SRClose", 1);
            }
            m_state = 3;
        }
        break;
    case 3:
        if (fn_8003DBB0(&m_mission)) {
            m_managerState = lbl_805EF9F4->m_state;
            m_state = 0;
        }
        break;
    }
    int busy = m_mission.Update(false);
    busy += m_heroIcon.Update(false);
    busy += m_normalIcon.Update(false);
    busy += m_darkIcon.Update(false);
    busy += m_missionBoard.Update(false);
    busy += m_changeMission.Update(false);
    busy += m_pause.Update(false);
    busy += m_text.Update(false);
    busy += m_storyRouteText.Update(false);
    return busy != 0;
}

void folderPause::Draw() {
    switch (m_state) {
    case 2:
        if (!lbl_8057D6A0.field_0x125 &&
            GetMission(fn_80176C3C(fn_800B74B4(), -1))[lbl_805EF9F4->m_player].m_id != -1) {
            int id = fn_80176C3C(fn_800B74B4(), -1)->field_0x44;
            if (id > 0) {
                boost::shared_ptr<UnkHintTarget_801FB180> target = fn_80177C80();
                if (target.get() != 0) {
                    target->m_scale = s_textLayout[2];
                    if (fn_80305BE8()) {
                        fn_80049BC0(target.get(), id, lbl_805EF9F4->m_player, 0, s_textLayout[0],
                                    1.24f * s_textLayout[1], 0, 0);
                    } else {
                        fn_80049BC0(target.get(), id, lbl_805EF9F4->m_player, 0, s_textLayout[0],
                                    s_textLayout[1], 0, 0);
                    }
                }
            }
        }
        break;
    }
}
