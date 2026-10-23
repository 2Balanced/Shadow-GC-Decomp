/* EggPawnAI (class name from RTTI): the Egg Pawn's AI (Game/EggPawn.h), an EnemyWalkerAI
 * (Game/EnemyWalkerAI.h). It waits standing or walking between its points and attacks with the
 * weapon it carries: firing the auto rifle or bazooka (WeaponAttack), thrusting the lance
 * (TukiAttack) or, unarmed, dashing at the target (DashAttack). File name inferred from the class.
 * Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* See EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_STATES

#include "Game/EggPawn.h"
#include "Game/EnemyAIState.h"
#include "Game/EnemyWalkerAI.h"
#include "Game/MathTypes.h"

extern "C" {
/* A point dist ahead of enemy toward position (behaviour inferred). */
Vec3f fn_8019AADC(Enemy *enemy, const Vec3f *position, float dist);
/* Turns enemy toward the tracked target id (behaviour inferred). */
void fn_8019B0B0(Enemy *enemy, int id);
}

/* The parameters' default for slot 0x48 (Game/Enemy.h; inline, first emitted here). */
inline int EnemyParaFunc::vfPara_0x48() {
    return -1;
}

/* The Egg Pawn's attack states (class names from RTTI; vtables 0x8052D784, 0x8052D7B0 and
 * 0x8052D7DC): the AI's step (EnemyBaseAI::m_step) walks them through aiming, attacking and
 * recovering. */
class EnemyAIState_EggPawnAI_WeaponAttack : public TEnemyAIState {
  public:
    EnemyAIState_EggPawnAI_WeaponAttack();
    virtual ~EnemyAIState_EggPawnAI_WeaponAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggPawnAI *m_ai;
};

class EnemyAIState_EggPawnAI_TukiAttack : public TEnemyAIState {
  public:
    EnemyAIState_EggPawnAI_TukiAttack();
    virtual ~EnemyAIState_EggPawnAI_TukiAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggPawnAI *m_ai;
};

class EnemyAIState_EggPawnAI_DashAttack : public TEnemyAIState {
  public:
    EnemyAIState_EggPawnAI_DashAttack();
    virtual ~EnemyAIState_EggPawnAI_DashAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggPawnAI *m_ai;
};

EnemyAIState_EggPawnAI_DashAttack::EnemyAIState_EggPawnAI_DashAttack() : m_ai(0) {}

void EnemyAIState_EggPawnAI_DashAttack::SetOwner(EnemyBaseAI *ai) {
    EggPawnAI *owner = dynamic_cast<EggPawnAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EggPawnAI_TukiAttack::EnemyAIState_EggPawnAI_TukiAttack() : m_ai(0) {}

void EnemyAIState_EggPawnAI_TukiAttack::SetOwner(EnemyBaseAI *ai) {
    EggPawnAI *owner = dynamic_cast<EggPawnAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EggPawnAI_WeaponAttack::EnemyAIState_EggPawnAI_WeaponAttack() : m_ai(0) {}

void EnemyAIState_EggPawnAI_WeaponAttack::SetOwner(EnemyBaseAI *ai) {
    EggPawnAI *owner = dynamic_cast<EggPawnAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EggPawnAI::EggPawnAI() {
    m_body = dynamic_cast<EggPawnBase *>(m_enemy->m_disp);
}

EggPawnAI::~EggPawnAI() {}

void EggPawnAI::vfAI_0x38() {
    switch (m_body->GetAppearType()) {
    case 0:
        if (m_body->vf_0x250() == 0) {
            fn_8019F944(&m_stateMachine, GetStandingState());
        } else {
            fn_8019F944(&m_stateMachine, GetMovingState());
        }
        break;
    case 1:
    case 2:
        fn_8019F944(&m_stateMachine, GetReturnHomeState());
        break;
    case 3:
        fn_8019F944(&m_stateMachine, GetReturnHomeDashState());
        break;
    default:
        fn_8019F944(&m_stateMachine, GetStandingState());
        break;
    }
}

void EggPawnAI::vfAI_0x3C() {
    if (m_body->vf_0x250() == 0) {
        fn_8019F944(&m_stateMachine, GetStandingState());
    } else {
        fn_8019F944(&m_stateMachine, GetMovingState());
    }
}

/* The attack states' objects (names inferred). */
static TEnemyAIState *GetDashAttackState() {
    static EnemyAIState_EggPawnAI_DashAttack s_state;
    return &s_state;
}

static TEnemyAIState *GetTukiAttackState() {
    static EnemyAIState_EggPawnAI_TukiAttack s_state;
    return &s_state;
}

static TEnemyAIState *GetWeaponAttackState() {
    static EnemyAIState_EggPawnAI_WeaponAttack s_state;
    return &s_state;
}

void EggPawnAI::vfAI_0x48() {
    switch (m_body->GetWeaponType()) {
    case 0:
        fn_8019F944(&m_stateMachine, GetDashAttackState());
        break;
    case 3:
        fn_8019F944(&m_stateMachine, GetTukiAttackState());
        break;
    default:
        fn_8019F944(&m_stateMachine, GetWeaponAttackState());
        break;
    }
}

void EnemyAIState_EggPawnAI_DashAttack::Enter() {
    if (m_ai->m_body->HasShield()) {
        m_ai->m_enemy->m_status->vfStat_0x20(13, true);
    } else {
        m_ai->m_enemy->m_status->vfStat_0x20(12, true);
    }
    fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f));
    m_ai->SetTimer(3.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_EggPawnAI_DashAttack::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->IsTimeUp() || m_ai->m_body->vf_0x0C()) {
            m_ai->SetTimer(1.0f);
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
            m_ai->m_step++;
        }
        break;
    case 1:
        void *key = m_ai->vfCtrl_0x10();
        if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
            m_ai->vfAI_0x3C();
        } else if (m_ai->IsTimeUp()) {
            m_ai->SetTimer(3.0f);
            fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f));
            if (m_ai->m_body->HasShield()) {
                m_ai->m_enemy->m_status->vfStat_0x20(13, true);
            } else {
                m_ai->m_enemy->m_status->vfStat_0x20(12, true);
            }
            m_ai->m_step--;
        }
        break;
    }
}

void EnemyAIState_EggPawnAI_DashAttack::Exit() {}

void EnemyAIState_EggPawnAI_TukiAttack::Enter() {
    if (m_ai->m_body->HasShield()) {
        m_ai->m_enemy->m_status->vfStat_0x20(13, true);
    } else {
        m_ai->m_enemy->m_status->vfStat_0x20(12, true);
    }
    fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f));
    m_ai->SetTimer(3.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_EggPawnAI_TukiAttack::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->IsTimeUp() || fn_8019F710(&m_ai->m_target) < 15.0f ||
            m_ai->m_body->vf_0x0C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(14, true);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->SetTimer(1.0f);
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
            m_ai->m_step++;
        }
        break;
    case 2:
        void *key = m_ai->vfCtrl_0x10();
        if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
            m_ai->vfAI_0x3C();
        } else if (m_ai->IsTimeUp()) {
            m_ai->SetTimer(3.0f);
            fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f));
            if (m_ai->m_body->HasShield()) {
                m_ai->m_enemy->m_status->vfStat_0x20(13, true);
            } else {
                m_ai->m_enemy->m_status->vfStat_0x20(12, true);
            }
            m_ai->m_step = 0;
        }
        break;
    }
}

void EnemyAIState_EggPawnAI_TukiAttack::Exit() {}

void EnemyAIState_EggPawnAI_WeaponAttack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vfPara_0x58(), true);
    m_ai->m_step = 0;
}

void EnemyAIState_EggPawnAI_WeaponAttack::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vfPara_0x4C(), true);
            fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)fn_8019AADC(m_ai->m_body,
                                                                    m_ai->m_enemy->GetPosition(),
                                                                    30.0f));
            m_ai->SetTimer(2.0f);
            m_ai->m_step++;
        }
        break;
    case 1:
        void *key = m_ai->vfCtrl_0x10();
        if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
            m_ai->vfAI_0x3C();
        } else if (m_ai->m_body->vf_0x0C() || m_ai->IsTimeUp()) {
            fn_8019B0B0(m_ai->m_enemy, fn_8019F65C(&m_ai->m_target));
            m_ai->m_step++;
        }
        break;
    case 2:
        if (m_ai->m_body->vf_0x10()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vfPara_0x58(), true);
            m_ai->m_step = 0;
        }
        break;
    }
}

void EnemyAIState_EggPawnAI_WeaponAttack::Exit() {}
