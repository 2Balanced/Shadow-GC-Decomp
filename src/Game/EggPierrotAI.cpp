/* EggPierrot::EggPierrotAI (class name from RTTI): the Egg Pierrot's AI (Game/EggPierrot.h). It
 * waits about its home (Wait) or goes back to it (GoHome); with the target in its area it juggles
 * and throws bombs at it from where it stands (BakudanAttack), and once the target is close it
 * rolls at it on its ball (DashAttack). While the ball is worn down it totters about its home
 * (Ottoto). File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
/* See EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_STATES

#include "Game/EggPierrot.h"
#include "Game/EnemyAIState.h"
#include "Game/MathTypes.h"

extern "C" {
/* Turns enemy toward direction, and to the angles at rotation (behaviours inferred; see
 * GunBigfootAI.cpp). */
void fn_8019B0B0(Enemy *enemy, const Vec3f *direction);
void fn_8019B0F8(Enemy *enemy, const Vec3f *rotation);
/* The distance from enemy to point (Game/Enemy.cpp). */
float fn_8019B298(Enemy *enemy, const Vec3f *point);
/* Whether the enemy should start moving (Game/Enemy.cpp; behaviour inferred). */
bool fn_8019AF04(Enemy *enemy);
/* A point dist ahead of enemy toward position (behaviour inferred). */
Vec3f fn_8019AADC(Enemy *enemy, const Vec3f *position, float dist);
/* Whether the machine's current state is state (behaviour inferred). */
bool fn_8019F86C(UnkAIStateMachine_8019F944 *machine, TEnemyAIState *state);
/* The blend of a and b at t, vector * scale, and vector scaled to length 1 and by scale, in place
 * (behaviours inferred). */
Vec3f fn_8040F268(const Vec3f *a, const Vec3f *b, float t);
Vec3f fn_800091A4(const Vec3f *vector, float scale);
float fn_8040CE74(Vec3f *v);
void fn_8040CE38(Vec3f *v, float scale);
void fn_8040CFF8(Vec3f *v, const Vec3f *value);
float fn_8040CF50(const Vec3f *v);
void fn_8040CE60(Vec3f *vector);

/* The velocity of the enemy's character info (CharInfo::field_0x18; inline, a weak copy emitted
 * here; name kept like fn_80181F0C's, see Game/EnemyAI.h). */
inline Vec3f *fn_8028F324(Enemy *enemy) {
    return &enemy->m_chara->field_0x18;
}
}

/* The enemy's home position (Enemy::field_0x48; name inferred). Inline: retail takes its address
 * after the call that comes before it in the arguments. */
inline const Vec3f *GetHome(Enemy *enemy) {
    return &enemy->field_0x48;
}

/* Whether machine is in state (name inferred). Inline, as EnemyBaseAI::ChangeState: the state
 * getter in its argument is then called out of line, as retail has it. */
inline bool IsInState(UnkAIStateMachine_8019F944 *machine, TEnemyAIState *state) {
    return fn_8019F86C(machine, state);
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* *v += value (name inferred; as in GunBigfootAI.cpp). Inline: the calls in its arguments are then
 * made out of line, as retail makes them. */
inline void AddVector(Vec3f *v, const Vec3f &value) {
    fn_8040CFF8(v, &value);
}

namespace EggPierrot {

/* The clown's states (class names from RTTI; vtables 0x8055233C to 0x805523EC). */
class EnemyAIState_EggPierrotAI_BakudanAttack : public TEnemyAIState {
  public:
    EnemyAIState_EggPierrotAI_BakudanAttack();
    virtual ~EnemyAIState_EggPierrotAI_BakudanAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggPierrotAI *m_ai;
};

class EnemyAIState_EggPierrotAI_DashAttack : public TEnemyAIState {
  public:
    EnemyAIState_EggPierrotAI_DashAttack();
    virtual ~EnemyAIState_EggPierrotAI_DashAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggPierrotAI *m_ai;
};

class EnemyAIState_EggPierrotAI_Ottoto : public TEnemyAIState {
  public:
    EnemyAIState_EggPierrotAI_Ottoto();
    virtual ~EnemyAIState_EggPierrotAI_Ottoto() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggPierrotAI *m_ai;
};

class EnemyAIState_EggPierrotAI_GoHome : public TEnemyAIState {
  public:
    EnemyAIState_EggPierrotAI_GoHome();
    virtual ~EnemyAIState_EggPierrotAI_GoHome() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggPierrotAI *m_ai;
};

class EnemyAIState_EggPierrotAI_Wait : public TEnemyAIState {
  public:
    EnemyAIState_EggPierrotAI_Wait();
    virtual ~EnemyAIState_EggPierrotAI_Wait() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggPierrotAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetWaitState() {
    static EnemyAIState_EggPierrotAI_Wait s_state;
    return &s_state;
}

static inline TEnemyAIState *GetGoHomeState() {
    static EnemyAIState_EggPierrotAI_GoHome s_state;
    return &s_state;
}

static inline TEnemyAIState *GetOttotoState() {
    static EnemyAIState_EggPierrotAI_Ottoto s_state;
    return &s_state;
}

static inline TEnemyAIState *GetDashAttackState() {
    static EnemyAIState_EggPierrotAI_DashAttack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetBakudanAttackState() {
    static EnemyAIState_EggPierrotAI_BakudanAttack s_state;
    return &s_state;
}

EnemyAIState_EggPierrotAI_Wait::EnemyAIState_EggPierrotAI_Wait() : m_ai(0) {}

void EnemyAIState_EggPierrotAI_Wait::SetOwner(EnemyBaseAI *ai) {
    EggPierrotAI *owner = dynamic_cast<EggPierrotAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EggPierrotAI_GoHome::EnemyAIState_EggPierrotAI_GoHome() : m_ai(0) {}

void EnemyAIState_EggPierrotAI_GoHome::SetOwner(EnemyBaseAI *ai) {
    EggPierrotAI *owner = dynamic_cast<EggPierrotAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EggPierrotAI_Ottoto::EnemyAIState_EggPierrotAI_Ottoto() : m_ai(0) {}

void EnemyAIState_EggPierrotAI_Ottoto::SetOwner(EnemyBaseAI *ai) {
    EggPierrotAI *owner = dynamic_cast<EggPierrotAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EggPierrotAI_DashAttack::EnemyAIState_EggPierrotAI_DashAttack() : m_ai(0) {}

void EnemyAIState_EggPierrotAI_DashAttack::SetOwner(EnemyBaseAI *ai) {
    EggPierrotAI *owner = dynamic_cast<EggPierrotAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EggPierrotAI_BakudanAttack::EnemyAIState_EggPierrotAI_BakudanAttack() : m_ai(0) {}

void EnemyAIState_EggPierrotAI_BakudanAttack::SetOwner(EnemyBaseAI *ai) {
    EggPierrotAI *owner = dynamic_cast<EggPierrotAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EggPierrotAI::EggPierrotAI() {
    m_body = dynamic_cast<EggPierrot *>(m_enemy->m_disp);
}

EggPierrotAI::~EggPierrotAI() {}

void EggPierrotAI::vfAI_0x2C() {
    if (m_body->IsBallWeak()) {
        if (!IsInState(&m_stateMachine, ::EggPierrot::GetOttotoState())) {
            ChangeState(::EggPierrot::GetOttotoState());
        }
    } else if (IsInState(&m_stateMachine, ::EggPierrot::GetOttotoState())) {
        vfAI_0x3C();
    }
    EnemyBaseAI::vfAI_0x2C();
}

void EggPierrotAI::vfAI_0x38() {
    switch (m_body->m_params.AppearType) {
    case 1:
        ChangeState(::EggPierrot::GetGoHomeState());
        break;
    default:
        ChangeState(::EggPierrot::GetWaitState());
        break;
    }
}

void EggPierrotAI::vfAI_0x3C() {
    ChangeState(::EggPierrot::GetWaitState());
}

void EggPierrotAI::vfAI_0x48() {
    if (m_body->IsBallWeak()) {
        ChangeState(::EggPierrot::GetOttotoState());
    } else if (m_body->m_targetInArea) {
        ChangeState(::EggPierrot::GetDashAttackState());
    } else {
        ChangeState(::EggPierrot::GetBakudanAttackState());
    }
}

void EnemyAIState_EggPierrotAI_Wait::Enter() {
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
    m_ai->m_step = -1;
}

void EnemyAIState_EggPierrotAI_Wait::Update() {
    if (fn_8019AF04(m_ai->m_enemy)) {
        m_ai->vfAI_0x44();
        return;
    }
    switch (m_ai->m_step) {
    case -1:
        if (m_ai->m_enemy->m_status->vfStat_0x28()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), false);
            m_ai->m_step++;
        }
        break;
    case 0:
        if (m_ai->m_enemy->vf_0x0C()) {
            fn_8019B0F8(m_ai->m_enemy, &m_ai->m_enemy->field_0x60);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->m_enemy->vf_0x10() && m_ai->m_enemy->m_status->vfStat_0x28()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), false);
            m_ai->m_step++;
        }
        break;
    case 2:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), false);
        }
        break;
    }
}

void EnemyAIState_EggPierrotAI_Wait::Exit() {}

void EnemyAIState_EggPierrotAI_GoHome::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), false);
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
}

void EnemyAIState_EggPierrotAI_GoHome::Update() {
    if (m_ai->m_enemy->vf_0x0C()) {
        m_ai->vfAI_0x3C();
    }
}

void EnemyAIState_EggPierrotAI_GoHome::Exit() {}

void EnemyAIState_EggPierrotAI_Ottoto::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(4, true);
    m_ai->m_body->StopBakudan();
    Vec3f target;
    fn_800091EC(&target, &(const Vec3f &)fn_8040F268(m_ai->m_enemy->GetPosition(),
                                                    GetHome(m_ai->m_enemy), 0.2f));
    fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)fn_8019AADC(m_ai->m_enemy, &target, 5.0f));
    m_ai->SetTimer(3.0f);
    fn_8040CE60(fn_8028F324(m_ai->m_enemy));
}

void EnemyAIState_EggPierrotAI_Ottoto::Update() {
    fn_8019B0B0(m_ai->m_enemy, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    m_ai->m_enemy->vf_0x10();
    Vec3f move;
    fn_800091EC(&move, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                           &m_ai->m_enemy->field_0x6C, m_ai->m_enemy->GetPosition()));
    float step = m_ai->m_enemy->m_para->vfPara_0x20() * m_ai->m_enemy->GetFrameTime();
    float distance = fn_8040CF50(&move);
    if (step > distance) {
        step = distance;
    }
    fn_8040CE74(&move);
    fn_8040CE38(&move, step);
    AddVector(fn_8028F324(m_ai->m_enemy), move);
    AddVector(fn_80181F0C(m_ai->m_enemy),
              fn_800091A4(fn_8028F324(m_ai->m_enemy), m_ai->m_enemy->GetFrameTime()));
    if (m_ai->IsTimeUp() || fn_8019B298(m_ai->m_enemy, &m_ai->m_enemy->field_0x6C) < 2.0f) {
        m_ai->SetTimer(3.0f);
        Vec3f target;
        fn_800091EC(&target, &(const Vec3f &)fn_8040F268(m_ai->m_enemy->GetPosition(),
                                                        GetHome(m_ai->m_enemy), 0.01f));
        fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)fn_8019AADC(m_ai->m_enemy, &target, 5.0f));
    }
}

void EnemyAIState_EggPierrotAI_Ottoto::Exit() {
    m_ai->m_body->StartBakudan();
    fn_8040CE60(fn_8028F324(m_ai->m_enemy));
}

void EnemyAIState_EggPierrotAI_DashAttack::Enter() {
    m_ai->m_body->StopBakudan();
    fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f));
    m_ai->m_enemy->m_status->vfStat_0x20(2, true);
    m_ai->SetTimer(3.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_EggPierrotAI_DashAttack::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->IsTimeUp() || m_ai->m_body->vf_0x0C()) {
            m_ai->SetTimer(1.0f);
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), false);
            m_ai->m_step++;
        }
        break;
    case 1: {
        void *key = m_ai->vfCtrl_0x10();
        if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
            m_ai->vfAI_0x3C();
        } else if (!m_ai->m_body->m_targetInArea) {
            m_ai->vfAI_0x48();
        } else if (m_ai->IsTimeUp()) {
            fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f));
            m_ai->m_enemy->m_status->vfStat_0x20(2, true);
            m_ai->SetTimer(3.0f);
            m_ai->m_step--;
        }
        break;
    }
    }
}

void EnemyAIState_EggPierrotAI_DashAttack::Exit() {
    m_ai->m_body->StartBakudan();
}

void EnemyAIState_EggPierrotAI_BakudanAttack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(0, false);
    m_ai->m_body->m_throwReady = 1;
}

void EnemyAIState_EggPierrotAI_BakudanAttack::Update() {
    fn_8019B0B0(m_ai->m_enemy, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    m_ai->m_enemy->vf_0x10();
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->vfAI_0x3C();
    } else if (m_ai->m_body->m_targetInArea) {
        m_ai->vfAI_0x48();
    }
}

void EnemyAIState_EggPierrotAI_BakudanAttack::Exit() {
    m_ai->m_body->m_throwReady = 0;
}

} // namespace EggPierrot
