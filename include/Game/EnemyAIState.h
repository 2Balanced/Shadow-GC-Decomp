#ifndef GAME_ENEMYAISTATE_H
#define GAME_ENEMYAISTATE_H

/* TEnemyAIState (class name from RTTI; vtable 0x8052D758, a weak copy first emitted with
 * Game/EggPawnAI.cpp): a state of an enemy AI's state machine (EnemyBaseAI::m_stateMachine,
 * Game/EnemyAI.h). The concrete states are EnemyAIState_<AI>_<State>; each AI keeps one static
 * object per state. Method names are inferred. */

#include <dolphin/types.h>

class EnemyBaseAI;

class TEnemyAIState {
  public:
    virtual ~TEnemyAIState() {}
    /* Binds the state to ai (the states keep the AI's own type). */
    virtual void SetOwner(EnemyBaseAI *ai) = 0;
    /* Entering, running and leaving the state. */
    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;
    /* The state's name, for debugging. */
    virtual const char *GetName() {
        return "none";
    }
};

#endif
