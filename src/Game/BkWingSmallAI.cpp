/* BkWingSmallAI (class name from RTTI): the small Black Arms wing's AI (Game/BkWingSmall.h), a
 * FlyerCommonAI (Game/EnemyFlyer.h). It floats in place (FlyerCommonAI's state) or follows its
 * path until it reaches its attack stretch (MoveOnPath), cautions, then dives at its target point
 * and flies on (Attack). File name inferred from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* See FlyerCommonAI (Game/EnemyFlyer.h) and EnemyBaseAI (Game/EnemyAI.h). */
#define FLYER_COMMON_AI_STATES
#define ENEMY_BASE_AI_HELPERS

#include "Game/BkWingSmall.h"
#include "Game/EnemyAIState.h"
#include "Game/MathTypes.h"

extern "C" {
/* Turns the enemy a step toward position (behaviour inferred; see fn_8019F53C). */
bool fn_8019F2E0(UnkAITurn_8019F53C *turn, const Vec3f *position);
/* The distance from enemy to position (behaviour inferred). */
float fn_8019B24C(Enemy *enemy, const Vec3f *position);
/* Plays sound id for enemy, at most once a frame (behaviour inferred). */
void fn_8019A954(Enemy *enemy, int id, int arg);
void fn_8019AA40(Enemy *enemy, int id, int arg);
/* Vector routines that return their result (they store it through their first argument): a + b,
 * v scaled to length 1, v * scale. */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *v);
Vec3f fn_800091A4(const Vec3f *v, float scale);
/* v += value. */
void fn_8040CFF8(Vec3f *v, const Vec3f *value);
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* The parameters' default for slot 0x20 (Game/Enemy.h; inline, first emitted here). */
inline float EnemyParaFunc::vfPara_0x20() {
    return 0.0f;
}

/* The smaller of a and b (name inferred; weak copy 0x80181F18). */
inline float Min(float a, float b) {
    return a < b ? a : b;
}

/* *v += value (name inferred). Inline: the calls in its arguments are then made out of line, as
 * retail makes them. */
inline void AddVector(Vec3f *v, const Vec3f &value) {
    fn_8040CFF8(v, &value);
}

/* The small wing's states (class names from RTTI; vtables 0x8052E6FC to 0x8052E754). */
class EnemyAIState_BkWingSmallAI_MoveOnPath : public TEnemyAIState {
  public:
    EnemyAIState_BkWingSmallAI_MoveOnPath();
    virtual ~EnemyAIState_BkWingSmallAI_MoveOnPath() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWingSmallAI *m_ai;
};

class EnemyAIState_BkWingSmallAI_Attack : public TEnemyAIState {
  public:
    EnemyAIState_BkWingSmallAI_Attack();
    virtual ~EnemyAIState_BkWingSmallAI_Attack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWingSmallAI *m_ai;
};

class EnemyAIState_BkWingSmallAI_Caution : public TEnemyAIState {
  public:
    EnemyAIState_BkWingSmallAI_Caution();
    virtual ~EnemyAIState_BkWingSmallAI_Caution() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWingSmallAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetCautionState() {
    static EnemyAIState_BkWingSmallAI_Caution s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAttackState() {
    static EnemyAIState_BkWingSmallAI_Attack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetMoveOnPathState() {
    static EnemyAIState_BkWingSmallAI_MoveOnPath s_state;
    return &s_state;
}

EnemyAIState_BkWingSmallAI_Caution::EnemyAIState_BkWingSmallAI_Caution() : m_ai(0) {}

void EnemyAIState_BkWingSmallAI_Caution::SetOwner(EnemyBaseAI *ai) {
    BkWingSmallAI *owner = dynamic_cast<BkWingSmallAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkWingSmallAI_Attack::EnemyAIState_BkWingSmallAI_Attack() : m_ai(0) {}

void EnemyAIState_BkWingSmallAI_Attack::SetOwner(EnemyBaseAI *ai) {
    BkWingSmallAI *owner = dynamic_cast<BkWingSmallAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkWingSmallAI_MoveOnPath::EnemyAIState_BkWingSmallAI_MoveOnPath() : m_ai(0) {}

void EnemyAIState_BkWingSmallAI_MoveOnPath::SetOwner(EnemyBaseAI *ai) {
    BkWingSmallAI *owner = dynamic_cast<BkWingSmallAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

BkWingSmallAI::BkWingSmallAI() {
    m_wing = dynamic_cast<BkWingSmallBase *>(m_enemy->m_disp);
}

void BkWingSmallAI::vfAI_0x3C() {
    if (m_wing->vf_0x238()) {
        ChangeState(::GetMoveOnPathState());
    } else {
        fn_8019F944(&m_stateMachine, GetWaitFloatingState());
    }
}

void BkWingSmallAI::vfAI_0x44() {
    ChangeState(GetCautionState());
}

void EnemyAIState_BkWingSmallAI_Caution::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_wing->vfPara_0x54(), true);
    fn_8019A954(m_ai->m_enemy, 0x603F, 1);
}

void EnemyAIState_BkWingSmallAI_Caution::Update() {
    fn_8019F2E0(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        m_ai->ChangeState(GetAttackState());
    }
}

void EnemyAIState_BkWingSmallAI_Caution::Exit() {}

void EnemyAIState_BkWingSmallAI_Attack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_wing->vfPara_0x58(), true);
    fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)fn_8040CDD0(&(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f),
                                                           &(const Vec3f &)Vec3f(0.0f, 5.0f, 0.0f)));
    fn_8019AA40(m_ai->m_enemy, 0x603D, 1);
    m_ai->SetTimer(3.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_BkWingSmallAI_Attack::Update() {
    float distance = fn_8019B24C(m_ai->m_enemy, &m_ai->m_enemy->field_0x6C);
    AssignVector(&m_ai->m_direction,
                 &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_ai->m_enemy->field_0x6C,
                                                                 m_ai->m_enemy->GetPosition()));
    switch (m_ai->m_step) {
    case 0:
        fn_8019F2E0(&m_ai->m_turn, &m_ai->m_direction);
        if (distance < 2.0f || m_ai->IsTimeUp()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_wing->vfPara_0x48(), true);
            if (m_ai->m_wing->vf_0x238()) {
                Vec3f end;
                m_ai->m_wing->GetAttackEndPoint(&end);
                fn_8017DE10(m_ai->m_enemy, &end);
            } else {
                fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
            }
            m_ai->m_step++;
        }
        AddVector(fn_80181F0C(m_ai->m_enemy),
                  fn_800091A4(&(const Vec3f &)fn_8040CEE8(&m_ai->m_direction),
                              Min(m_ai->m_wing->GetSpeed() * m_ai->GetDeltaTime(), distance)));
        break;
    case 1:
        if (fn_8019F2E0(&m_ai->m_turn, &m_ai->m_direction)) {
            m_ai->SetTimer(3.0f);
            if (m_ai->m_wing->vf_0x238()) {
                m_ai->m_step = 2;
            } else {
                m_ai->m_step = 3;
            }
        }
        break;
    case 2:
        AddVector(fn_80181F0C(m_ai->m_enemy),
                  fn_800091A4(&(const Vec3f &)fn_800091A4(
                                  &(const Vec3f &)fn_8040CEE8(&m_ai->m_direction),
                                  m_ai->m_enemy->m_para->vfPara_0x20()),
                              m_ai->GetDeltaTime()));
        break;
    case 3:
        if (distance < 2.0f || m_ai->IsTimeUp()) {
            m_ai->vfAI_0x3C();
        }
        AddVector(fn_80181F0C(m_ai->m_enemy),
                  fn_800091A4(&(const Vec3f &)fn_8040CEE8(&m_ai->m_direction),
                              Min(m_ai->m_enemy->m_para->vfPara_0x20() * m_ai->GetDeltaTime(),
                                  distance)));
        break;
    }
}

void EnemyAIState_BkWingSmallAI_Attack::Exit() {}

void EnemyAIState_BkWingSmallAI_MoveOnPath::Enter() {
    m_ai->m_step = 0;
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    m_ai->m_flyer->SetMoveRate(1.0f);
}

void EnemyAIState_BkWingSmallAI_MoveOnPath::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_flyer->IsInAttackRange()) {
            m_ai->m_step++;
            m_ai->ChangeState(GetCautionState());
        }
        break;
    }
    m_ai->m_flyer->MoveAlongPath(m_ai->GetDeltaTime());
}

void EnemyAIState_BkWingSmallAI_MoveOnPath::Exit() {}
