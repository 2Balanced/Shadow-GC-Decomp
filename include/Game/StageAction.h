#ifndef GAME_STAGEACTION_H
#define GAME_STAGEACTION_H

#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* StageAction::IAction and the stage actions (class names from RTTI; IAction's vtable is at
 * 0x80543CDC). The stage action manager (Game/StageActionManager.cpp) holds the current one in a
 * boost::shared_ptr<IAction> and creates each through `new`, so the sizes below are the ones the
 * game allocates. Constructors and destructors live in the actions' own units; the destructors are
 * not virtual (boost::checked_delete calls each one directly). Member names are inferred. */
class Task;

namespace StageAction {

class IAction {
  public:
    /* Runs once per frame (name inferred; Game/StageInitalizeTask.cpp emits the out-of-line copy,
     * 0x8017458C, which the actions' vtables use). */
    virtual void Execute() {}
    /* Whether the action has finished (name inferred; Game/ShootingGameManager.cpp emits the copy
     * at 0x8013D37C). */
    virtual bool IsDone() {
        return false;
    }
};

#define STAGEACTION_DECLARE(Name, Size)                                                            \
    class Name : public IAction {                                                                  \
      public:                                                                                      \
        Name();                                                                                    \
        ~Name();                                                                                   \
                                                                                                   \
        u8 field_0x4[Size - 0x4];                                                                  \
    };                                                                                             \
    SIZE_ASSERT(Name, Size)

#define STAGEACTION_DECLARE_EMPTY(Name)                                                            \
    class Name : public IAction {                                                                  \
      public:                                                                                      \
        Name();                                                                                    \
        ~Name();                                                                                   \
    };                                                                                             \
    SIZE_ASSERT(Name, 0x4)

/* Game/InitAction.cpp. */
class InitAction : public IAction {
  public:
    InitAction();
    ~InitAction();
    virtual void Execute();
};
SIZE_ASSERT(InitAction, 0x4);
/* Game/PlayAction.cpp. */
class PlayAction : public IAction {
  public:
    PlayAction();
    ~PlayAction();
    virtual void Execute();
};
SIZE_ASSERT(PlayAction, 0x4);
/* Game/EventAction.cpp. */
class EventAction : public IAction {
  public:
    EventAction();
    ~EventAction();
    virtual void Execute();

    /* 0x4 */ u8 m_hudVisible; /* the game state's flag before the event (meaning inferred) */
};
SIZE_ASSERT(EventAction, 0x8);
/* Game/GoalAction.cpp. */
class GoalAction : public IAction {
  public:
    GoalAction();
    ~GoalAction();
    virtual void Execute();

    /* Starts the goal: fades to white and finds the mission cleared (name inferred). */
    void Begin();
    /* Records the result in the save data (name inferred). */
    void SaveResult();

    /* 0x04 */ s32 m_timer;
    /* 0x08 */ s32 m_prevTimer; /* m_timer at the end of the last Execute */
    /* 0x0C */ s32 m_frames;
    /* 0x10 */ s32 m_result;    /* the mission cleared (0 to 2) */
    /* 0x14 */ u8 m_savedD1;    /* the game state's byte 0xD1 before the goal (meaning unknown) */
    /* 0x18 */ s32 m_voice;     /* the clear voice, or -1 */
    /* 0x1C */ s32 m_state;
};
SIZE_ASSERT(GoalAction, 0x20);
STAGEACTION_DECLARE_EMPTY(PauseAction);
/* Game/RestartAction.cpp. */
class RestartAction : public IAction {
  public:
    RestartAction();
    ~RestartAction();
    virtual void Execute();
    virtual bool IsDone();

    /* 0x4 */ s32 m_state;
    /* 0x8 */ s32 m_timer; /* frames left (meaning inferred) */
};
SIZE_ASSERT(RestartAction, 0xC);
/* Game/ContinueAction.cpp. */
class ContinueAction : public IAction {
  public:
    ContinueAction();
    ~ContinueAction();
    virtual void Execute();
    virtual bool IsDone();

    /* 0x4 */ s32 m_state;
    /* 0x8 */ s32 m_timer; /* frames left (meaning inferred) */
};
SIZE_ASSERT(ContinueAction, 0xC);
/* Game/DeadAction.cpp. */
class DeadAction : public IAction {
  public:
    DeadAction();
    ~DeadAction();
    virtual void Execute();

    /* 0x4 */ s32 m_state;
    /* 0x8 */ s32 m_timer; /* frames left (meaning inferred) */
};
SIZE_ASSERT(DeadAction, 0xC);
/* Saves the game through a SaveCycle::CycleTask (Game/SaveAction.cpp). */
class SaveAction : public IAction {
  public:
    SaveAction();
    ~SaveAction();
    virtual void Execute();

    /* 0x4 */ Task *m_task; /* a SaveCycle::CycleTask */
    /* 0x8 */ s32 m_state;
    /* 0xC */ u8 m_started;
};
SIZE_ASSERT(SaveAction, 0x10);
/* The try-again question after a game over (Game/TryAgainAction.cpp). */
class TryAgainAction : public IAction {
  public:
    TryAgainAction();
    ~TryAgainAction();
    virtual void Execute();

    /* 0x4 */ s32 m_state;
    /* 0x8 */ u8 m_retry; /* the selected answer: 1 try again, 0 quit */
};
SIZE_ASSERT(TryAgainAction, 0xC);
STAGEACTION_DECLARE(ShowAction, 0x18);
STAGEACTION_DECLARE(FlashbackAction, 0xC);
STAGEACTION_DECLARE(Result2PAction, 0x3C);
STAGEACTION_DECLARE(TimeupAction, 0xC);
/* Game/EndAction.cpp. */
class EndAction : public IAction {
  public:
    EndAction();
    ~EndAction();
    virtual void Execute();

    /* 0x4 */ u8 m_held;   /* the end was held up at least once (meaning inferred) */
    /* 0x8 */ s32 m_timer; /* frames left (meaning inferred) */
};
SIZE_ASSERT(EndAction, 0xC);

#undef STAGEACTION_DECLARE
#undef STAGEACTION_DECLARE_EMPTY

} // namespace StageAction

/* The stage action manager (class name unknown; Game/StageActionManager.cpp): fn_80074470 returns
 * the game's one. A small state machine that replaces the current action whenever the state
 * changes. */
struct StageActionManager {
    StageActionManager();

    /* Sets how the stage ended (name and meaning inferred). */
    void SetResult(int result) {
        field_0x10 = result;
    }
    /* True unless the current action is still running (name inferred; Game/ShootingGameManager.cpp
     * defines it). */
    bool IsActionDone();

    /* 0x00 */ boost::shared_ptr<StageAction::IAction> m_action;
    /* 0x08 */ s32 m_prevState;
    /* 0x0C */ s32 m_state;
    /* 0x10 */ s32 field_0x10;
    /* 0x14 */ u8 m_locked;
    /* 0x15 */ u8 field_0x15[2];
    /* 0x17 */ u8 m_holds[17]; /* per state: keep the state after it ran (name inferred) */
};
SIZE_ASSERT(StageActionManager, 0x28);

extern "C" {
StageActionManager *fn_80074470(void);
/* Requests a state (see Game/StageActionManager.cpp). */
void fn_80175EEC(StageActionManager *manager, int state, bool force);
}

#endif
