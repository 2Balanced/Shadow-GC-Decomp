/* StageAction::ShowAction (class name from RTTI; declared in Game/StageAction.h): the stage action
 * that opens a 2P battle. It plays each player's introduction event, shows the "Start" screen of
 * the "CsdFiles/2P_Start" project with the player's label, and then lets the stage play (state 4
 * asks the stage action manager for state 3). File name from the class; all other names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CsdRCPtr.h"
#include "Game/Render.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The loaded CSD project (class name unknown; see Game/TGi.h). */
class UnkCsdResource_8003E808 {
  public:
    UnkCsdResource_8003E808();
    virtual ~UnkCsdResource_8003E808();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
};

#define STAGEACTION_SHOWACTION_IMPL
#include "Game/StageAction.h"

using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

namespace Chao {
namespace CSD {

/* RCPtr's operator-> (as in Game/TGi.h); retail calls it out of line. */
template <class T> inline T *RCPtr<T>::operator->() const {
    return Get();
}

} // namespace CSD
} // namespace Chao

/* The value a project returns from fn_8036D598 (class name unknown; see Game/TGi.h). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* 0x0 */ void *m_object;
};

/* The current stage's record in the game state (see Game/ContinueAction.cpp). */
struct UnkStageRecord_8057D748 {
    /* 0x00 */ u8 field_0x0[0x24];
};

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x1C];
    /* 0x01C */ float field_0x1C; /* meaning unknown (see Game/ContinueAction.cpp) */
    /* 0x020 */ u8 field_0x20[0x60 - 0x20];
    /* 0x060 */ s32 field_0x60; /* meaning unknown */
    /* 0x064 */ s32 field_0x64; /* meaning unknown */
    /* 0x068 */ s32 field_0x68; /* meaning unknown */
    /* 0x06C */ u8 field_0x6C[0xA8 - 0x6C];
    /* 0x0A8 */ UnkStageRecord_8057D748 m_stageRecord;
    /* 0x0CC */ u8 field_0xCC[0xD1 - 0xCC];
    /* 0x0D1 */ u8 field_0xD1; /* meaning unknown */
    /* 0x0D2 */ u8 field_0xD2[0x130 - 0xD2];
    /* 0x130 */ s32 field_0x130;      /* nonzero in the battle's later rounds (meaning inferred) */
    /* 0x134 */ u8 field_0x134[0x13C - 0x134];
    /* 0x13C */ s32 m_characters[2]; /* the characters the players picked, from 0xE on */
    /* 0x144 */ s32 m_winner;        /* the player who won the last round (meaning inferred) */
    /* 0x148 */ u8 m_hudVisible;
    /* 0x149 */ u8 field_0x149[0x150 - 0x149];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* Returns x (an inline call that keeps its caller out of line, as retail has GetTask). */
template <class T> inline T &EventRef(T &x) {
    return x;
}

/* What fn_801723E0 returns (class name unknown; see Game/PjsEventActionFade.cpp). */
struct UnkEventHolder_801723E0 {
    /* The event task, if any (name inferred). */
    Task *GetTask() {
        return EventRef(m_task) ? m_task : 0;
    }

    /* 0x0 */ Task *m_task;
};

/* The event task (Game/PjsEventActionManager.cpp; only the word at 0x4C is written here). */
struct UnkEventTask_801723E0 {
    /* 0x00 */ u8 field_0x0[0x4C];
    /* 0x4C */ s32 m_player; /* the player whose introduction plays (meaning inferred) */
};

struct UnkSound_80053164;

extern "C" {
/* The sound manager: a voice, the stage's music and its jingles (behaviour inferred; see
 * Game/AdvBgm.cpp). */
UnkSound_80053164 *fn_80053164(void);
void fn_801D51A0(UnkSound_80053164 *sound, int id, int arg2, int arg3);
void fn_801D4EA0(UnkSound_80053164 *sound);
void fn_801D4FBC(UnkSound_80053164 *sound, int arg1);
void fn_801D522C(UnkSound_80053164 *sound);
void fn_801D524C(UnkSound_80053164 *sound, const char *file);
/* Sets or clears the stage record's flag 0x20 (behaviour inferred). */
bool fn_80203970(UnkStageRecord_8057D748 *record, bool set);
/* The events: starting one, and whether the game is busy loading (behaviour inferred; see
 * Game/GoalAction.cpp). */
UnkEventHolder_801723E0 *fn_801723E0(void);
void fn_8017430C(UnkEventHolder_801723E0 *holder, int event);
int fn_80044E10(void);
/* Fades the screen (behaviour inferred; see Game/AdvBattleMode.cpp). */
void fn_80173834(int arg0, float time);
/* The resource manager's paths of a CSD file and its texture archive, and loading them into a
 * project (behaviour inferred; see Game/GiGameOver.cpp). */
void *fn_8003F8D4(void);
const char *fn_800C02CC(const char *name);
void fn_800422A8(UnkCsdResource_8003E808 *project, const char *file);
const char *fn_800423B8(void *arg0, const char *name, int arg2);
void fn_8036E114(UnkCsdResource_8003E808 *project, const char *file);
/* Projects and scenes (behaviour inferred; see Game/TGi.h). */
UnkCsdHandle_80042DF0 fn_8036D598(UnkCsdResource_8003E808 *project, int arg1);
RCPtr<CScene> fn_8036D660(UnkCsdResource_8003E808 *project, const char *name);
void fn_8036D94C(UnkCsdResource_8003E808 *project);
void fn_8036DAD4(UnkCsdResource_8003E808 *project);
void fn_80367238(CScene *scene, const char *motion);
float fn_803644A0(CScene *scene, const char *motion);
void fn_80366740(CScene *scene, float frame);
void fn_803659E4(CScene *scene);
void fn_803659DC(CScene *scene, int arg1);
/* Creates a Render (behaviour inferred; see Game/EggLastMechSlotDisp2D.cpp). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
}

namespace StageAction {

/* The introduction event of a character (name inferred). */
inline int IntroEvent(int character) {
    int event;
    switch (character) {
    case 0xE:
        event = 0x3B6;
        break;
    case 0xF:
        event = 0x3D2;
        break;
    case 0x10:
        event = 0x3D0;
        break;
    case 0x11:
        event = 0x3CA;
        break;
    case 0x12:
        event = 0x3CE;
        break;
    case 0x13:
        event = 0x3CC;
        break;
    default:
        event = 0x3B6;
        break;
    }
    return event;
}

ShowAction::ShowAction() : m_state(0) {
    fn_80203970(&lbl_8057D6A0.m_stageRecord, true);
    fn_801D4FBC(fn_80053164(), 1);
    lbl_8057D6A0.field_0x60 = 0;
    lbl_8057D6A0.field_0xD1 = 0;
    m_render = fn_8004E24C(fn_80046178()->field_0x54);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&ShowAction::Draw, this, _1));
    m_project = new UnkCsdResource_8003E808();
    if (m_project != 0) {
        fn_8003F8D4();
        fn_800422A8(m_project, fn_800C02CC("CsdFiles/2P_Start"));
        fn_8036E114(m_project, fn_800423B8(fn_8003F8D4(), "CsdFiles/2P_Start", 0));
        fn_8036D598(m_project, 0);
    }
    lbl_8057D6A0.m_hudVisible = 0;
    fn_801D522C(fn_80053164());
}

ShowAction::~ShowAction() {
    fn_801D4EA0(fn_80053164());
    lbl_8057D6A0.field_0xD1 = 1;
    if (m_project != 0) {
        delete m_project;
    }
    lbl_8057D6A0.m_hudVisible = 1;
    fn_801D524C(fn_80053164(), "sng_stg0800.adx");
}

void ShowAction::Execute() {
    RCPtr<CScene> scene;
    switch (m_state) {
    case 0: {
        fn_8017430C(fn_801723E0(), IntroEvent(lbl_8057D6A0.m_characters[0]));
        UnkEventTask_801723E0 *task = (UnkEventTask_801723E0 *)fn_801723E0()->GetTask();
        task->m_player = 0;
        m_state = 1;
        m_playing = 0;
        PlayVoice(0);
        break;
    }
    case 1:
        if (!m_playing) {
            if (fn_801723E0()->m_task == 0 || fn_80044E10() == 0) {
                scene = fn_8036D660(m_project, "Start");
                if (scene.Get() != 0) {
                    fn_80367238(scene.Get(), "1P");
                    fn_803659DC(scene.Get(), 0);
                    CScene *s = scene.Get();
                    fn_80366740(s, fn_803644A0(scene.Get(), "1P"));
                    fn_803659E4(scene.Get());
                }
                m_playing = 1;
            }
        } else if (fn_801723E0()->m_task == 0) {
            fn_8017430C(fn_801723E0(), IntroEvent(lbl_8057D6A0.m_characters[1]));
            UnkEventTask_801723E0 *task = (UnkEventTask_801723E0 *)fn_801723E0()->GetTask();
            task->m_player = 1;
            m_state = 2;
            m_playing = 0;
            PlayVoice(1);
        }
        break;
    case 2:
        if (!m_playing) {
            if (fn_801723E0()->m_task == 0 || fn_80044E10() == 0) {
                scene = fn_8036D660(m_project, "Start");
                if (scene.Get() != 0) {
                    fn_80367238(scene.Get(), "2P");
                    fn_803659DC(scene.Get(), 0);
                    CScene *s = scene.Get();
                    fn_80366740(s, fn_803644A0(scene.Get(), "2P"));
                    fn_803659E4(scene.Get());
                }
                m_playing = 1;
            }
        } else if (fn_801723E0()->m_task == 0) {
            fn_80173834(1, 0.0f);
            lbl_8057D6A0.field_0x1C = 1.0f;
            lbl_8057D6A0.field_0x60 = 1;
            lbl_8057D6A0.field_0x64 = 0;
            lbl_8057D6A0.field_0x68 = 1;
            m_state = 3;
            m_playing = 0;
            fn_801D4EA0(fn_80053164());
        }
        break;
    case 3:
        scene = fn_8036D660(m_project, "Start");
        if (!m_playing) {
            if (fn_80044E10() == 0) {
                scene = fn_8036D660(m_project, "Start");
                if (scene.Get() != 0) {
                    fn_80367238(scene.Get(), "Start");
                    fn_803659DC(scene.Get(), 0);
                    CScene *s = scene.Get();
                    fn_80366740(s, fn_803644A0(scene.Get(), "Start"));
                    fn_803659E4(scene.Get());
                }
                m_playing = 1;
            }
        } else if (scene->m_end != 0) {
            m_state = 4;
        }
        break;
    case 4:
        fn_80175EEC(fn_80074470(), 3, true);
        break;
    }
    if (m_project != 0 && m_playing) {
        fn_8036DAD4(m_project);
    }
}

void ShowAction::Draw(RednerDrawArgs &args) {
    if (m_project == 0 || !m_playing) {
        return;
    }
    fn_8036D94C(m_project);
}

/* Plays the announcer's voice for player's introduction (name inferred). */
void ShowAction::PlayVoice(int player) {
    if (lbl_8057D6A0.field_0x130 == 0) {
        switch (player) {
        case 0:
            fn_801D51A0(fn_80053164(), 0x1B, -1, -1);
            break;
        case 1:
            fn_801D51A0(fn_80053164(), 0x1D, -1, -1);
            break;
        }
    } else if (lbl_8057D6A0.m_winner == player) {
        fn_801D51A0(fn_80053164(), 0x1E, -1, -1);
    } else {
        fn_801D51A0(fn_80053164(), 0x20, -1, -1);
    }
}

} // namespace StageAction
