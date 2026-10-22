#ifndef GAME_GAMESYSTEM_H
#define GAME_GAMESYSTEM_H

/* GameSystem (class name from RTTI; vtable at 0x8053D790): one of the game's top-level systems,
 * run one at a time (Advertise, WaitInitialize and StageSequence::Manager derive from it). Its
 * constructor (0x801E29A8) registers the new system as the current one. The virtual methods are
 * pure; their names and meanings are inferred from the derived classes. */

#include <dolphin/types.h>

class GameSystem {
  public:
    GameSystem();
    virtual ~GameSystem();
    /* Starts the system (true; meaning inferred). */
    virtual bool Initialize() = 0;
    /* Whether the system finished (meaning inferred). */
    virtual bool IsFinished() = 0;
    /* Shuts the system down (true; meaning inferred). */
    virtual bool Finalize() = 0;
    /* Asks the system to end early (meaning inferred). */
    virtual void Abort() = 0;
};

#endif
