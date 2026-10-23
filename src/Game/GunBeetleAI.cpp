/* GunBeetleAI (class name from RTTI): the Gun Beetle's AI (Game/GunBeetle.h), a FlyerCommonAI
 * (Game/EnemyFlyer.h). It follows its path or floats (FlyerCommonAI's states); with its target in
 * the area it aims a moment (Caution) and attacks (Attack). File name inferred from the class.
 * Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* See FlyerCommonAI (Game/EnemyFlyer.h) and EnemyBaseAI (Game/EnemyAI.h). */
#define FLYER_COMMON_AI_STATES
#define ENEMY_BASE_AI_HELPERS

#include "Game/GunBeetle.h"
#include "Game/EnemyAIState.h"
#include "Game/MathTypes.h"

extern "C" {
/* Turns the enemy a step toward position (behaviour inferred; see fn_8019F53C). */
bool fn_8019F2E0(UnkAITurn_8019F53C *turn, const Vec3f *position);
}

/* Inline in the original: the weak copy is first emitted here (see Game/GunBeetle.h). */
inline bool GunBeetleBase::vf_0x238() {
    return m_params.AppearType == 1;
}

/* The Beetle's states (class names from RTTI; vtables 0x8052F3CC and 0x8052F3F8). */
class EnemyAIState_GunBeetleAI_Attack : public TEnemyAIState {
  public:
    EnemyAIState_GunBeetleAI_Attack();
    virtual ~EnemyAIState_GunBeetleAI_Attack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunBeetleAI *m_ai;
};

class EnemyAIState_GunBeetleAI_Caution : public TEnemyAIState {
  public:
    EnemyAIState_GunBeetleAI_Caution();
    virtual ~EnemyAIState_GunBeetleAI_Caution() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunBeetleAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetCautionState() {
    static EnemyAIState_GunBeetleAI_Caution s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAttackState() {
    static EnemyAIState_GunBeetleAI_Attack s_state;
    return &s_state;
}

EnemyAIState_GunBeetleAI_Caution::EnemyAIState_GunBeetleAI_Caution() : m_ai(0) {}

void EnemyAIState_GunBeetleAI_Caution::SetOwner(EnemyBaseAI *ai) {
    GunBeetleAI *owner = dynamic_cast<GunBeetleAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunBeetleAI_Attack::EnemyAIState_GunBeetleAI_Attack() : m_ai(0) {}

void EnemyAIState_GunBeetleAI_Attack::SetOwner(EnemyBaseAI *ai) {
    GunBeetleAI *owner = dynamic_cast<GunBeetleAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

GunBeetleAI::GunBeetleAI() {
    m_body = dynamic_cast<GunBeetleBase *>(m_enemy->m_disp);
}

void GunBeetleAI::vfAI_0x3C() {
    if (m_body->vf_0x238()) {
        fn_8019F944(&m_stateMachine, GetMoveOnPathState());
    } else {
        fn_8019F944(&m_stateMachine, GetWaitFloatingState());
    }
}

void GunBeetleAI::vfAI_0x44() {
    ChangeState(::GetCautionState());
}

void EnemyAIState_GunBeetleAI_Caution::Enter() {
    m_ai->SetTimer(1.0f);
}

void EnemyAIState_GunBeetleAI_Caution::Update() {
    Vec3f target;
    fn_800091EC(&target, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    target.y += 10.0f;
    fn_8019F2E0(&m_ai->m_turn, &target);
    if (m_ai->IsTimeUp()) {
        m_ai->ChangeState(GetAttackState());
    }
}

void EnemyAIState_GunBeetleAI_Caution::Exit() {}

void EnemyAIState_GunBeetleAI_Attack::Enter() {
    m_ai->SetTimer(3.0f);
    m_ai->m_step = 0;
    m_ai->m_body->vf_0x244();
}

void EnemyAIState_GunBeetleAI_Attack::Update() {
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        fn_8019F944(&m_ai->m_stateMachine, FlyerCommonAI::GetWaitFloatingState());
    } else {
        Vec3f target;
        fn_800091EC(&target, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
        target.y += 10.0f;
        fn_8019F2E0(&m_ai->m_turn, &target);
        if (m_ai->IsTimeUp()) {
            m_ai->ChangeState(GetCautionState());
        }
    }
}

void EnemyAIState_GunBeetleAI_Attack::Exit() {
    m_ai->m_body->vf_0x248();
}
