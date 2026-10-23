/* BkWingLargeAI (class name from RTTI): the large Black Arms wing's AI (Game/BkWingLarge.h), a
 * FlyerCommonAI (Game/EnemyFlyer.h). It follows its path or floats (FlyerCommonAI's states),
 * cautions and attacks, resists the vacuum gun (ResistVacuum) and, when shot down with an air
 * wing on board, turns level and spawns it (DeadToAirWing). File name inferred from the class.
 * Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* See FlyerCommonAI (Game/EnemyFlyer.h) and EnemyBaseAI (Game/EnemyAI.h). */
#define FLYER_COMMON_AI_STATES
#define ENEMY_AI_MESSAGE_ARG
#define ENEMY_BASE_AI_HELPERS

#include "Game/BkWingLarge.h"
#include "Game/EnemyAIState.h"
#include "Game/EnemyRender.h"
#include "Game/MathTypes.h"

extern "C" {
/* Turns the enemy a step toward position (behaviour inferred; see fn_8019F53C). */
bool fn_8019F2E0(UnkAITurn_8019F53C *turn, const Vec3f *position);
/* Moves angle toward target by a step (behaviour inferred). */
float fn_8040F314(float angle, float target, float step);
/* -v (fn_80016284 stores the result through its first argument). */
Vec3f fn_80016284(const Vec3f *v);
/* Plays sound id for enemy (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, int id, int arg);
void fn_802CD21C(EnemyRender *render, float speed);
/* The objects the wing spawns (class unknown; behaviour inferred). */
void *fn_801B48D4(int kind);
void fn_801B5D24(void *object, const Vec3f *position, const Vec3f *rotation);
void fn_801B5CE8(void *object);
void fn_801B5D1C(void *object, float value);
float fn_800C9938(UnkPlacement_800C9CD0 *placement);
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const Vec3f *position, int arg3, int arg4);
}

/* The parameters' default for slot 0x60 (Game/Enemy.h; inline, first emitted here). */
inline int EnemyParaFunc::vfPara_0x60() {
    return -1;
}

/* The do-nothing defaults of slots the states call (Game/Enemy.h; inline, first emitted here). */
inline void EnemySetFunc::vfSet_0x10() {}

inline boost::shared_ptr<EnemyRender> EnemyDispFunc::vfDisp_0x20() {
    return boost::shared_ptr<EnemyRender>();
}

inline void EnemyDispFunc::vfDisp_0x50() {}

/* The large wing's states (class names from RTTI; vtables 0x8052E214 to 0x8052E298). */
class EnemyAIState_BkWingLargeAI_DeadToAirWing : public TEnemyAIState {
  public:
    EnemyAIState_BkWingLargeAI_DeadToAirWing();
    virtual ~EnemyAIState_BkWingLargeAI_DeadToAirWing() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWingLargeAI *m_ai;
};

class EnemyAIState_BkWingLargeAI_Attack : public TEnemyAIState {
  public:
    EnemyAIState_BkWingLargeAI_Attack();
    virtual ~EnemyAIState_BkWingLargeAI_Attack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWingLargeAI *m_ai;
};

class EnemyAIState_BkWingLargeAI_Caution : public TEnemyAIState {
  public:
    EnemyAIState_BkWingLargeAI_Caution();
    virtual ~EnemyAIState_BkWingLargeAI_Caution() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWingLargeAI *m_ai;
};

class EnemyAIState_BkWingLargeAI_ResistVacuum : public TEnemyAIState {
  public:
    EnemyAIState_BkWingLargeAI_ResistVacuum();
    virtual ~EnemyAIState_BkWingLargeAI_ResistVacuum() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkWingLargeAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetResistVacuumState() {
    static EnemyAIState_BkWingLargeAI_ResistVacuum s_state;
    return &s_state;
}

static inline TEnemyAIState *GetCautionState() {
    static EnemyAIState_BkWingLargeAI_Caution s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAttackState() {
    static EnemyAIState_BkWingLargeAI_Attack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetDeadToAirWingState() {
    static EnemyAIState_BkWingLargeAI_DeadToAirWing s_state;
    return &s_state;
}

EnemyAIState_BkWingLargeAI_ResistVacuum::EnemyAIState_BkWingLargeAI_ResistVacuum() : m_ai(0) {}

void EnemyAIState_BkWingLargeAI_ResistVacuum::SetOwner(EnemyBaseAI *ai) {
    BkWingLargeAI *owner = dynamic_cast<BkWingLargeAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkWingLargeAI_Caution::EnemyAIState_BkWingLargeAI_Caution() : m_ai(0) {}

void EnemyAIState_BkWingLargeAI_Caution::SetOwner(EnemyBaseAI *ai) {
    BkWingLargeAI *owner = dynamic_cast<BkWingLargeAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkWingLargeAI_Attack::EnemyAIState_BkWingLargeAI_Attack() : m_ai(0) {}

void EnemyAIState_BkWingLargeAI_Attack::SetOwner(EnemyBaseAI *ai) {
    BkWingLargeAI *owner = dynamic_cast<BkWingLargeAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkWingLargeAI_DeadToAirWing::EnemyAIState_BkWingLargeAI_DeadToAirWing() : m_ai(0) {}

void EnemyAIState_BkWingLargeAI_DeadToAirWing::SetOwner(EnemyBaseAI *ai) {
    BkWingLargeAI *owner = dynamic_cast<BkWingLargeAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

BkWingLargeAI::BkWingLargeAI() {
    m_body = dynamic_cast<BkWingLargeBase *>(m_enemy->m_disp);
}

void BkWingLargeAI::vfAI_0x28(UnkEnemyMessage_8019DC9C *message) {
    switch (message->m_id) {
    case 0x14:
        if (!m_body->vf_0x238()) {
            ChangeState(GetResistVacuumState());
            message->m_flags |= 2;
            break;
        }
    default:
        EnemyBaseAI::vfAI_0x28(message);
        break;
    }
}

void BkWingLargeAI::vfAI_0x3C() {
    if (m_body->vf_0x238()) {
        fn_8019F944(&m_stateMachine, GetMoveOnPathState());
    } else {
        fn_8019F944(&m_stateMachine, GetWaitFloatingState());
    }
}

void BkWingLargeAI::vfAI_0x44() {
    if (m_body->vf_0x23C()) {
        ChangeState(::GetCautionState());
    }
}

void BkWingLargeAI::vfAI_0x58() {
    if (m_body->m_params.Type & 0xF) {
        ChangeState(GetDeadToAirWingState());
    } else {
        FlyerCommonAI::vfAI_0x58();
    }
}

void BkWingLargeAI::SpawnAirWing() {
    int kind = 8;
    if ((m_body->m_params.Type >> 4) & 0xF) {
        kind = 9;
    }
    void *object = fn_801B48D4(kind);
    if (object != 0) {
        fn_801B5D24(object, m_enemy->GetPosition(), m_enemy->GetRotation());
        fn_801B5CE8(object);
        UnkPlacement_800C9CD0 *placement =
            reinterpret_cast<UnkPlacement_800C9CD0 *>(m_enemy->m_set->vfSet_0x2C());
        if (placement != 0) {
            fn_801B5D1C(object, fn_800C9938(placement));
        } else {
            fn_801B5D1C(object, 500.0f);
        }
        fn_801D54F4(fn_80053164(), 0x6045, m_enemy->GetPosition(), 0, 1);
        m_enemy->m_disp->vfDisp_0x50();
    }
}

void EnemyAIState_BkWingLargeAI_ResistVacuum::Enter() {
    fn_802CD21C(m_ai->m_enemy->m_disp->vfDisp_0x20().get(), 3.0f);
    if (m_ai->m_enemy->m_status->vfStat_0x24() != 3) {
        m_ai->m_enemy->m_status->vfStat_0x20(3, true);
    }
    m_ai->SetTimer(1.0f);
}

void EnemyAIState_BkWingLargeAI_ResistVacuum::Update() {
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_80016284((const Vec3f *)fn_8019F65C(&m_ai->m_target)));
    fn_8019F2E0(&m_ai->m_turn, &direction);
    if (m_ai->IsTimeUp()) {
        m_ai->vfAI_0x3C();
    }
}

void EnemyAIState_BkWingLargeAI_ResistVacuum::Exit() {
    fn_802CD21C(m_ai->m_enemy->m_disp->vfDisp_0x20().get(), 1.0f);
}

void EnemyAIState_BkWingLargeAI_Caution::Enter() {
    m_ai->SetTimer(1.0f);
}

void EnemyAIState_BkWingLargeAI_Caution::Update() {
    Vec3f target;
    fn_800091EC(&target, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    target.y += 10.0f;
    fn_8019F2E0(&m_ai->m_turn, &target);
    if (m_ai->IsTimeUp()) {
        m_ai->ChangeState(GetAttackState());
    }
}

void EnemyAIState_BkWingLargeAI_Caution::Exit() {}

void EnemyAIState_BkWingLargeAI_Attack::Enter() {
    m_ai->m_body->vf_0x244();
    fn_8019AA40(m_ai->m_enemy, 0x6044, 1);
}

void EnemyAIState_BkWingLargeAI_Attack::Update() {
    Vec3f target;
    fn_800091EC(&target, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    target.y += 5.0f;
    fn_8019F2E0(&m_ai->m_turn, &target);
    if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        void *key = m_ai->vfCtrl_0x10();
        if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
            m_ai->vfAI_0x3C();
        } else {
            m_ai->ChangeState(GetCautionState());
        }
    }
}

void EnemyAIState_BkWingLargeAI_Attack::Exit() {
    m_ai->m_body->vf_0x248();
}

void EnemyAIState_BkWingLargeAI_DeadToAirWing::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x60(), true);
}

void EnemyAIState_BkWingLargeAI_DeadToAirWing::Update() {
    m_ai->m_enemy->GetRotation()->x = fn_8040F314(m_ai->m_enemy->GetRotation()->x, 0.0f, 0.5f);
    m_ai->m_enemy->GetRotation()->z = fn_8040F314(m_ai->m_enemy->GetRotation()->z, 0.0f, 0.5f);
    if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        m_ai->SpawnAirWing();
        m_ai->m_enemy->m_set->vfSet_0x10();
    }
}

void EnemyAIState_BkWingLargeAI_DeadToAirWing::Exit() {}
