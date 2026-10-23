#ifndef GAME_ENEMYWALKERAI_H
#define GAME_ENEMYWALKERAI_H

/* EnemyWalkerAI (class name from RTTI; vtable 0x8053A284): the AI of the walking enemies
 * (EggPawnAI, GunRobotAI; their bodies are EnemyWalkers, Game/EnemyWalker.h), with the states
 * they share. Its code and the states' are in its own unit (states around 0x80199968 and 0x8019A660);
 * the states' destructors are inline (weak copies first emitted with Game/EggPawnAI.cpp). Each
 * state has one static object, which the inline getters below construct on first use. State
 * names are from RTTI; the other names are inferred. */

#include "Game/EnemyAI.h"
#include <dolphin/types.h>
#include "size_assert.h"

class EnemyWalkerAI;

/* The game's vector assignment, out of line (not original: Vec3f has it inline). */
inline void AssignVector(Vec3f *to, const Vec3f *from) {
    fn_800091D0(to, from);
}

extern "C" {
/* Sets enemy's target position (Enemy::field_0x6C; inline, first emitted with
 * Game/EggPawnAI.cpp; name kept as other units call it). */
inline void fn_8017DE10(Enemy *enemy, const Vec3f *target) {
    AssignVector(&enemy->field_0x6C, target);
}
}

/* The states, behind the same macro as EnemyBaseAI's (Game/EnemyAI.h): their statics come out in
 * .sbss and .bss of every unit that sees them, so only the AI units define it. */
#ifdef ENEMY_BASE_AI_STATES
#include "Game/EnemyAIState.h"

/* The walking enemies' states (class names from RTTI): returning home (walking, dashing),
 * waiting at a point (standing, moving between the points) and caution. */
class EnemyAIState_EnemyWalkerAI_ReturnHome : public TEnemyAIState {
  public:
    EnemyAIState_EnemyWalkerAI_ReturnHome(); /* 0x8019A86C */
    virtual ~EnemyAIState_EnemyWalkerAI_ReturnHome() {}
    virtual void SetOwner(EnemyBaseAI *ai); /* 0x8019A820 */
    virtual void Enter();                   /* 0x8019A41C */
    virtual void Update();                  /* 0x8019A3C0 */
    virtual void Exit();                    /* 0x8019A3BC */

    /* 0x4 */ EnemyWalkerAI *m_ai;
};

class EnemyAIState_EnemyWalkerAI_ReturnHomeDash : public TEnemyAIState {
  public:
    EnemyAIState_EnemyWalkerAI_ReturnHomeDash(); /* 0x8019A7FC */
    virtual ~EnemyAIState_EnemyWalkerAI_ReturnHomeDash() {}
    virtual void SetOwner(EnemyBaseAI *ai); /* 0x8019A7B0 */
    virtual void Enter();                   /* 0x8019A340 */
    virtual void Update();                  /* 0x8019A2E4 */
    virtual void Exit();                    /* 0x8019A2E0 */

    /* 0x4 */ EnemyWalkerAI *m_ai;
};

class EnemyAIState_EnemyWalkerAI_WaitAction_Standing : public TEnemyAIState {
  public:
    EnemyAIState_EnemyWalkerAI_WaitAction_Standing(); /* 0x8019A78C */
    virtual ~EnemyAIState_EnemyWalkerAI_WaitAction_Standing() {}
    virtual void SetOwner(EnemyBaseAI *ai); /* 0x8019A740 */
    virtual void Enter();                   /* 0x8019A250 */
    virtual void Update();                  /* 0x80199F44 */
    virtual void Exit();                    /* 0x80199F40 */

    /* 0x4 */ EnemyWalkerAI *m_ai;
};

class EnemyAIState_EnemyWalkerAI_WaitAction_Moving : public TEnemyAIState {
  public:
    EnemyAIState_EnemyWalkerAI_WaitAction_Moving(); /* 0x8019A71C */
    virtual ~EnemyAIState_EnemyWalkerAI_WaitAction_Moving() {}
    virtual void SetOwner(EnemyBaseAI *ai); /* 0x8019A6D0 */
    virtual void Enter();                   /* 0x80199E5C */
    virtual void Update();                  /* 0x80199AD4 */
    virtual void Exit();                    /* 0x80199AD0 */

    /* 0x4 */ EnemyWalkerAI *m_ai;
};

class EnemyAIState_EnemyWalkerAI_Caution : public TEnemyAIState {
  public:
    EnemyAIState_EnemyWalkerAI_Caution(); /* 0x8019A6AC */
    virtual ~EnemyAIState_EnemyWalkerAI_Caution() {}
    virtual void SetOwner(EnemyBaseAI *ai); /* 0x8019A660 */
    virtual void Enter();                   /* 0x80199A50 */
    virtual void Update();                  /* 0x8019996C */
    virtual void Exit();                    /* 0x80199968 */

    /* 0x4 */ EnemyWalkerAI *m_ai;
};

#endif

class EnemyWalkerAI : public EnemyBaseAI {
  public:
    EnemyWalkerAI();            /* 0x8019A5E0 */
    virtual ~EnemyWalkerAI() {} /* weak, first at 0x8019A584 */
    virtual void vfAI_0x44();   /* 0x8019A4A0 */

#ifdef ENEMY_BASE_AI_STATES
    /* The states' objects. MWCC lays out the statics of a class's inline functions in reverse
     * order, so these are declared last state first. */
    static TEnemyAIState *GetCautionState() {
        static EnemyAIState_EnemyWalkerAI_Caution s_state;
        return &s_state;
    }
    static TEnemyAIState *GetMovingState() {
        static EnemyAIState_EnemyWalkerAI_WaitAction_Moving s_state;
        return &s_state;
    }
    static TEnemyAIState *GetStandingState() {
        static EnemyAIState_EnemyWalkerAI_WaitAction_Standing s_state;
        return &s_state;
    }
    static TEnemyAIState *GetReturnHomeDashState() {
        static EnemyAIState_EnemyWalkerAI_ReturnHomeDash s_state;
        return &s_state;
    }
    static TEnemyAIState *GetReturnHomeState() {
        static EnemyAIState_EnemyWalkerAI_ReturnHome s_state;
        return &s_state;
    }
#endif

    /* 0x74 */ u8 field_0x74[0x7C - 0x74];
};

#endif
