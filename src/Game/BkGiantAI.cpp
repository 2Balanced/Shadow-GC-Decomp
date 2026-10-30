/* BkGiant::BkGiantAI (class name from RTTI): the Black Arms giant's AI (Game/BkGiant.h). Once the
 * target is in its area the giant chases it (ChaseTarget) and swings its weapon at it from close
 * by (StickAttack), or, carrying the big barrel, turns to it and fires (GunAttack); after three
 * hits in a row it strikes back (RejectAttack). File name inferred from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
/* See EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_STATES

#include "Game/BkGiant.h"
#include "Game/EnemyAIState.h"
#include "Game/EnemyWeapon.h"
#include "Game/MathTypes.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in BkSoldier.cpp). */
namespace std {
float fabs(float x);
}

extern float lbl_805FAD38; /* pi */

extern "C" {
/* Tests bit of a flag word (see ThrowBomb.cpp). */
bool fn_80014E34(u32 *flags, int bit);
/* Goes back to the state before the current one, and leaves the current one (behaviours
 * inferred). */
void fn_8019F898(UnkAIStateMachine_8019F944 *machine);
void fn_8019F9F0(UnkAIStateMachine_8019F944 *machine);
}

/* Degrees to radians (name inferred; as in BalloonGenerator.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace BkGiant {

/* The giant's states (class names from RTTI; vtables 0x80551F34 to 0x80551FB8). */
class EnemyAIState_BkGiantAI_RejectAttack : public TEnemyAIState {
  public:
    EnemyAIState_BkGiantAI_RejectAttack();
    virtual ~EnemyAIState_BkGiantAI_RejectAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkGiantAI *m_ai;
};

class EnemyAIState_BkGiantAI_GunAttack : public TEnemyAIState {
  public:
    EnemyAIState_BkGiantAI_GunAttack();
    virtual ~EnemyAIState_BkGiantAI_GunAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkGiantAI *m_ai;
};

class EnemyAIState_BkGiantAI_StickAttack : public TEnemyAIState {
  public:
    EnemyAIState_BkGiantAI_StickAttack();
    virtual ~EnemyAIState_BkGiantAI_StickAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkGiantAI *m_ai;
};

class EnemyAIState_BkGiantAI_ChaseTarget : public TEnemyAIState {
  public:
    EnemyAIState_BkGiantAI_ChaseTarget();
    virtual ~EnemyAIState_BkGiantAI_ChaseTarget() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkGiantAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetChaseTargetState() {
    static EnemyAIState_BkGiantAI_ChaseTarget s_state;
    return &s_state;
}

static inline TEnemyAIState *GetStickAttackState() {
    static EnemyAIState_BkGiantAI_StickAttack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetGunAttackState() {
    static EnemyAIState_BkGiantAI_GunAttack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetRejectAttackState() {
    static EnemyAIState_BkGiantAI_RejectAttack s_state;
    return &s_state;
}

EnemyAIState_BkGiantAI_ChaseTarget::EnemyAIState_BkGiantAI_ChaseTarget() : m_ai(0) {}

void EnemyAIState_BkGiantAI_ChaseTarget::SetOwner(EnemyBaseAI *ai) {
    BkGiantAI *owner = dynamic_cast<BkGiantAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkGiantAI_StickAttack::EnemyAIState_BkGiantAI_StickAttack() : m_ai(0) {}

void EnemyAIState_BkGiantAI_StickAttack::SetOwner(EnemyBaseAI *ai) {
    BkGiantAI *owner = dynamic_cast<BkGiantAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkGiantAI_GunAttack::EnemyAIState_BkGiantAI_GunAttack() : m_ai(0) {}

void EnemyAIState_BkGiantAI_GunAttack::SetOwner(EnemyBaseAI *ai) {
    BkGiantAI *owner = dynamic_cast<BkGiantAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkGiantAI_RejectAttack::EnemyAIState_BkGiantAI_RejectAttack() : m_ai(0) {}

void EnemyAIState_BkGiantAI_RejectAttack::SetOwner(EnemyBaseAI *ai) {
    BkGiantAI *owner = dynamic_cast<BkGiantAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

BkGiantAI::BkGiantAI() {
    m_body = dynamic_cast<BkGiant *>(m_enemy->m_disp);
}

BkGiantAI::~BkGiantAI() {}

void BkGiantAI::vfAI_0x2C() {
    EnemyBaseAI::vfAI_0x2C();
    if (m_body->m_hits >= 3) {
        m_body->m_hits = 0;
        ChangeState(GetRejectAttackState());
    }
}

void BkGiantAI::vfAI_0x38() {
    fn_8019F944(&m_stateMachine, GetGoHomeState());
}

void BkGiantAI::vfAI_0x3C() {
    fn_8019F944(&m_stateMachine, GetWaitState());
}

void BkGiantAI::vfAI_0x48() {
    if (m_body->m_params.WeaponType == 2) {
        ChangeState(GetGunAttackState());
    } else {
        ChangeState(GetChaseTargetState());
    }
}

void BkGiantAI::vfAI_0x50() {
    if (fn_80014E34(&m_enemy->m_status->vfStat_0xA8()->m_bits, 2)) {
        fn_8019F9F0(&m_stateMachine);
    }
    EnemyBaseAI::vfAI_0x50();
}

void EnemyAIState_BkGiantAI_ChaseTarget::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    fn_8017DE10(m_ai->m_enemy, static_cast<const Vec3f *>(m_ai->vfCtrl_0x10()));
    m_ai->SetTimer(3.0f);
}

void EnemyAIState_BkGiantAI_ChaseTarget::Update() {
    fn_8017DE10(m_ai->m_enemy, static_cast<const Vec3f *>(m_ai->vfCtrl_0x10()));
    m_ai->m_enemy->vf_0x0C();
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        fn_8019F944(&m_ai->m_stateMachine, EnemyBaseAI::GetGoHomeState());
    } else if (fn_8019F710(&m_ai->m_target) < 30.0f) {
        m_ai->ChangeState(GetStickAttackState());
    } else if (m_ai->IsTimeUp()) {
        m_ai->vfAI_0x3C();
    }
}

void EnemyAIState_BkGiantAI_ChaseTarget::Exit() {}

void EnemyAIState_BkGiantAI_StickAttack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
    if (m_ai->m_body->m_weapon != 0) {
        fn_801AABD4(m_ai->m_body->m_weapon);
    }
}

void EnemyAIState_BkGiantAI_StickAttack::Update() {
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->vfAI_0x3C();
    } else if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        if (fn_8019F710(&m_ai->m_target) > 30.0f) {
            m_ai->ChangeState(GetChaseTargetState());
        } else {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
        }
    }
}

void EnemyAIState_BkGiantAI_StickAttack::Exit() {
    if (m_ai->m_body->m_weapon != 0) {
        fn_801AAB68(m_ai->m_body->m_weapon);
    }
}

void EnemyAIState_BkGiantAI_GunAttack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    m_ai->SetTimer(1.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_BkGiantAI_GunAttack::Update() {
    switch (m_ai->m_step) {
    case 0:
        m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
        if (fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target)) ||
            m_ai->IsTimeUp()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
            m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
            m_ai->SetTimer(3.0f);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->IsTimeUp() || (m_ai->m_enemy->m_status->vfStat_0x28() &&
                                 std::fabs(fn_8019F698(&m_ai->m_target)) > DegToRad(30.0f))) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
            m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
            m_ai->SetTimer(1.0f);
            m_ai->m_step--;
            return;
        }
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
        }
        break;
    }
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        fn_8019F944(&m_ai->m_stateMachine, EnemyBaseAI::GetGoHomeState());
    }
}

void EnemyAIState_BkGiantAI_GunAttack::Exit() {
    m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
}

void EnemyAIState_BkGiantAI_RejectAttack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(7, true);
    m_ai->m_step = 0;
    if (m_ai->m_body->m_weapon != 0) {
        fn_801AABD4(m_ai->m_body->m_weapon);
    }
}

void EnemyAIState_BkGiantAI_RejectAttack::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->SetTimer(1.0f);
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->IsTimeUp()) {
            fn_8019F898(&m_ai->m_stateMachine);
        }
        break;
    }
}

void EnemyAIState_BkGiantAI_RejectAttack::Exit() {
    if (m_ai->m_body->m_weapon != 0) {
        fn_801AAB68(m_ai->m_body->m_weapon);
    }
}

} // namespace BkGiant
