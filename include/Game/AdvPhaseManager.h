#ifndef GAME_ADVPHASEMANAGER_H
#define GAME_ADVPHASEMANAGER_H

#include "Game/AdvBase.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The front-end phase manager (class name unknown): the singleton fn_801E5E00 returns. It runs one
 * AdvReal phase at a time; fn_802B7528 requests the next one by id and the next Update creates it:
 * 1 Title, 2 Option, 3 and 4 StoryLibrary (mode 0 and 1), 5 Synopsis, 6 SelectMode, 7 BattleMode,
 * 8 Movie, 9 MemoryCheck, 10 Logo. Its functions are in Game/AdvFolder.cpp; the constructor is at
 * 0x802B7534. Member and function names are inferred. */
class UnkManager_801E5E00 {
  public:
    UnkManager_801E5E00();

    /* Creates the requested phase if it changed (0x802B6470). */
    void ChangePhase();
    /* Changes phase if needed and runs it; the phase's result, or false without one (0x802B74D8). */
    bool Update();
    /* Draws the phase; its result, or false without one (0x802B7498). */
    bool Draw();

    /* 0x00 */ s32 m_request;
    /* 0x04 */ s32 m_current;
    /* 0x08 */ boost::shared_ptr<AdvReal::AdvBase> m_phase;
};
SIZE_ASSERT(UnkManager_801E5E00, 0x10);

extern "C" {
/* The phase manager (creating it on first use). */
UnkManager_801E5E00 *fn_801E5E00(void);
/* Requests phase id; always returns true. */
bool fn_802B7528(UnkManager_801E5E00 *manager, s32 id);
}

#endif
