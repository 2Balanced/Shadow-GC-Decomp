/* BkNinja::BkNinjaAI (class name from RTTI): the Black Arms ninja's AI (Game/BkNinja.h). Once the
 * target is in its area the ninja turns to it and throws its weapon ShootCount times (GunAttack),
 * then vanishes and reappears at a random point near the target (MoveDisappear). With AppearType
 * 2 or 3 it waits by warping between its two points (WaitWarp) until the target comes, and then
 * appears at home (AppearHome). File name inferred from the class. Names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* See EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_STATES

#include "Game/BkNinja.h"
#include "Game/EnemyAIState.h"
#include "Game/MathTypes.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in BkSoldier.cpp). */
namespace std {
float fabs(float x);
}

extern float lbl_805FAD38; /* pi */

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019A954(Enemy *enemy, u16 sound, int arg);
/* The yaw that faces the AI's target (behaviour inferred; Game/EnemyBaseAI.cpp). */
float fn_8019F6D4(UnkAITarget_8019F65C *target);
}

/* Degrees to radians (name inferred; as in BalloonGenerator.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace BkNinja {

/* The ninja's states (class names from RTTI; vtables 0x80555C74 to 0x80555CF8). */
class EnemyAIState_BkNinjaAI_GunAttack : public TEnemyAIState {
  public:
    EnemyAIState_BkNinjaAI_GunAttack();
    virtual ~EnemyAIState_BkNinjaAI_GunAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkNinjaAI *m_ai;
};

class EnemyAIState_BkNinjaAI_MoveDisappear : public TEnemyAIState {
  public:
    EnemyAIState_BkNinjaAI_MoveDisappear();
    virtual ~EnemyAIState_BkNinjaAI_MoveDisappear() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkNinjaAI *m_ai;
};

class EnemyAIState_BkNinjaAI_AppearHome : public TEnemyAIState {
  public:
    EnemyAIState_BkNinjaAI_AppearHome();
    virtual ~EnemyAIState_BkNinjaAI_AppearHome() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkNinjaAI *m_ai;
};

class EnemyAIState_BkNinjaAI_WaitWarp : public TEnemyAIState {
  public:
    EnemyAIState_BkNinjaAI_WaitWarp();
    virtual ~EnemyAIState_BkNinjaAI_WaitWarp() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkNinjaAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetWaitWarpState() {
    static EnemyAIState_BkNinjaAI_WaitWarp s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAppearHomeState() {
    static EnemyAIState_BkNinjaAI_AppearHome s_state;
    return &s_state;
}

static inline TEnemyAIState *GetMoveDisappearState() {
    static EnemyAIState_BkNinjaAI_MoveDisappear s_state;
    return &s_state;
}

static inline TEnemyAIState *GetGunAttackState() {
    static EnemyAIState_BkNinjaAI_GunAttack s_state;
    return &s_state;
}

EnemyAIState_BkNinjaAI_WaitWarp::EnemyAIState_BkNinjaAI_WaitWarp() : m_ai(0) {}

void EnemyAIState_BkNinjaAI_WaitWarp::SetOwner(EnemyBaseAI *ai) {
    BkNinjaAI *owner = dynamic_cast<BkNinjaAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkNinjaAI_AppearHome::EnemyAIState_BkNinjaAI_AppearHome() : m_ai(0) {}

void EnemyAIState_BkNinjaAI_AppearHome::SetOwner(EnemyBaseAI *ai) {
    BkNinjaAI *owner = dynamic_cast<BkNinjaAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkNinjaAI_MoveDisappear::EnemyAIState_BkNinjaAI_MoveDisappear() : m_ai(0) {}

void EnemyAIState_BkNinjaAI_MoveDisappear::SetOwner(EnemyBaseAI *ai) {
    BkNinjaAI *owner = dynamic_cast<BkNinjaAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkNinjaAI_GunAttack::EnemyAIState_BkNinjaAI_GunAttack() : m_ai(0) {}

void EnemyAIState_BkNinjaAI_GunAttack::SetOwner(EnemyBaseAI *ai) {
    BkNinjaAI *owner = dynamic_cast<BkNinjaAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

BkNinjaAI::BkNinjaAI() {
    m_body = dynamic_cast<BkNinja *>(m_enemy->m_disp);
}

BkNinjaAI::~BkNinjaAI() {}

void BkNinjaAI::vfAI_0x38() {
    switch (m_body->m_params.AppearType) {
    case 1:
        fn_8019F944(&m_stateMachine, GetGoHomeState());
        break;
    case 2:
    case 3:
        ChangeState(GetWaitWarpState());
        break;
    case 0:
        fn_8019F944(&m_stateMachine, GetGoHomeState());
        break;
    default:
        fn_8019F944(&m_stateMachine, GetGoHomeState());
        break;
    }
}

void BkNinjaAI::vfAI_0x3C() {
    switch (m_body->m_params.AppearType) {
    case 1:
        fn_8019F944(&m_stateMachine, GetWaitState());
        break;
    case 2:
    case 3:
        ChangeState(GetWaitWarpState());
        break;
    case 0:
        fn_8019F944(&m_stateMachine, GetWaitState());
        break;
    default:
        fn_8019F944(&m_stateMachine, GetWaitState());
        break;
    }
}

void BkNinjaAI::vfAI_0x48() {
    ChangeState(GetGunAttackState());
}

void EnemyAIState_BkNinjaAI_WaitWarp::Enter() {
    m_ai->m_body->m_hidden = 1;
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->GetWaitMotion(), true);
    fn_800091D0(m_ai->m_enemy->GetPosition(), &(const Vec3f &)m_ai->m_body->GetWarpPosition());
    m_ai->m_enemy->GetRotation()->y = m_ai->m_body->GetWarpAngle();
    m_ai->m_body->Disappear();
    m_ai->SetTimer(1.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_BkNinjaAI_WaitWarp::Update() {
    m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
    switch (m_ai->m_step) {
    case 0:
        m_ai->m_enemy->m_disp->vfDisp_0x50();
        m_ai->m_step++;
    case 1:
        if (m_ai->IsTimeUp()) {
            if (m_ai->m_body->GetWaitInterval() > 0.0f) {
                m_ai->SetTimer(m_ai->m_body->GetWaitInterval());
                m_ai->m_step = 2;
            } else {
                m_ai->m_step = 100;
            }
            m_ai->m_body->Appear();
        }
        break;
    case 2:
        if (m_ai->IsTimeUp()) {
            m_ai->SetTimer(0.5f);
            m_ai->m_body->Disappear();
            m_ai->m_step = 3;
        }
        break;
    case 3:
        if (m_ai->IsTimeUp()) {
            m_ai->SetTimer(m_ai->m_body->GetDisappearTime());
            m_ai->m_step = 4;
        }
        break;
    case 4: {
        void *key = m_ai->vfCtrl_0x10();
        if (m_ai->m_enemy->field_0xA8.Contains(key)) {
            m_ai->ChangeState(GetAppearHomeState());
        } else if (m_ai->IsTimeUp()) {
            m_ai->m_body->NextWarpPoint();
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->GetWaitMotion(), true);
            fn_800091D0(m_ai->m_enemy->GetPosition(), &(const Vec3f &)m_ai->m_body->GetWarpPosition());
            m_ai->m_enemy->GetRotation()->y = m_ai->m_body->GetWarpAngle();
            m_ai->SetTimer(1.0f);
            m_ai->m_step = 0;
        }
        break;
    }
    case 100:
        break;
    }
}

void EnemyAIState_BkNinjaAI_WaitWarp::Exit() {
    m_ai->m_body->m_hidden = 0;
    m_ai->m_body->Appear();
}

void EnemyAIState_BkNinjaAI_MoveDisappear::Enter() {
    m_ai->m_body->m_hidden = 1;
    m_ai->SetTimer(1.0f);
    m_ai->m_step = 0;
    m_ai->m_body->Disappear();
}

void EnemyAIState_BkNinjaAI_MoveDisappear::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->IsTimeUp()) {
            fn_800091D0(m_ai->m_enemy->GetPosition(),
                        reinterpret_cast<const Vec3f *>(&(const Fn8040CopyVector3 &)m_ai->m_body->FindWarpPoint(
                            static_cast<const Vec3f *>(m_ai->vfCtrl_0x10()), 100.0f)));
            m_ai->m_enemy->GetRotation()->y = fn_8019F6D4(&m_ai->m_target);
            m_ai->SetTimer(1.0f);
            m_ai->m_step++;
        }
        break;
    case 1:
        m_ai->m_enemy->m_disp->vfDisp_0x50();
        m_ai->m_step++;
    case 2:
        if (m_ai->IsTimeUp()) {
            m_ai->vfAI_0x48();
        }
        break;
    }
}

void EnemyAIState_BkNinjaAI_MoveDisappear::Exit() {
    m_ai->m_body->m_hidden = 0;
    m_ai->m_body->Appear();
}

void EnemyAIState_BkNinjaAI_AppearHome::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(1, true);
    AssignVector(m_ai->m_enemy->GetPosition(), &m_ai->m_enemy->field_0x48);
    *m_ai->m_enemy->GetRotation() = *m_ai->m_enemy->GetHomeRotation();
    m_ai->m_enemy->GetRotation()->y = fn_8019F6D4(&m_ai->m_target);
    m_ai->SetTimer(1.0f);
    m_ai->m_body->Disappear();
    m_ai->m_step = 0;
}

void EnemyAIState_BkNinjaAI_AppearHome::Update() {
    m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
    switch (m_ai->m_step) {
    case 0:
        m_ai->m_enemy->m_disp->vfDisp_0x50();
        m_ai->m_step++;
    case 1:
        if (m_ai->IsTimeUp()) {
            m_ai->m_enemy->m_status->vfStat_0x20(1, true);
            m_ai->m_body->Appear();
            m_ai->m_step++;
        }
        break;
    case 2:
        if (m_ai->m_enemy->m_status->vfStat_0x28()) {
            void *key = m_ai->vfCtrl_0x10();
            if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
                m_ai->vfAI_0x3C();
            } else {
                m_ai->vfAI_0x48();
            }
        }
        break;
    }
}

void EnemyAIState_BkNinjaAI_AppearHome::Exit() {}

void EnemyAIState_BkNinjaAI_GunAttack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    m_ai->SetTimer(1.0f);
    m_ai->m_step = 0;
    m_ai->m_shots = 0;
}

void EnemyAIState_BkNinjaAI_GunAttack::Update() {
    switch (m_ai->m_step) {
    case 0:
        m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
        if (fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target)) ||
            m_ai->IsTimeUp()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
            m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
            fn_8019A954(m_ai->m_enemy, 0x606A, 1);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            void *key = m_ai->vfCtrl_0x10();
            if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
                m_ai->vfAI_0x3C();
            } else if (++m_ai->m_shots >= m_ai->m_body->GetShootCount()) {
                m_ai->ChangeState(GetMoveDisappearState());
            } else if (std::fabs(fn_8019F698(&m_ai->m_target)) > DegToRad(10.0f)) {
                m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
                m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
                m_ai->SetTimer(1.0f);
                m_ai->m_step = 0;
            } else {
                m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
                m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
                fn_8019A954(m_ai->m_enemy, 0x606A, 1);
            }
        }
        break;
    }
}

void EnemyAIState_BkNinjaAI_GunAttack::Exit() {
    m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
}

} // namespace BkNinja
