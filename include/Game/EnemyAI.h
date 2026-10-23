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

/* The state machine of an EnemyBaseAI (class name unknown; code around 0x8019F944). */
struct UnkAIStateMachine_8019F944 {
    /* 0x00 */ u8 field_0x0[0x10];
};

/* What an EnemyBaseAI tracks (class name unknown; code around 0x8019F65C). */
struct UnkAITarget_8019F65C {
    /* 0x00 */ u8 field_0x0[0x34];
};

extern "C" {
/* Switches the machine to state (behaviour inferred). */
void fn_8019F944(UnkAIStateMachine_8019F944 *machine, TEnemyAIState *state);
/* The tracked target's id and distance (behaviour inferred). */
int fn_8019F65C(UnkAITarget_8019F65C *target);
float fn_8019F710(UnkAITarget_8019F65C *target);
}

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
    virtual void vfAI_0x28();    /* 0x8019DC9C */
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
#endif

    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ s32 m_step; /* the current state's step (name inferred) */
    /* 0x28 */ UnkAIStateMachine_8019F944 m_stateMachine;
    /* 0x38 */ UnkAITarget_8019F65C m_target;
    /* 0x6C */ float field_0x6C; /* a timer the states count down (inferred) */
    /* 0x70 */ float field_0x70;
};

/* SoldierCommonAI (class name from RTTI; vtable 0x8052DC38): the soldiers' AI. */
class SoldierCommonAI : public EnemyBaseAI {
  public:
    SoldierCommonAI();
    virtual ~SoldierCommonAI() {} /* weak, first at 0x8017EE3C */

    /* 0x74 */ u8 field_0x74[0x7C - 0x74];
};

/* BkSoldierAI (class name from RTTI; vtable 0x8052DBD4; constructor 0x80180698): the Black Arms
 * soldier's AI. */
class BkSoldierAI : public SoldierCommonAI {
  public:
    BkSoldierAI();
    virtual ~BkSoldierAI() {} /* weak, first at 0x8017EDE4 */
    virtual void vfAI_0x3C(); /* 0x801803BC */
    virtual void vfAI_0x44(); /* 0x80180384 */

    /* 0x7C */ u8 field_0x7C[0x84 - 0x7C];
};

#endif
