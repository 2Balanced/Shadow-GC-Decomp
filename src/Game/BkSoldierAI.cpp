/* BkSoldierAI (class name from RTTI): the Black Arms soldier's AI (Game/BkSoldier.h), a
 * SoldierCommonAI (Game/EnemyAI.h). It waits standing or walking between its points
 * (SoldierCommonAI's states), comes out and walks home (AppearHome), cautions, chases the target
 * and attacks with the weapon it carries: from afar (Attack), from close by (NearAttack) or, with
 * the vacuum gun, aiming at the target until it gives up (AttackVacuum). File name inferred from
 * the class. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
/* See EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_STATES
#define SOLDIER_COMMON_AI_STATES

#include "Game/BkSoldier.h"
#include "Game/EnemyAIState.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp). */
namespace std {
float fabs(float x);
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* The parameters' defaults for slots 0x4C, 0x54 and 0x58 (Game/Enemy.h; inline, first emitted
 * here). */
inline int EnemyParaFunc::vfPara_0x4C() {
    return -1;
}

inline int EnemyParaFunc::vfPara_0x54() {
    return -1;
}

inline int EnemyParaFunc::vfPara_0x58() {
    return -1;
}

/* The display's default for slot 0x3C (Game/Enemy.h; inline, first emitted here). */
inline void EnemyDispFunc::vfDisp_0x3C(bool moving) {}

/* The Black Arms soldier's states (class names from RTTI; vtables 0x8052DCA8 to 0x8052DDB0). */
class EnemyAIState_BkSoldierAI_AppearHome : public TEnemyAIState {
  public:
    EnemyAIState_BkSoldierAI_AppearHome();
    virtual ~EnemyAIState_BkSoldierAI_AppearHome() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkSoldierAI *m_ai;
};

class EnemyAIState_BkSoldierAI_GoHome : public TEnemyAIState {
  public:
    EnemyAIState_BkSoldierAI_GoHome();
    virtual ~EnemyAIState_BkSoldierAI_GoHome() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkSoldierAI *m_ai;
};

class EnemyAIState_BkSoldierAI_AttackVacuum : public TEnemyAIState {
  public:
    EnemyAIState_BkSoldierAI_AttackVacuum();
    virtual ~EnemyAIState_BkSoldierAI_AttackVacuum() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkSoldierAI *m_ai;
};

class EnemyAIState_BkSoldierAI_Attack : public TEnemyAIState {
  public:
    EnemyAIState_BkSoldierAI_Attack();
    virtual ~EnemyAIState_BkSoldierAI_Attack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkSoldierAI *m_ai;
};

class EnemyAIState_BkSoldierAI_NearAttack : public TEnemyAIState {
  public:
    EnemyAIState_BkSoldierAI_NearAttack();
    virtual ~EnemyAIState_BkSoldierAI_NearAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkSoldierAI *m_ai;
};

class EnemyAIState_BkSoldierAI_ChaseTarget : public TEnemyAIState {
  public:
    EnemyAIState_BkSoldierAI_ChaseTarget();
    virtual ~EnemyAIState_BkSoldierAI_ChaseTarget() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkSoldierAI *m_ai;
};

class EnemyAIState_BkSoldierAI_Caution : public TEnemyAIState {
  public:
    EnemyAIState_BkSoldierAI_Caution();
    virtual ~EnemyAIState_BkSoldierAI_Caution() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkSoldierAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetCautionState() {
    static EnemyAIState_BkSoldierAI_Caution s_state;
    return &s_state;
}

static inline TEnemyAIState *GetChaseTargetState() {
    static EnemyAIState_BkSoldierAI_ChaseTarget s_state;
    return &s_state;
}

static inline TEnemyAIState *GetNearAttackState() {
    static EnemyAIState_BkSoldierAI_NearAttack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAttackState() {
    static EnemyAIState_BkSoldierAI_Attack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAttackVacuumState() {
    static EnemyAIState_BkSoldierAI_AttackVacuum s_state;
    return &s_state;
}

static inline TEnemyAIState *GetGoHomeState() {
    static EnemyAIState_BkSoldierAI_GoHome s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAppearHomeState() {
    static EnemyAIState_BkSoldierAI_AppearHome s_state;
    return &s_state;
}

/* Whether the target is more than limit degrees off the enemy's facing (name inferred). */
static inline bool IsFacingAway(EnemyBaseAI *ai, float limit) {
    float radians = limit * (lbl_805FAD38 / 180.0f);
    return std::fabs(fn_8019F698(&ai->m_target)) > radians;
}

/* A random bit (the low bit of a counter in lbl_8057D6A0; meaning inferred). */
static inline u32 RandomBit() {
    return *reinterpret_cast<u32 *>(&lbl_8057D6A0[0x88]) & 1;
}

EnemyAIState_BkSoldierAI_Caution::EnemyAIState_BkSoldierAI_Caution() : m_ai(0) {}

void EnemyAIState_BkSoldierAI_Caution::SetOwner(EnemyBaseAI *ai) {
    BkSoldierAI *owner = dynamic_cast<BkSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkSoldierAI_ChaseTarget::EnemyAIState_BkSoldierAI_ChaseTarget() : m_ai(0) {}

void EnemyAIState_BkSoldierAI_ChaseTarget::SetOwner(EnemyBaseAI *ai) {
    BkSoldierAI *owner = dynamic_cast<BkSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkSoldierAI_NearAttack::EnemyAIState_BkSoldierAI_NearAttack() : m_ai(0) {}

void EnemyAIState_BkSoldierAI_NearAttack::SetOwner(EnemyBaseAI *ai) {
    BkSoldierAI *owner = dynamic_cast<BkSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkSoldierAI_Attack::EnemyAIState_BkSoldierAI_Attack() : m_ai(0) {}

void EnemyAIState_BkSoldierAI_Attack::SetOwner(EnemyBaseAI *ai) {
    BkSoldierAI *owner = dynamic_cast<BkSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkSoldierAI_AttackVacuum::EnemyAIState_BkSoldierAI_AttackVacuum() : m_ai(0) {}

void EnemyAIState_BkSoldierAI_AttackVacuum::SetOwner(EnemyBaseAI *ai) {
    BkSoldierAI *owner = dynamic_cast<BkSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkSoldierAI_GoHome::EnemyAIState_BkSoldierAI_GoHome() : m_ai(0) {}

void EnemyAIState_BkSoldierAI_GoHome::SetOwner(EnemyBaseAI *ai) {
    BkSoldierAI *owner = dynamic_cast<BkSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkSoldierAI_AppearHome::EnemyAIState_BkSoldierAI_AppearHome() : m_ai(0) {}

void EnemyAIState_BkSoldierAI_AppearHome::SetOwner(EnemyBaseAI *ai) {
    BkSoldierAI *owner = dynamic_cast<BkSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

BkSoldierAI::BkSoldierAI() {
    m_appeared = 0;
    m_nearAttack = 0;
    m_body = dynamic_cast<BkSoldierBase *>(m_enemy->m_disp);
}

void BkSoldierAI::vfAI_0x3C() {
    switch (m_body->m_appearType) {
    case 0:
        fn_8019F944(&m_stateMachine, GetStandingState());
        break;
    case 1:
    case 2:
    case 3:
        fn_8019F944(&m_stateMachine, GetMovingState());
        break;
    case 4:
    case 5:
        if (m_appeared) {
            fn_8019F944(&m_stateMachine, GetStandingState());
        } else {
            m_appeared = 1;
            ChangeState(::GetAppearHomeState());
        }
        break;
    default:
        fn_8019F944(&m_stateMachine, GetStandingState());
        break;
    }
}

void BkSoldierAI::vfAI_0x44() {
    ChangeState(::GetCautionState());
}

void BkSoldierAI::StartAttack() {
    int weapon = m_body->GetWeaponType();
    if (weapon == 0 || weapon == 1) {
        ChangeState(::GetChaseTargetState());
    } else if (weapon == 6) {
        ChangeState(::GetAttackVacuumState());
    } else {
        ChangeState(::GetAttackState());
    }
}

/* Not original: unused functions the linker drops. Retail has the weak copies of the slots they
 * call where these stand, so the original called them from code that was dropped too; the calls
 * put the copies in the same places. */
bool BkSoldierAI_UnusedStatus0x34(BkSoldierAI *ai) {
    return ai->m_enemy->m_status->vfStat_0x34();
}

void EnemyAIState_BkSoldierAI_Caution::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x54(), true);
}

void EnemyAIState_BkSoldierAI_Caution::Update() {
    fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    int weapon = m_ai->m_body->GetWeaponType();
    if (weapon == 0 || weapon == 1) {
        m_ai->ChangeState(GetChaseTargetState());
    } else if (m_ai->m_enemy->m_status->vfStat_0x28()) {
        m_ai->StartAttack();
    }
}

void EnemyAIState_BkSoldierAI_Caution::Exit() {}

void EnemyAIState_BkSoldierAI_ChaseTarget::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
}

void EnemyAIState_BkSoldierAI_ChaseTarget::Update() {
    fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        fn_8019F944(&m_ai->m_stateMachine, EnemyBaseAI::GetGoHomeState());
    } else if (fn_8019F710(&m_ai->m_target) < 30.0f) {
        m_ai->ChangeState(GetNearAttackState());
    }
}

void EnemyAIState_BkSoldierAI_ChaseTarget::Exit() {}

void EnemyAIState_BkSoldierAI_NearAttack::Enter() {
    m_ai->m_body->field_0x348 = RandomBit();
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
}

void EnemyAIState_BkSoldierAI_NearAttack::Update() {
    fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        fn_8019F944(&m_ai->m_stateMachine, EnemyBaseAI::GetGoHomeState());
    } else {
        if (m_ai->m_nearAttack) {
            m_ai->m_nearAttack = 0;
        }
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            if (fn_8019F710(&m_ai->m_target) > 30.0f) {
                m_ai->ChangeState(GetChaseTargetState());
            } else {
                m_ai->m_nearAttack = 1;
                m_ai->m_body->field_0x348 = RandomBit();
                m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
            }
        }
    }
}

void EnemyAIState_BkSoldierAI_NearAttack::Exit() {
    m_ai->m_nearAttack = 0;
}

void EnemyAIState_BkSoldierAI_Attack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
    m_ai->m_body->field_0x359 = 0;
    m_ai->SetTimer(1.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_BkSoldierAI_Attack::Update() {
    switch (m_ai->m_step) {
    case 0:
        m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
        if (fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target)) ||
            m_ai->IsTimeUp()) {
            m_ai->m_body->field_0x348 = RandomBit();
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
            m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
            fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f));
            m_ai->m_body->field_0x359 = 1;
            m_ai->SetTimer(3.0f);
            m_ai->m_step++;
        }
        break;
    case 1:
        fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f));
        if (m_ai->IsTimeUp() ||
            (m_ai->m_enemy->m_status->vfStat_0x28() &&
             IsFacingAway(m_ai, 30.0f))) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
            m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
            m_ai->m_body->field_0x359 = 0;
            m_ai->SetTimer(1.0f);
            m_ai->m_step--;
            return;
        } else if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
        }
        break;
    }
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->ChangeState(GetGoHomeState());
    }
}

void EnemyAIState_BkSoldierAI_Attack::Exit() {
    m_ai->m_body->field_0x359 = 0;
    m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
}

void EnemyAIState_BkSoldierAI_AttackVacuum::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
    m_ai->SetTimer(1.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_BkSoldierAI_AttackVacuum::Update() {
    switch (m_ai->m_step) {
    case 0:
        m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
        if (fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target)) ||
            m_ai->IsTimeUp()) {
            m_ai->m_enemy->m_status->vfStat_0x20(13, true);
            m_ai->m_body->fn_8018D3B0();
            fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f));
            m_ai->m_body->field_0x359 = 1;
            m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
            m_ai->SetTimer(3.0f);
            m_ai->m_step++;
        }
        break;
    case 1:
        fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f));
        if (m_ai->IsTimeUp() ||
            IsFacingAway(m_ai, 30.0f)) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
            m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
            m_ai->m_body->field_0x359 = 0;
            m_ai->SetTimer(1.0f);
            m_ai->m_body->fn_8018D34C();
            m_ai->m_step--;
            return;
        }
        break;
    }
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->ChangeState(GetGoHomeState());
    }
}

void EnemyAIState_BkSoldierAI_AttackVacuum::Exit() {
    m_ai->m_body->fn_8018D34C();
    m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
}

void EnemyAIState_BkSoldierAI_GoHome::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
}

void EnemyAIState_BkSoldierAI_GoHome::Update() {
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                &m_ai->m_enemy->field_0x6C, m_ai->m_enemy->GetPosition()));
    fn_8019F53C(&m_ai->m_turn, &direction);
    void *key = m_ai->vfCtrl_0x10();
    if (m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->ChangeState(GetCautionState());
    } else {
        direction.y = 0.0f;
        if (fn_8040CF50(&direction) < 10.0f) {
            m_ai->vfAI_0x3C();
        }
    }
}

void EnemyAIState_BkSoldierAI_GoHome::Exit() {}

void EnemyAIState_BkSoldierAI_AppearHome::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
}

void EnemyAIState_BkSoldierAI_AppearHome::Update() {
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                &m_ai->m_enemy->field_0x6C, m_ai->m_enemy->GetPosition()));
    fn_8019F53C(&m_ai->m_turn, &direction);
    direction.y = 0.0f;
    if (fn_8040CF50(&direction) < 10.0f) {
        m_ai->StartAttack();
    }
}

void EnemyAIState_BkSoldierAI_AppearHome::Exit() {}

/* Not original (see BkSoldierAI_UnusedStatus0x34). */
int BkSoldierAI_UnusedPara0x4C(BkSoldierAI *ai) {
    return ai->m_enemy->m_para->vfPara_0x4C();
}
