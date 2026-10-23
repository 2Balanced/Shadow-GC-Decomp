/* BkWormAI (class name from RTTI): the Black Arms worm's AI (Game/BkWorm.h), an EnemyBaseAI
 * (Game/EnemyAI.h). Underground, the worm waits for the target to come into its area
 * (WaitAppear), comes out (Appear), waits (WaitStanding), fires its shots (Attack, AfterAttack) and
 * burrows to a new point (Move). File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
/* See EnemyBaseAI (Game/EnemyAI.h) and EnemyStatFunc (Game/Enemy.h). */
#define ENEMY_BASE_AI_HELPERS
#define ENEMY_STAT_0x90_ARG

#include "Game/BkWorm.h"
#include "Game/EnemyAIState.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"

extern "C" {
/* Stops and restarts the state machine's updates (they clear and set its byte at 0xC; behaviour
 * inferred). */
void fn_8019F9F0(UnkAIStateMachine_8019F944 *machine);
void fn_8019F9FC(UnkAIStateMachine_8019F944 *machine);
/* A random point within range of the enemy's home (behaviour inferred). */
void fn_8019B000(Enemy *enemy, Vec3f *point, float range);
/* The angle from the enemy to position (behaviour inferred). */
float fn_8019B1D8(Enemy *enemy, const Vec3f *position);
/* Vector routines that return their result (they store it through their first argument): a + b,
 * v turned about the y axis. */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
/* v scaled to length 1, v * scale, in place. */
float fn_8040CE74(Vec3f *v);
void fn_8040CE38(Vec3f *v, float scale);
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* The display's default for slot 0x4C (Game/Enemy.h; inline, first emitted here). */
inline void EnemyDispFunc::vfDisp_0x4C() {}

/* The worm's states (class names from RTTI; vtables 0x8052EADC to 0x8052EBB8). */
class EnemyAIState_BkWormAI_Move : public TEnemyAIState {
  public:
    EnemyAIState_BkWormAI_Move();
    virtual ~EnemyAIState_BkWormAI_Move() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWormAI *m_ai;
};

class EnemyAIState_BkWormAI_AfterAttack : public TEnemyAIState {
  public:
    EnemyAIState_BkWormAI_AfterAttack();
    virtual ~EnemyAIState_BkWormAI_AfterAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWormAI *m_ai;
};

class EnemyAIState_BkWormAI_Attack : public TEnemyAIState {
  public:
    EnemyAIState_BkWormAI_Attack();
    virtual ~EnemyAIState_BkWormAI_Attack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWormAI *m_ai;
};

class EnemyAIState_BkWormAI_WaitStanding : public TEnemyAIState {
  public:
    EnemyAIState_BkWormAI_WaitStanding();
    virtual ~EnemyAIState_BkWormAI_WaitStanding() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWormAI *m_ai;
};

class EnemyAIState_BkWormAI_Appear : public TEnemyAIState {
  public:
    EnemyAIState_BkWormAI_Appear();
    virtual ~EnemyAIState_BkWormAI_Appear() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWormAI *m_ai;
};

class EnemyAIState_BkWormAI_WaitAppear : public TEnemyAIState {
  public:
    EnemyAIState_BkWormAI_WaitAppear();
    virtual ~EnemyAIState_BkWormAI_WaitAppear() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWormAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetWaitAppearState() {
    static EnemyAIState_BkWormAI_WaitAppear s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAppearState() {
    static EnemyAIState_BkWormAI_Appear s_state;
    return &s_state;
}

static inline TEnemyAIState *GetWaitStandingState() {
    static EnemyAIState_BkWormAI_WaitStanding s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAttackState() {
    static EnemyAIState_BkWormAI_Attack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAfterAttackState() {
    static EnemyAIState_BkWormAI_AfterAttack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetMoveState() {
    static EnemyAIState_BkWormAI_Move s_state;
    return &s_state;
}

EnemyAIState_BkWormAI_WaitAppear::EnemyAIState_BkWormAI_WaitAppear() : m_ai(0) {}

void EnemyAIState_BkWormAI_WaitAppear::SetOwner(EnemyBaseAI *ai) {
    BkWormAI *owner = dynamic_cast<BkWormAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkWormAI_Appear::EnemyAIState_BkWormAI_Appear() : m_ai(0) {}

void EnemyAIState_BkWormAI_Appear::SetOwner(EnemyBaseAI *ai) {
    BkWormAI *owner = dynamic_cast<BkWormAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkWormAI_WaitStanding::EnemyAIState_BkWormAI_WaitStanding() : m_ai(0) {}

void EnemyAIState_BkWormAI_WaitStanding::SetOwner(EnemyBaseAI *ai) {
    BkWormAI *owner = dynamic_cast<BkWormAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkWormAI_Attack::EnemyAIState_BkWormAI_Attack() : m_ai(0) {}

void EnemyAIState_BkWormAI_Attack::SetOwner(EnemyBaseAI *ai) {
    BkWormAI *owner = dynamic_cast<BkWormAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkWormAI_AfterAttack::EnemyAIState_BkWormAI_AfterAttack() : m_ai(0) {}

void EnemyAIState_BkWormAI_AfterAttack::SetOwner(EnemyBaseAI *ai) {
    BkWormAI *owner = dynamic_cast<BkWormAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkWormAI_Move::EnemyAIState_BkWormAI_Move() : m_ai(0) {}

void EnemyAIState_BkWormAI_Move::SetOwner(EnemyBaseAI *ai) {
    BkWormAI *owner = dynamic_cast<BkWormAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

BkWormAI::BkWormAI() {
    m_worm = dynamic_cast<BkWormBase *>(m_enemy->m_disp);
    m_underground = 1;
}

void BkWormAI::vfAI_0x38() {
    m_enemy->m_status->vfStat_0x20(2, true);
    ChangeState(GetWaitAppearState());
}

void BkWormAI::vfAI_0x3C() {
    if (m_underground) {
        ChangeState(GetWaitAppearState());
    } else {
        ChangeState(GetWaitStandingState());
    }
}

void BkWormAI::vfAI_0x58() {
    m_enemy->m_disp->vfDisp_0x4C();
    m_enemy->m_set->vfSet_0x10();
}

void BkWormAI::GetMovePoint(Vec3f *point) {
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_enemy->field_0x48,
                                                                         m_enemy->GetPosition()));
    offset.y = 0.0f;
    float limit = fn_8018346C(m_enemy) - 50.0f;
    if (fn_8040CF50(&offset) < limit) {
        fn_800091D0(point, &(const Vec3f &)fn_8040CDD0(
                               m_enemy->GetPosition(),
                               &(const Vec3f &)fn_8040D0AC(
                                   &(const Vec3f &)Vec3f(0.0f, 0.0f, 50.0f),
                                   fn_8040EC80(0.0f, 360.0f * (lbl_805FAD38 / 180.0f)))));
    } else {
        fn_8040CE74(&offset);
        fn_8040CE38(&offset, 50.0f);
        float half = 45.0f * (lbl_805FAD38 / 180.0f);
        fn_800091D0(point, &(const Vec3f &)fn_8040CDD0(
                               m_enemy->GetPosition(),
                               &(const Vec3f &)fn_8040D0AC(
                                   &offset,
                                   fn_8040EC80(0.0f, 90.0f * (lbl_805FAD38 / 180.0f)) - half)));
    }
}

void EnemyAIState_BkWormAI_WaitAppear::Enter() {
    m_ai->SetAppeared(false);
    m_ai->m_underground = 1;
    m_ai->SetTimer(m_ai->m_worm->GetInterval());
    m_ai->m_step = 0;
}

void EnemyAIState_BkWormAI_WaitAppear::Update() {
    switch (m_ai->m_step) {
    case 0: {
        void *key = m_ai->vfCtrl_0x10();
        if (m_ai->m_enemy->field_0xA8.Contains(key) && m_ai->IsTimeUp()) {
            m_ai->m_worm->StartEffect(m_ai->m_enemy->GetPosition(), 1);
            m_ai->SetTimer(1.0f);
            m_ai->m_step++;
        }
        break;
    }
    case 1:
        if (m_ai->IsTimeUp()) {
            m_ai->ChangeState(GetAppearState());
        }
        break;
    }
}

void EnemyAIState_BkWormAI_WaitAppear::Exit() {}

void EnemyAIState_BkWormAI_Appear::Enter() {
    m_ai->SetAppeared(true);
    u8 *object = reinterpret_cast<u8 *>(m_ai->m_enemy->m_disp->vfDisp_0x1C());
    object[0xC] = true;
    m_ai->m_underground = 0;
    m_ai->m_enemy->m_status->vfStat_0x20(1, true);
    m_ai->m_enemy->GetRotation()->y =
        -fn_8019B1D8(m_ai->m_enemy, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
}

void EnemyAIState_BkWormAI_Appear::Update() {
    fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        m_ai->ChangeState(GetWaitStandingState());
    }
}

void EnemyAIState_BkWormAI_Appear::Exit() {}

void EnemyAIState_BkWormAI_WaitStanding::Enter() {
    m_ai->SetTimer(fn_8040EC80(1.0f, 3.0f));
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
}

void EnemyAIState_BkWormAI_WaitStanding::Update() {
    fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    if (m_ai->IsTimeUp()) {
        m_ai->m_shots = m_ai->m_worm->m_params.AttackTimes;
        m_ai->ChangeState(GetAttackState());
    }
}

void EnemyAIState_BkWormAI_WaitStanding::Exit() {}

void EnemyAIState_BkWormAI_Attack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(3, true);
}

void EnemyAIState_BkWormAI_Attack::Update() {
    fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        m_ai->ChangeState(GetAfterAttackState());
    }
}

void EnemyAIState_BkWormAI_Attack::Exit() {}

void EnemyAIState_BkWormAI_AfterAttack::Enter() {
    m_ai->SetTimer(fn_8040EC80(0.8f, 1.2f));
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
}

void EnemyAIState_BkWormAI_AfterAttack::Update() {
    fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    if (m_ai->IsTimeUp()) {
        if (--m_ai->m_shots < 1) {
            m_ai->ChangeState(GetMoveState());
        } else {
            m_ai->ChangeState(GetAttackState());
        }
    }
}

void EnemyAIState_BkWormAI_AfterAttack::Exit() {}

void EnemyAIState_BkWormAI_Move::Enter() {
    fn_8019F9FC(&m_ai->m_stateMachine);
    m_ai->SetTimer(fn_8040EC80(0.8f, 1.2f));
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
    Vec3f point;
    m_ai->GetMovePoint(&point);
    fn_8017DE10(m_ai->m_enemy, &point);
    m_ai->m_step = 0;
}

void EnemyAIState_BkWormAI_Move::Update() {
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                &m_ai->m_enemy->field_0x6C, m_ai->m_enemy->GetPosition()));
    switch (m_ai->m_step) {
    case 0:
        if (fn_8019F53C(&m_ai->m_turn, &direction)) {
            m_ai->m_enemy->m_status->vfStat_0x20(5, true);
            m_ai->m_worm->StartEffect(&m_ai->m_enemy->field_0x6C, 0);
            m_ai->m_enemy->m_status->vfStat_0x7C(true, false);
            m_ai->m_enemy->m_status->vfStat_0x90(false);
            m_ai->m_underground = 1;
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            Vec3f point;
            fn_8019B000(m_ai->m_enemy, &point, -1.0f);
            fn_8017DE10(m_ai->m_enemy, &point);
            AssignVector(m_ai->m_enemy->GetPosition(), &m_ai->m_enemy->field_0x6C);
            m_ai->m_worm->StopEffect(0);
            fn_8019F9F0(&m_ai->m_stateMachine);
            m_ai->ChangeState(GetWaitAppearState());
        }
        break;
    }
}

void EnemyAIState_BkWormAI_Move::Exit() {
    fn_8019F9F0(&m_ai->m_stateMachine);
}
