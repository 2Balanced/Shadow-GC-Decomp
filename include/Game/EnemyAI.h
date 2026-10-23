#ifndef GAME_ENEMYAI_H
#define GAME_ENEMYAI_H

/* The enemies' AI (class names from RTTI): EnemyBaseAI implements EnemyCtrlFunc (see
 * Game/Enemy.h) and runs a state machine of TEnemyAIState objects (EnemyAIState_<AI>_<State>,
 * code from 0x8017EDDC). SoldierCommonAI is the soldiers' AI, BkSoldierAI the Black Arms
 * soldier's. EnemyBaseAI's code is at 0x8019CB4C..0x8019EB08. Slot names follow Game/Enemy.h. */

#include "Game/Enemy.h"
#include <dolphin/types.h>
#include "size_assert.h"

class TEnemyAIState;

/* A message an EnemyBaseAI handles in vfAI_0x28 (struct name unknown): its id picks the slot the
 * handler calls (0x201 and up: vfAI_0x34 and up), and the handler sets bit 1 of the flags once it
 * has handled it. Only seen with ENEMY_AI_MESSAGE_ARG (see EnemyBaseAI). */
struct UnkEnemyMessage_8019DC9C {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u16 m_id;
    /* 0x6 */ u8 field_0x6;
    /* 0x7 */ u8 m_flags;
};

/* The state machine of an EnemyBaseAI (class name unknown; code around 0x8019F944). */
struct UnkAIStateMachine_8019F944 {
    /* 0x00 */ u8 field_0x0[0x10];
};

/* What an EnemyBaseAI tracks (class name unknown; code around 0x8019F65C). Its accessors bring it
 * up to date while the byte at 0x8 is clear; fn_8019F65C returns the address of the vector at 0xC
 * (the target's position, inferred), fn_8019F710 the float at 0x18 and fn_8019F698 the one at
 * 0x20 (an angle, as the AI states compare its absolute value with 30 degrees). */
struct UnkAITarget_8019F65C {
    /* 0x00 */ u8 field_0x0[0x28];
};

/* What turns an EnemyBaseAI's enemy (class name unknown; fn_8019F53C): the enemy at 0x0. */
struct UnkAITurn_8019F53C {
    /* 0x00 */ u8 field_0x0[0xC];
};

extern "C" {
/* Switches the machine to state (behaviour inferred). */
void fn_8019F944(UnkAIStateMachine_8019F944 *machine, TEnemyAIState *state);
/* The tracked target's id and distance (behaviour inferred). */
int fn_8019F65C(UnkAITarget_8019F65C *target);
float fn_8019F710(UnkAITarget_8019F65C *target);
/* The tracked target's angle (behaviour inferred). */
float fn_8019F698(UnkAITarget_8019F65C *target);
/* Turns the enemy a step toward direction; true once it faces it (behaviour inferred). */
bool fn_8019F53C(UnkAITurn_8019F53C *turn, const Vec3f *direction);
}

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

#if defined(ENEMY_BASE_AI_STATES) || defined(ENEMY_BASE_AI_HELPERS)
extern "C" {
/* The vector at 0x9C of the enemy's character info (EnemyCharInfo::field_0x9C: the movement of
 * the frame, which the AI adds to) and the radius of its area (inline; weak copies first at
 * 0x80181F0C and 0x8018346C; Game/Enemy.cpp and Game/FlyerCommonBase.cpp call them by these
 * names). */
inline Vec3f *fn_80181F0C(Enemy *enemy) {
    return reinterpret_cast<Vec3f *>(reinterpret_cast<u8 *>(enemy->m_chara.get()) + 0x9C);
}
inline float fn_8018346C(Enemy *enemy) {
    return enemy->field_0xA8.GetRange();
}
}
#endif

#ifdef ENEMY_BASE_AI_STATES
#include "Game/EnemyAIState.h"

/* EnemyBaseAI's own states (class names from RTTI; constructors in its unit, around 0x8019FA18).
 * Only their objects matter here (see EnemyBaseAI below); the methods' addresses are unknown. */
class EnemyAIState_EnemyBaseAI_None : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_None(); /* 0x8019FC54 */
    virtual ~EnemyAIState_EnemyBaseAI_None() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

class EnemyAIState_EnemyBaseAI_Idle : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_Idle(); /* 0x8019FC20 */
    virtual ~EnemyAIState_EnemyBaseAI_Idle() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

class EnemyAIState_EnemyBaseAI_CaptureWait : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_CaptureWait(); /* 0x8019FBEC */
    virtual ~EnemyAIState_EnemyBaseAI_CaptureWait() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

class EnemyAIState_EnemyBaseAI_WarpWait : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_WarpWait(); /* 0x8019FBB8 */
    virtual ~EnemyAIState_EnemyBaseAI_WarpWait() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

class EnemyAIState_EnemyBaseAI_Wait : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_Wait(); /* 0x8019FB84 */
    virtual ~EnemyAIState_EnemyBaseAI_Wait() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

class EnemyAIState_EnemyBaseAI_Guard : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_Guard(); /* 0x8019FB50 */
    virtual ~EnemyAIState_EnemyBaseAI_Guard() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

class EnemyAIState_EnemyBaseAI_Caution : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_Caution(); /* 0x8019FB1C */
    virtual ~EnemyAIState_EnemyBaseAI_Caution() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

class EnemyAIState_EnemyBaseAI_GoHome : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_GoHome(); /* 0x8019FAE8 */
    virtual ~EnemyAIState_EnemyBaseAI_GoHome() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

class EnemyAIState_EnemyBaseAI_WaitFinishDamaged : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_WaitFinishDamaged(); /* 0x8019FAB4 */
    virtual ~EnemyAIState_EnemyBaseAI_WaitFinishDamaged() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

class EnemyAIState_EnemyBaseAI_WaitFinishFalling : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_WaitFinishFalling(); /* 0x8019FA80 */
    virtual ~EnemyAIState_EnemyBaseAI_WaitFinishFalling() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

class EnemyAIState_EnemyBaseAI_Dead : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_Dead(); /* 0x8019FA4C */
    virtual ~EnemyAIState_EnemyBaseAI_Dead() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

class EnemyAIState_EnemyBaseAI_Recover : public TEnemyAIState {
  public:
    EnemyAIState_EnemyBaseAI_Recover(); /* 0x8019FA18 */
    virtual ~EnemyAIState_EnemyBaseAI_Recover() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EnemyBaseAI *m_ai;
};

#endif

/* EnemyBaseAI (class name from RTTI; vtable 0x8053A5D0). Its data starts after the pointer to
 * the EnemyReferer at 0x1C. */
class EnemyBaseAI : public virtual EnemyReferer, public EnemyCtrlFunc {
  public:
    EnemyBaseAI();

    virtual void vfCtrl_0x08();  /* 0x8019EB08 */
    virtual void vfCtrl_0x0C();  /* 0x8019E710 */
    virtual void *vfCtrl_0x10(); /* 0x8019DBC0 */
    virtual void *vfCtrl_0x14(); /* 0x8019DAF0 */
    virtual float vfCtrl_0x18(); /* 0x8019DA0C */
    virtual Vec3f vfCtrl_0x1C(float arg); /* 0x8019D660 */
    virtual void vfCtrl_0x20();  /* 0x8017B50C */
    virtual ~EnemyBaseAI() {}    /* weak, first at 0x80183614 */
#ifdef ENEMY_AI_MESSAGE_ARG
    /* The message handler takes the message (0x8019DC9C reads it from r4). Behind a macro because
     * the symbols of this slot carry the argument-less signature; only units that need the
     * argument define it. */
    virtual void vfAI_0x28(UnkEnemyMessage_8019DC9C *message); /* 0x8019DC9C */
#else
    virtual void vfAI_0x28();    /* 0x8019DC9C */
#endif
    virtual void vfAI_0x2C();    /* 0x8019E798 */
    virtual void vfAI_0x30();    /* 0x8019E794 */
    virtual void vfAI_0x34();    /* 0x8019D504 */
    virtual void vfAI_0x38();    /* 0x8019D4D8 */
    virtual void vfAI_0x3C();    /* 0x8019D428 */
    virtual void vfAI_0x40();    /* 0x8019D378 */
    virtual void vfAI_0x44();    /* 0x8019D278 */
    virtual void vfAI_0x48();    /* 0x8019D1C8 */
    virtual void vfAI_0x4C();    /* 0x8019D0C8 */
    virtual void vfAI_0x50();    /* 0x8019CF6C */
    virtual void vfAI_0x54();    /* 0x8019CE10 */
    virtual void vfAI_0x58();    /* 0x8019CCB4 */
    virtual void vfAI_0x5C();    /* 0x8019CCB0 */
    virtual void vfAI_0x60();    /* 0x8019CB4C */

#ifdef ENEMY_BASE_AI_STATES
    /* The states' objects (names inferred). Their statics come out in .sbss and .bss with every
     * unit that sees them (MWCC lays a class's out last first, hence the order), so they are
     * behind a macro: units that define it are the AI units, whose sections hold them. */
    static TEnemyAIState *GetRecoverState() {
        static EnemyAIState_EnemyBaseAI_Recover s_state;
        return &s_state;
    }
    static TEnemyAIState *GetDeadState() {
        static EnemyAIState_EnemyBaseAI_Dead s_state;
        return &s_state;
    }
    static TEnemyAIState *GetWaitFinishFallingState() {
        static EnemyAIState_EnemyBaseAI_WaitFinishFalling s_state;
        return &s_state;
    }
    static TEnemyAIState *GetWaitFinishDamagedState() {
        static EnemyAIState_EnemyBaseAI_WaitFinishDamaged s_state;
        return &s_state;
    }
    static TEnemyAIState *GetGoHomeState() {
        static EnemyAIState_EnemyBaseAI_GoHome s_state;
        return &s_state;
    }
    static TEnemyAIState *GetCautionState() {
        static EnemyAIState_EnemyBaseAI_Caution s_state;
        return &s_state;
    }
    static TEnemyAIState *GetGuardState() {
        static EnemyAIState_EnemyBaseAI_Guard s_state;
        return &s_state;
    }
    static TEnemyAIState *GetWaitState() {
        static EnemyAIState_EnemyBaseAI_Wait s_state;
        return &s_state;
    }
    static TEnemyAIState *GetWarpWaitState() {
        static EnemyAIState_EnemyBaseAI_WarpWait s_state;
        return &s_state;
    }
    static TEnemyAIState *GetCaptureWaitState() {
        static EnemyAIState_EnemyBaseAI_CaptureWait s_state;
        return &s_state;
    }
    static TEnemyAIState *GetIdleState() {
        static EnemyAIState_EnemyBaseAI_Idle s_state;
        return &s_state;
    }
    static TEnemyAIState *GetNoneState() {
        static EnemyAIState_EnemyBaseAI_None s_state;
        return &s_state;
    }
#endif

#if defined(ENEMY_BASE_AI_STATES) || defined(ENEMY_BASE_AI_HELPERS)
    /* Units that need the helpers below but not EnemyBaseAI's states define ENEMY_BASE_AI_HELPERS. */

    /* Switches to state (name inferred). Inline: a state getter in its argument is then called out
     * of line, as retail has the AI units' own getters (see Game/BkSoldierAI.cpp). */
    void ChangeState(TEnemyAIState *state) {
        fn_8019F944(&m_stateMachine, state);
    }

    /* The states' timer (names inferred; behind the macro too: the float constant would renumber
     * other units' @N locals). */
    void SetTimer(float time) {
        field_0x6C = time;
        field_0x70 = time;
    }
    bool IsTimeUp() {
        bool up = false;
        if (field_0x6C < 0.0f) {
            up = true;
        }
        return up;
    }
    /* The frame time (name inferred; weak copy first at 0x80181F28). */
    float GetDeltaTime() {
        return field_0x20;
    }
#endif

    /* 0x20 */ float field_0x20; /* the frame time (inferred) */
    /* 0x24 */ s32 m_step; /* the current state's step (name inferred) */
    /* 0x28 */ UnkAIStateMachine_8019F944 m_stateMachine;
    /* 0x38 */ UnkAITarget_8019F65C m_target;
    /* 0x60 */ UnkAITurn_8019F53C m_turn;
    /* 0x6C */ float field_0x6C; /* a timer the states count down (inferred) */
    /* 0x70 */ float field_0x70;
};

/* The soldiers' states (class names from RTTI): waiting at a point while moving between the
 * points or standing. Behind their own macro for the reason given at ENEMY_BASE_AI_STATES: only
 * the units that use them define it. Their code is in SoldierCommonAI's unit (0x801887A4..), their
 * destructors are inline (weak copies first emitted with Game/BkSoldierAI.cpp). */
#ifdef SOLDIER_COMMON_AI_STATES
#include "Game/EnemyAIState.h"

class SoldierCommonAI;

class EnemyAIState_SoldierCommonAI_WaitAction_Moving : public TEnemyAIState {
  public:
    EnemyAIState_SoldierCommonAI_WaitAction_Moving(); /* 0x8018941C */
    virtual ~EnemyAIState_SoldierCommonAI_WaitAction_Moving() {}
    virtual void SetOwner(EnemyBaseAI *ai); /* 0x801893D0 */
    virtual void Enter();                   /* 0x80188E10 */
    virtual void Update();                  /* 0x801887E4 */
    virtual void Exit();                    /* 0x801887A4 */

    /* 0x4 */ SoldierCommonAI *m_ai;
};

class EnemyAIState_SoldierCommonAI_WaitAction_Standing : public TEnemyAIState {
  public:
    EnemyAIState_SoldierCommonAI_WaitAction_Standing(); /* 0x8018948C */
    virtual ~EnemyAIState_SoldierCommonAI_WaitAction_Standing() {}
    virtual void SetOwner(EnemyBaseAI *ai); /* 0x80189440 */
    virtual void Enter();                   /* 0x80189240 */
    virtual void Update();                  /* 0x80188F54 */
    virtual void Exit();                    /* 0x80188F50 */

    /* 0x4 */ SoldierCommonAI *m_ai;
};
#endif

class SoldierCommonBase;

/* SoldierCommonAI (class name from RTTI; vtable 0x8052DC38; constructor 0x80189350): the soldiers'
 * AI. */
class SoldierCommonAI : public EnemyBaseAI {
  public:
    SoldierCommonAI();
    virtual ~SoldierCommonAI() {} /* weak, first at 0x8017EE3C */

#ifdef SOLDIER_COMMON_AI_STATES
    /* The states' objects (names inferred), declared last state first (see EnemyWalkerAI). */
    static TEnemyAIState *GetMovingState() {
        static EnemyAIState_SoldierCommonAI_WaitAction_Moving s_state;
        return &s_state;
    }
    static TEnemyAIState *GetStandingState() {
        static EnemyAIState_SoldierCommonAI_WaitAction_Standing s_state;
        return &s_state;
    }
#endif

    /* 0x74 */ u8 field_0x74[0x78 - 0x74];
    /* 0x78 */ SoldierCommonBase *m_soldier; /* the enemy's body (name inferred) */
};

class BkSoldierBase;

/* BkSoldierAI (class name from RTTI; vtable 0x8052DBD4; constructor 0x80180698; Game/BkSoldierAI.cpp):
 * the Black Arms soldier's AI. */
class BkSoldierAI : public SoldierCommonAI {
  public:
    BkSoldierAI();
    virtual ~BkSoldierAI() {} /* weak, first at 0x8017EDE4 */
    virtual void vfAI_0x3C(); /* 0x801803BC */
    virtual void vfAI_0x44(); /* 0x80180384 */

    /* Goes over to the attack that suits the soldier's weapon (0x801801AC; name inferred). */
    void StartAttack();

    /* 0x7C */ u8 m_appeared;   /* has come out (AppearType 4 and 5; name inferred) */
    /* 0x7D */ u8 m_nearAttack; /* is attacking from close by (name inferred) */
    /* 0x80 */ BkSoldierBase *m_body;
};

#endif
