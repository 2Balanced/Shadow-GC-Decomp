/* StageAction::Result2PAction (class name from RTTI): the stage action that ends a round of the
 * two-player mode. It loads the CSD project "csdFiles/2P_Result" (scenes "TimeUp" and "Result"),
 * decides the round from which player is out and from the players' rings, counts the round and the
 * win in the game state, plays "Default" or "Draw", fades to white when a player has won the match
 * (half the rounds plus one) and to black otherwise, plays the winner's event, then shows
 * "Winner" or moves on to the next round (state 7), the game over (11) or the match's end (16).
 * File name inferred. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CsdRCPtr.h"
#include "Game/Render.h"
#include "Game/ScreenFade.h"
#include "Game/StageAction.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

namespace Chao {
namespace CSD {
/* CTexList (class name from the RTTI of RCPtr<CTexList>::RCObject<CTexList>): a project's texture
 * list. */
class CTexList;
} // namespace CSD
} // namespace Chao

using Chao::CSD::CTexList;

/* The loaded CSD project (Chao::CSD::CProject, Game/csdmProject.cpp; declared as in Game/TGi.h). */
namespace Chao {
namespace CSD {
class CProject {
  public:
    CProject();
    virtual ~CProject();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
};
} // namespace CSD
} // namespace Chao

/* The value a project returns from fn_8036D598 (class name unknown; declared as in Game/TGi.h). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* 0x0 */ void *m_object;
};

/* The current stage's record in the game state (struct name unknown; see Game/StageRecord.cpp). */
struct UnkStageRecord_8057D748 {
    /* 0x00 */ u8 field_0x0[0x24];
};

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x18];
    /* 0x018 */ float m_frameTime; /* the frame's duration (meaning inferred) */
    /* 0x01C */ u8 field_0x1C[0x60 - 0x1C];
    /* 0x060 */ u32 field_0x60; /* meaning unknown (GoalAction clears it too) */
    /* 0x064 */ u8 field_0x64[0xA8 - 0x64];
    /* 0x0A8 */ UnkStageRecord_8057D748 m_stageRecord;
    /* 0x0CC */ u8 field_0xCC[0xD1 - 0xCC];
    /* 0x0D1 */ u8 field_0xD1; /* meaning unknown */
    /* 0x0D2 */ u8 field_0xD2[0x12C - 0xD2];
    /* 0x12C */ s32 m_rounds; /* the rounds of a match (meaning inferred) */
    /* 0x130 */ s32 m_roundsPlayed;
    /* 0x134 */ s32 m_wins[2];
    /* 0x13C */ s32 m_characters[2]; /* each player's character type (meaning inferred) */
    /* 0x144 */ s32 m_lastWinner; /* -1 for a draw */
    /* 0x148 */ u8 m_hudVisible; /* meaning inferred */
    /* 0x149 */ u8 field_0x149[0x150 - 0x149];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A player's record (struct name unknown; see Game/TGi.h). */
struct UnkPlayerInfo_80178B5C {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ u8 m_rings[0x4]; /* read by fn_8016F280 (meaning inferred) */
};

/* What fn_801723E0 returns (class name unknown; see Game/GoalAction.cpp). */
struct UnkEventHolder_801723E0 {
    /* The task, if any (name inferred). */
    Task *GetTask() {
        return m_task ? m_task : 0;
    }

    /* 0x0 */ Task *m_task;
};

/* The event task (Game/PjsEventActionManager.cpp; only its state is read here). */
class TPjsEventActionManager : public Task {
  public:
    /* 0x28 */ s32 m_state; /* 5 once the event is ready (meaning inferred) */
};

/* The stage blocks of the first player (see Game/GoalAction.cpp). */
struct UnkBlockManage_8051CE88 {
    /* 0x000 */ u8 field_0x0[0x40];
    /* 0x040 */ s32 field_0x40;
    /* 0x044 */ u8 field_0x44[0x978 - 0x44];
    /* 0x978 */ u8 field_0x978;
};

extern UnkBlockManage_8051CE88 *lbl_8051CE88[4];

/* What the file system's load request takes (struct name unknown; as in Game/ResultTask.cpp). */
struct Result2PRequest {
    /* 0x00 */ const char *m_path;
    /* 0x04 */ void *m_data;
    /* 0x08 */ u32 m_size;
    /* 0x0C */ void *m_user;
    /* 0x10 */ void (*m_callback)(Result2PRequest *request);
};

class UnkFileSystem_800371D8;
struct UnkCharaList_80074194;
struct UnkStageData_80079808;
struct UnkSound_80053164;

extern "C" {
UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int number);
void fn_800A8D48(UnkCharaList_80074194 *list);
void fn_800A8D9C(UnkCharaList_80074194 *list);
void fn_800A8DFC(UnkCharaList_80074194 *list);
/* Whether a character is out (behaviour inferred; see Game/BossCtrlUnit.cpp). */
bool fn_800B77A0(const boost::shared_ptr<CharInfo> *chara);
UnkStageData_80079808 *fn_80079808(void);
UnkPlayerInfo_80178B5C *fn_80178B5C(UnkStageData_80079808 *data, int player);
/* The value of a ring counter (behaviour inferred). */
int fn_8016F280(u8 *counter);
/* Sets or clears the stage record's flag 0x20 (behaviour inferred). */
bool fn_80203970(UnkStageRecord_8057D748 *record, bool set);
/* Creates a Render (behaviour inferred; see Game/StageMovie.cpp). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* A file's path, and its language-specific path (behaviour inferred). */
const char *fn_800C02CC(const char *name);
void *fn_8003F8D4(void);
const char *fn_800423B8(void *arg0, const char *name, int arg2);
/* The file system, a load request to it, and the release of loaded data (as in
 * Game/ResultTask.cpp). */
UnkFileSystem_800371D8 *fn_800371D8(void);
bool fn_80045044(UnkFileSystem_800371D8 *fileSystem, Result2PRequest *request, int arg2);
void fn_80049B34(void *data);
/* CSD (behaviour inferred; see Game/TGi.h and Game/AdvCsdCache.cpp): a texture list of a type,
 * loading textures from memory into it, giving it to a project, loading a project from memory,
 * advancing and drawing a project, a scene by name, and the scene calls. */
RCPtr<CTexList> fn_80362094(u32 type);
void fn_80361F30(CTexList *texList, void *data, u32 size, int arg3);
void fn_8036C64C(Chao::CSD::CProject *project, const RCPtr<CTexList> &texList);
void fn_8036DB70(Chao::CSD::CProject *project, void *data, u32 size, int arg3);
void fn_8036DAD4(Chao::CSD::CProject *project);
void fn_8036D94C(Chao::CSD::CProject *project);
UnkCsdHandle_80042DF0 fn_8036D598(Chao::CSD::CProject *project, int arg1);
RCPtr<CScene> fn_8036D660(Chao::CSD::CProject *project, const char *name);
void fn_80367238(CScene *scene, const char *motion);
void fn_80367140(CScene *scene, float speed);
void fn_803659DC(CScene *scene, int arg1);
/* Sounds (see Game/GoalAction.cpp). */
UnkSound_80053164 *fn_80053164(void);
void fn_801D522C(UnkSound_80053164 *sound);
void fn_801D51A0(UnkSound_80053164 *sound, s32 voice, int arg2, int arg3);
/* The event of the round's winner (see Game/GoalAction.cpp). */
UnkEventHolder_801723E0 *fn_801723E0(void);
void fn_8017430C(UnkEventHolder_801723E0 *holder, s32 event);
void fn_801742E0(UnkEventHolder_801723E0 *holder);
void fn_80172F48(TPjsEventActionManager *manager, bool play);
/* Fades the screen in (Game/PjsEventActionManager.cpp; argument meanings inferred). */
void fn_80173834(int arg0, float time);
}

namespace StageAction {

/* The load callbacks of the textures and of the project file (names inferred). */
static void OnTexturesLoaded(Result2PRequest *request);
static void OnProjectLoaded(Result2PRequest *request);

Result2PAction::Result2PAction() {
    fn_800A8D48(fn_80074194());
    fn_80203970(&lbl_8057D6A0.m_stageRecord, true);
    m_savedD1 = lbl_8057D6A0.field_0xD1;
    StageActionManager *manager = fn_80074470();
    manager->m_locked = 1;
    m_render = fn_8004E24C(fn_80046178()->field_0x54);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&Result2PAction::Draw, this, _1));
    m_project = 0;
    Result2PRequest request;
    request.m_path = fn_800C02CC("csdFiles/2P_Result");
    request.m_data = 0;
    request.m_size = 0;
    request.m_user = this;
    request.m_callback = OnTexturesLoaded;
    fn_80045044(fn_800371D8(), &request, 1);
    m_texData = 0;
    m_texSize = 0;
    m_texFailed = 0;
    request.m_path = fn_800423B8(fn_8003F8D4(), "csdFiles/2P_Result", 0);
    request.m_data = 0;
    request.m_size = 0;
    request.m_user = this;
    request.m_callback = OnProjectLoaded;
    fn_80045044(fn_800371D8(), &request, 1);
    m_projectData = 0;
    m_projectSize = 0;
    m_projectFailed = 0;
    boost::shared_ptr<CharInfo> player1(fn_800A8C54(fn_80074194(), 0).lock());
    boost::shared_ptr<CharInfo> player2(fn_800A8C54(fn_80074194(), 1).lock());
    if (player1 && player2) {
        bool out1 = fn_800B77A0(&player1);
        bool out2 = fn_800B77A0(&player2);
        if (!out1 && !out2) {
            int rings1 = fn_8016F280(fn_80178B5C(fn_80079808(), 0)->m_rings);
            int rings2 = fn_8016F280(fn_80178B5C(fn_80079808(), 1)->m_rings);
            if (rings1 == rings2) {
                lbl_8057D6A0.m_lastWinner = -1;
                m_winner = 0;
            } else if (rings1 < rings2) {
                lbl_8057D6A0.m_lastWinner = 1;
                lbl_8057D6A0.m_roundsPlayed++;
                lbl_8057D6A0.m_wins[1]++;
                m_winner = 1;
            } else {
                lbl_8057D6A0.m_lastWinner = 0;
                lbl_8057D6A0.m_roundsPlayed++;
                lbl_8057D6A0.m_wins[0]++;
                m_winner = 1;
            }
            m_state = 0;
        } else if (out1 && out2) {
            lbl_8057D6A0.m_lastWinner = -1;
            m_winner = 0;
            m_state = 2;
        } else if (out1) {
            lbl_8057D6A0.m_lastWinner = 1;
            lbl_8057D6A0.m_roundsPlayed++;
            lbl_8057D6A0.m_wins[1]++;
            m_winner = 1;
            m_state = 4;
            m_wait = 1.0f;
        } else {
            lbl_8057D6A0.m_lastWinner = 0;
            lbl_8057D6A0.m_roundsPlayed++;
            lbl_8057D6A0.m_wins[0]++;
            m_winner = 1;
            m_state = 4;
            m_wait = 1.0f;
        }
    }
}

Result2PAction::~Result2PAction() {
    lbl_8057D6A0.field_0xD1 = m_savedD1;
    fn_800A8DFC(fn_80074194());
    if (m_project != 0) {
        delete m_project;
    }
    if (m_texData != 0) {
        fn_80049B34(m_texData);
    }
    if (m_projectData != 0) {
        fn_80049B34(m_projectData);
    }
}

void Result2PAction::Execute() {
    RCPtr<CScene> scene;
    if (m_project != 0) {
        fn_8036DAD4(m_project);
    } else if (m_texData != 0 && m_projectData != 0) {
        m_project = new Chao::CSD::CProject;
        RCPtr<CTexList> texList = fn_80362094(0x2E525754);
        fn_80361F30(texList.Get(), m_texData, m_texSize, 0);
        fn_8036C64C(m_project, texList);
        fn_8036DB70(m_project, m_projectData, m_projectSize, 0);
        fn_8036D598(m_project, 0);
        scene = fn_8036D660(m_project, "Result");
        fn_803659DC(scene.Get(), 0);
        fn_80367140(scene.Get(), 0.0f);
        scene = fn_8036D660(m_project, "TimeUp");
        fn_803659DC(scene.Get(), 0);
        fn_80367140(scene.Get(), 0.0f);
    }
    m_wait -= lbl_8057D6A0.m_frameTime;
    switch (m_state) {
    case 0:
        if (m_project != 0) {
            scene = fn_8036D660(m_project, "TimeUp");
            fn_80367238(scene.Get(), "Default");
            fn_80367140(scene.Get(), 1.0f);
            m_state = 1;
        }
        break;
    case 1: {
        s32 end = fn_8036D660(m_project, "TimeUp").Get()->m_end;
        if (end != 0) {
            if (m_winner == 0) {
                m_state = 2;
            } else {
                fn_801D522C(fn_80053164());
                lbl_8057D6A0.field_0xD1 = 0;
                int needed = lbl_8057D6A0.m_rounds / 2 + 1;
                if (lbl_8057D6A0.m_wins[0] == needed || lbl_8057D6A0.m_wins[1] == needed) {
                    static const RwRGBA white = {255, 255, 255, 255};
                    fn_80204874(&white);
                    m_winner = 2;
                } else {
                    static const RwRGBA black = {0, 0, 0, 0};
                    fn_80204874(&black);
                }
                fn_80173878(0, 1, 1.0f);
                m_state = 5;
            }
        }
        break;
    }
    case 2:
        if (m_project != 0) {
            scene = fn_8036D660(m_project, "Result");
            fn_80367238(scene.Get(), "Draw");
            fn_80367140(scene.Get(), 1.0f);
            m_state = 3;
        }
        break;
    case 3: {
        s32 end = fn_8036D660(m_project, "Result").Get()->m_end;
        if (end != 0) {
            static const RwRGBA black = {0, 0, 0, 0};
            fn_80204874(&black);
            fn_80173878(0, 1, 1.0f);
            m_state = 11;
        }
        break;
    }
    case 4:
        if (m_wait < 0.0f) {
            fn_801D522C(fn_80053164());
            lbl_8057D6A0.field_0xD1 = 0;
            int needed = lbl_8057D6A0.m_rounds / 2 + 1;
            if (lbl_8057D6A0.m_wins[0] == needed || lbl_8057D6A0.m_wins[1] == needed) {
                static const RwRGBA white = {255, 255, 255, 255};
                fn_80204874(&white);
                m_winner = 2;
            } else {
                static const RwRGBA black = {0, 0, 0, 0};
                fn_80204874(&black);
            }
            fn_80173878(0, 1, 1.0f);
            m_state = 5;
        }
        break;
    case 5:
        if (fn_80044E10() == 1) {
            s32 event;
            switch (lbl_8057D6A0.m_characters[lbl_8057D6A0.m_lastWinner]) {
            case 0xE:
                event = 0x3B7;
                break;
            case 0xF:
                event = 0x3D3;
                break;
            case 0x10:
                event = 0x3D1;
                break;
            case 0x11:
                event = 0x3CB;
                break;
            case 0x12:
                event = 0x3CF;
                break;
            case 0x13:
                event = 0x3CD;
                break;
            default:
                event = 0x3B6;
                break;
            }
            fn_8017430C(fn_801723E0(), event);
            lbl_8057D6A0.m_hudVisible = 0;
            fn_800A8D9C(fn_80074194());
            m_state = 6;
        }
        break;
    case 6: {
        TPjsEventActionManager *event = (TPjsEventActionManager *)fn_801723E0()->GetTask();
        if (event != 0) {
            if (event->m_state == 5) {
                fn_80172F48(event, true);
                m_timer = 0;
                m_state = 7;
            }
        } else {
            m_timer = 0;
            m_state = 7;
        }
        break;
    }
    case 7:
        m_timer++;
        if (m_timer > 2 && !(lbl_8051CE88[0]->field_0x978 | (lbl_8051CE88[0]->field_0x40 != 0))) {
            TPjsEventActionManager *event = (TPjsEventActionManager *)fn_801723E0()->GetTask();
            if (event != 0) {
                fn_80172F48(event, false);
            }
            fn_80173834(1, 0.0f);
            lbl_8057D6A0.field_0x60 = 0;
            m_state = 8;
            m_wait = 3.8f;
        }
        break;
    case 8:
        if (fn_80044E10() == 0 && m_project != 0) {
            if (m_winner == 2) {
                scene = fn_8036D660(m_project, "Result");
                fn_80367238(scene.Get(), "Winner");
                fn_80367140(scene.Get(), 1.0f);
                m_state = 10;
                if (lbl_8057D6A0.m_rounds == 1) {
                    fn_801D51A0(fn_80053164(), 0x28, -1, -1);
                } else if (lbl_8057D6A0.m_rounds == lbl_8057D6A0.m_roundsPlayed) {
                    fn_801D51A0(fn_80053164(), 0x28, -1, -1);
                } else {
                    fn_801D51A0(fn_80053164(), 0x24, -1, -1);
                }
            } else {
                m_state = 9;
                if (lbl_8057D6A0.m_roundsPlayed == 1) {
                    fn_801D51A0(fn_80053164(), 0x22, -1, -1);
                } else {
                    fn_801D51A0(fn_80053164(), 0x26, -1, -1);
                }
            }
        }
        break;
    case 9:
        if (m_wait < 0.0f) {
            static const RwRGBA black = {0, 0, 0, 0};
            fn_80204874(&black);
            fn_80173878(0, 1, 1.0f);
            m_state = 11;
        }
        break;
    case 10:
        if (m_wait < 0.0f) {
            static const RwRGBA black = {0, 0, 0, 0};
            fn_80204874(&black);
            fn_80173878(0, 1, 1.0f);
            m_state = 11;
        }
        break;
    case 11:
        if (fn_80044E10() == 1) {
            if (m_winner == 0) {
                fn_80175EEC(fn_80074470(), 7, true);
            } else {
                lbl_8057D6A0.field_0xD1 = 1;
                fn_801742E0(fn_801723E0());
                if (m_winner == 2) {
                    fn_80175EEC(fn_80074470(), 11, true);
                } else {
                    fn_80074470()->SetResult(4);
                    fn_80175EEC(fn_80074470(), 16, true);
                }
            }
        }
        break;
    }
}

void Result2PAction::Draw(RednerDrawArgs &args) {
    if (m_project != 0) {
        fn_8036D94C(m_project);
    }
}

static void OnTexturesLoaded(Result2PRequest *request) {
    void *data = request->m_data;
    Result2PAction *action = (Result2PAction *)request->m_user;
    u32 size = request->m_size;
    action->m_texData = data;
    action->m_texSize = size;
    if (data == 0) {
        action->m_texFailed = 1;
    }
}

static void OnProjectLoaded(Result2PRequest *request) {
    void *data = request->m_data;
    Result2PAction *action = (Result2PAction *)request->m_user;
    u32 size = request->m_size;
    action->m_projectData = data;
    action->m_projectSize = size;
    if (data == 0) {
        action->m_projectFailed = 1;
    }
}

} // namespace StageAction
