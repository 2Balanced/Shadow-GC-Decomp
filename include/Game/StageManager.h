#ifndef GAME_STAGEMANAGER_H
#define GAME_STAGEMANAGER_H

/* StageManager (class name from RTTI; Game/StageManager.cpp): the stage a stage sequence phase
 * plays while the game itself runs (StageSequence::*::Phase create it through a
 * boost::shared_ptr<StageBase::Base>). It loads the stage's data archive, creates the stage's
 * tasks, runs the stage action manager until the stage ends and tears everything down again.
 * There is one at a time (s_instance). Member names are inferred. */

#include "Game/StageSequence.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class TFogSet;
class UnkHintTarget_801FB180;

class StageManager : public StageBase::Base {
  public:
    /* Plays stage stageId; replay is set for a replayed stage (meaning inferred). */
    StageManager(s32 stageId, u8 replay);
    virtual ~StageManager();
    virtual int Tick();
    virtual void vf_0x10();

    bool Start();
    bool WaitLoad();
    bool Run();
    bool Finish();
    bool LoadStageData(bool async);
    void ReleasePath();
    void SetUpBlocks();
    void ReleaseBlocks();
    static void OnStageDataLoaded(StageManager *manager);

    /* The manager playing, or 0. */
    static StageManager *s_instance;

    /* 0x08 */ s32 m_state; /* 0 start, 1 loading, 2 running, 3 ending */
    /* 0x0C */ u8 m_ended;
    /* 0x0D */ u8 field_0xD[0x14 - 0xD];
    /* 0x14 */ u8 field_0x14;
    /* 0x15 */ u8 field_0x15[0x18 - 0x15];
    /* 0x18 */ boost::shared_ptr<TFogSet> m_fog;
    /* 0x20 */ boost::shared_ptr<UnkHintTarget_801FB180> m_message; /* a TMessageDisp */
    /* 0x28 */ u8 m_loadPath; /* load the stage's PATH.PTP */
    /* 0x29 */ u8 m_replay;
};
SIZE_ASSERT(StageManager, 0x2C);

extern "C" {
/* The manager playing, or 0. */
StageManager *fn_80177D14(void);
/* Its byte at 0x14, or 0 without one (meaning unknown). */
u8 fn_80177CF8(void);
}

#endif
