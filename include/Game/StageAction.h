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

STAGEACTION_DECLARE_EMPTY(InitAction);
STAGEACTION_DECLARE_EMPTY(PlayAction);
STAGEACTION_DECLARE(EventAction, 0x8);
STAGEACTION_DECLARE(GoalAction, 0x20);
STAGEACTION_DECLARE_EMPTY(PauseAction);
STAGEACTION_DECLARE(RestartAction, 0xC);
STAGEACTION_DECLARE(ContinueAction, 0xC);
STAGEACTION_DECLARE(DeadAction, 0xC);
STAGEACTION_DECLARE(SaveAction, 0x10);
STAGEACTION_DECLARE(TryAgainAction, 0xC);
STAGEACTION_DECLARE(ShowAction, 0x18);
STAGEACTION_DECLARE(FlashbackAction, 0xC);
STAGEACTION_DECLARE(Result2PAction, 0x3C);
STAGEACTION_DECLARE(TimeupAction, 0xC);
STAGEACTION_DECLARE(EndAction, 0xC);

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
