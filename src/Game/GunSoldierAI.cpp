/* GunSoldier::GunSoldierAI (class name from RTTI): the G.U.N. soldier's AI (Game/GunSoldier.h), a
 * SoldierCommonAI (Game/EnemyAI.h). Like the Black Arms soldier's (Game/BkSoldierAI.cpp) it waits
 * standing or walking between its points, comes out and walks home (AppearHome), cautions, chases
 * the target and attacks from afar (Attack) or close by (NearAttack). After its radio contact it
 * thanks the player (Thanks) and keeps doing so until the target leaves; when it goes down
 * (ToHinshi, "hinshi" is near death) it lies there calling for help (Hinshi). File name inferred
 * from the class. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
/* See SoldierCommonAI and EnemyBaseAI (Game/EnemyAI.h). */
#define SOLDIER_COMMON_AI_STATES
#define ENEMY_BASE_AI_HELPERS

#include "Game/GunSoldier.h"
#include "Game/EnemyAIState.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp). */
namespace std {
float fabs(float x);
}

extern "C" {
/* Hold and release the state machine: the byte at 0xC is set and cleared (meaning inferred). */
void fn_8019F9F0(UnkAIStateMachine_8019F944 *machine);
void fn_8019F9FC(UnkAIStateMachine_8019F944 *machine);
/* Clears bit of a flag word (see Game/BKTankController.h). */
void fn_8006127C(u32 *flags, int bit);
/* Plays sound id for enemy (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, int id, int arg);
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* The defaults of slots the states call (Game/Enemy.h; inline, first emitted here). */
inline void EnemySetFunc::vfSet_0x18() {}

inline bool EnemyMoveFunc::vfMove_0x10() {
    return 0;
}

namespace GunSoldier {

/* The soldier's states (class names from RTTI; vtables 0x805300FC to 0x8053025C). */
class EnemyAIState_GunSoldierAI_AppearHome : public TEnemyAIState {
  public:
    EnemyAIState_GunSoldierAI_AppearHome();
    virtual ~EnemyAIState_GunSoldierAI_AppearHome() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunSoldierAI *m_ai;
};

class EnemyAIState_GunSoldierAI_GoHome : public TEnemyAIState {
  public:
    EnemyAIState_GunSoldierAI_GoHome();
    virtual ~EnemyAIState_GunSoldierAI_GoHome() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunSoldierAI *m_ai;
};

class EnemyAIState_GunSoldierAI_Attack : public TEnemyAIState {
  public:
    EnemyAIState_GunSoldierAI_Attack();
    virtual ~EnemyAIState_GunSoldierAI_Attack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunSoldierAI *m_ai;
};

class EnemyAIState_GunSoldierAI_NearAttack : public TEnemyAIState {
  public:
    EnemyAIState_GunSoldierAI_NearAttack();
    virtual ~EnemyAIState_GunSoldierAI_NearAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunSoldierAI *m_ai;
};

class EnemyAIState_GunSoldierAI_ChaseTarget : public TEnemyAIState {
  public:
    EnemyAIState_GunSoldierAI_ChaseTarget();
    virtual ~EnemyAIState_GunSoldierAI_ChaseTarget() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunSoldierAI *m_ai;
};

class EnemyAIState_GunSoldierAI_Caution : public TEnemyAIState {
  public:
    EnemyAIState_GunSoldierAI_Caution();
    virtual ~EnemyAIState_GunSoldierAI_Caution() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunSoldierAI *m_ai;
};

class EnemyAIState_GunSoldierAI_Thanks : public TEnemyAIState {
  public:
    EnemyAIState_GunSoldierAI_Thanks();
    virtual ~EnemyAIState_GunSoldierAI_Thanks() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunSoldierAI *m_ai;
};

class EnemyAIState_GunSoldierAI_Hinshi : public TEnemyAIState {
  public:
    EnemyAIState_GunSoldierAI_Hinshi();
    virtual ~EnemyAIState_GunSoldierAI_Hinshi() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunSoldierAI *m_ai;
};

class EnemyAIState_GunSoldierAI_ToHinshi : public TEnemyAIState {
  public:
    EnemyAIState_GunSoldierAI_ToHinshi();
    virtual ~EnemyAIState_GunSoldierAI_ToHinshi() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunSoldierAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetToHinshiState() {
    static EnemyAIState_GunSoldierAI_ToHinshi s_state;
    return &s_state;
}

static inline TEnemyAIState *GetHinshiState() {
    static EnemyAIState_GunSoldierAI_Hinshi s_state;
    return &s_state;
}

static inline TEnemyAIState *GetThanksState() {
    static EnemyAIState_GunSoldierAI_Thanks s_state;
    return &s_state;
}

static inline TEnemyAIState *GetCautionState() {
    static EnemyAIState_GunSoldierAI_Caution s_state;
    return &s_state;
}

static inline TEnemyAIState *GetChaseTargetState() {
    static EnemyAIState_GunSoldierAI_ChaseTarget s_state;
    return &s_state;
}

static inline TEnemyAIState *GetNearAttackState() {
    static EnemyAIState_GunSoldierAI_NearAttack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAttackState() {
    static EnemyAIState_GunSoldierAI_Attack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetGoHomeState() {
    static EnemyAIState_GunSoldierAI_GoHome s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAppearHomeState() {
    static EnemyAIState_GunSoldierAI_AppearHome s_state;
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

EnemyAIState_GunSoldierAI_ToHinshi::EnemyAIState_GunSoldierAI_ToHinshi() : m_ai(0) {}

void EnemyAIState_GunSoldierAI_ToHinshi::SetOwner(EnemyBaseAI *ai) {
    GunSoldierAI *owner = dynamic_cast<GunSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunSoldierAI_Hinshi::EnemyAIState_GunSoldierAI_Hinshi() : m_ai(0) {}

void EnemyAIState_GunSoldierAI_Hinshi::SetOwner(EnemyBaseAI *ai) {
    GunSoldierAI *owner = dynamic_cast<GunSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunSoldierAI_Thanks::EnemyAIState_GunSoldierAI_Thanks() : m_ai(0) {}

void EnemyAIState_GunSoldierAI_Thanks::SetOwner(EnemyBaseAI *ai) {
    GunSoldierAI *owner = dynamic_cast<GunSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunSoldierAI_Caution::EnemyAIState_GunSoldierAI_Caution() : m_ai(0) {}

void EnemyAIState_GunSoldierAI_Caution::SetOwner(EnemyBaseAI *ai) {
    GunSoldierAI *owner = dynamic_cast<GunSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunSoldierAI_ChaseTarget::EnemyAIState_GunSoldierAI_ChaseTarget() : m_ai(0) {}

void EnemyAIState_GunSoldierAI_ChaseTarget::SetOwner(EnemyBaseAI *ai) {
    GunSoldierAI *owner = dynamic_cast<GunSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunSoldierAI_NearAttack::EnemyAIState_GunSoldierAI_NearAttack() : m_ai(0) {}

void EnemyAIState_GunSoldierAI_NearAttack::SetOwner(EnemyBaseAI *ai) {
    GunSoldierAI *owner = dynamic_cast<GunSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunSoldierAI_Attack::EnemyAIState_GunSoldierAI_Attack() : m_ai(0) {}

void EnemyAIState_GunSoldierAI_Attack::SetOwner(EnemyBaseAI *ai) {
    GunSoldierAI *owner = dynamic_cast<GunSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunSoldierAI_GoHome::EnemyAIState_GunSoldierAI_GoHome() : m_ai(0) {}

void EnemyAIState_GunSoldierAI_GoHome::SetOwner(EnemyBaseAI *ai) {
    GunSoldierAI *owner = dynamic_cast<GunSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunSoldierAI_AppearHome::EnemyAIState_GunSoldierAI_AppearHome() : m_ai(0) {}

void EnemyAIState_GunSoldierAI_AppearHome::SetOwner(EnemyBaseAI *ai) {
    GunSoldierAI *owner = dynamic_cast<GunSoldierAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

GunSoldierAI::GunSoldierAI() {
    m_body = dynamic_cast<GunSoldierBase *>(m_enemy->m_disp);
}

void GunSoldierAI::vfAI_0x38() {
    if (m_body->m_contacted) {
        ChangeState(::GunSoldier::GetThanksState());
        return;
    }
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
        ChangeState(::GunSoldier::GetAppearHomeState());
        break;
    case 5:
        ChangeState(::GunSoldier::GetHinshiState());
        break;
    }
}

void GunSoldierAI::vfAI_0x58() {
    ChangeState(::GunSoldier::GetToHinshiState());
}

void GunSoldierAI::vfAI_0x3C() {
    if (m_body->m_contacted) {
        ChangeState(::GunSoldier::GetThanksState());
        return;
    }
    switch (m_body->m_appearType) {
    case 0:
    case 4:
        fn_8019F944(&m_stateMachine, GetStandingState());
        break;
    case 1:
    case 2:
    case 3:
        fn_8019F944(&m_stateMachine, GetMovingState());
        break;
    case 5:
        ChangeState(::GunSoldier::GetHinshiState());
        break;
    }
}

void GunSoldierAI::vfAI_0x60() {
    m_body->m_contacted = 1;
    fn_8019F9F0(&m_stateMachine);
    ChangeState(::GunSoldier::GetThanksState());
}

void GunSoldierAI::vfAI_0x44() {
    ChangeState(::GunSoldier::GetCautionState());
}

void GunSoldierAI::StartAttack() {
    int weapon = m_body->GetWeaponType();
    if (weapon == 0 || weapon == 1) {
        ChangeState(::GunSoldier::GetChaseTargetState());
    } else {
        ChangeState(::GunSoldier::GetAttackState());
    }
}

/* Not original: an unused function the linker drops. Retail has the weak copy of the appear
 * state's getter here, so the original called it from code that was dropped too; the call puts
 * the copy in the same place. */
void GunSoldierAI_UnusedAppearHome(GunSoldierAI *ai) {
    ai->ChangeState(GetAppearHomeState());
}

void EnemyAIState_GunSoldierAI_ToHinshi::Enter() {
    fn_8019F9FC(&m_ai->m_stateMachine);
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x60(), true);
}

void EnemyAIState_GunSoldierAI_ToHinshi::Update() {
    if (m_ai->m_enemy->m_move->vfMove_0x10()) {
        m_ai->m_enemy->m_set->vfSet_0x18();
        m_ai->m_enemy->m_set->vfSet_0x10();
    } else if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        fn_8019F9F0(&m_ai->m_stateMachine);
        m_ai->ChangeState(GetHinshiState());
    }
}

void EnemyAIState_GunSoldierAI_ToHinshi::Exit() {
    fn_8019F9F0(&m_ai->m_stateMachine);
}

void EnemyAIState_GunSoldierAI_Hinshi::Enter() {
    fn_8019F9FC(&m_ai->m_stateMachine);
    m_ai->m_body->fn_801972A8();
    m_ai->m_enemy->m_status->vfStat_0x80(false);
    m_ai->m_enemy->m_status->vfStat_0x7C(false, true);
    if (m_ai->m_body->field_0x357) {
        m_ai->m_enemy->m_status->vfStat_0x20(9, true);
    } else {
        m_ai->m_enemy->m_status->vfStat_0x20(10, true);
    }
    m_ai->SetTimer(fn_8040EC80(30.0f, 60.0f));
}

void EnemyAIState_GunSoldierAI_Hinshi::Update() {
    m_ai->m_enemy->m_status->vfStat_0x7C(false, true);
    if (m_ai->IsTimeUp()) {
        m_ai->SetTimer(fn_8040EC80(30.0f, 60.0f));
        fn_8019AA40(m_ai->m_enemy, 0x600F, 1);
    }
}

void EnemyAIState_GunSoldierAI_Hinshi::Exit() {
    fn_8019F9F0(&m_ai->m_stateMachine);
    m_ai->m_enemy->m_status->vfStat_0x7C(true, true);
    m_ai->m_enemy->m_status->vfStat_0x80(true);
    m_ai->m_body->fn_80197280();
}

void EnemyAIState_GunSoldierAI_Thanks::Enter() {
    m_ai->m_enemy->m_status->vfStat_0xA8()->Set(13);
    m_ai->m_enemy->m_status->vfStat_0x7C(false, true);
    fn_8019F9FC(&m_ai->m_stateMachine);
    m_ai->m_enemy->m_status->vfStat_0x20(13, true);
    m_ai->m_step = 0;
}

void EnemyAIState_GunSoldierAI_Thanks::Update() {
    m_ai->m_enemy->m_status->vfStat_0x7C(false, true);
    fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(14, true);
            m_ai->m_step = 1;
        }
        break;
    case 1: {
        void *key = m_ai->vfCtrl_0x10();
        if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
            m_ai->SetTimer(fn_8040EC80(5.0f, 15.0f));
            m_ai->m_step = 2;
        }
        break;
    }
    case 2: {
        void *key = m_ai->vfCtrl_0x10();
        if (m_ai->m_enemy->field_0xA8.Contains(key) || m_ai->IsTimeUp()) {
            m_ai->m_enemy->m_status->vfStat_0x20(13, true);
            m_ai->m_step = 0;
        }
        break;
    }
    }
    if (m_ai->m_enemy->m_status->vfStat_0x4C()) {
        fn_8019F9F0(&m_ai->m_stateMachine);
        m_ai->ChangeState(GetChaseTargetState());
    }
}

void EnemyAIState_GunSoldierAI_Thanks::Exit() {
    fn_8019F9F0(&m_ai->m_stateMachine);
    fn_8006127C(&m_ai->m_enemy->m_status->vfStat_0xA8()->m_bits, 13);
    m_ai->m_enemy->m_status->vfStat_0x7C(true, true);
}

void EnemyAIState_GunSoldierAI_Caution::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x54(), true);
}

void EnemyAIState_GunSoldierAI_Caution::Update() {
    fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    int weapon = m_ai->m_body->GetWeaponType();
    if (weapon == 0 || weapon == 1) {
        m_ai->ChangeState(GetChaseTargetState());
    } else if (m_ai->m_enemy->m_status->vfStat_0x28()) {
        m_ai->StartAttack();
    }
}

void EnemyAIState_GunSoldierAI_Caution::Exit() {}

void EnemyAIState_GunSoldierAI_ChaseTarget::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
}

void EnemyAIState_GunSoldierAI_ChaseTarget::Update() {
    fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->ChangeState(GetGoHomeState());
    } else if (fn_8019F710(&m_ai->m_target) < 15.0f) {
        m_ai->ChangeState(GetNearAttackState());
    }
}

void EnemyAIState_GunSoldierAI_ChaseTarget::Exit() {}

void EnemyAIState_GunSoldierAI_NearAttack::Enter() {
    m_ai->m_body->field_0x344 = RandomBit();
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
}

void EnemyAIState_GunSoldierAI_NearAttack::Update() {
    fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->ChangeState(GetGoHomeState());
    } else if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        if (fn_8019F710(&m_ai->m_target) > 20.0f) {
            m_ai->ChangeState(GetChaseTargetState());
        } else {
            m_ai->m_body->field_0x344 = RandomBit();
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
        }
    }
}

void EnemyAIState_GunSoldierAI_NearAttack::Exit() {}

void EnemyAIState_GunSoldierAI_Attack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
    m_ai->m_body->field_0x358 = 0;
    m_ai->SetTimer(1.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_GunSoldierAI_Attack::Update() {
    switch (m_ai->m_step) {
    case 0:
        m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
        if (fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target)) ||
            m_ai->IsTimeUp()) {
            m_ai->m_body->field_0x344 = RandomBit();
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
            m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
            m_ai->SetTimer(3.0f);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->IsTimeUp() ||
            (m_ai->m_enemy->m_status->vfStat_0x28() && IsFacingAway(m_ai, 30.0f))) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
            m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
            m_ai->m_body->field_0x358 = 0;
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

void EnemyAIState_GunSoldierAI_Attack::Exit() {
    m_ai->m_body->field_0x358 = 0;
    m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
}

void EnemyAIState_GunSoldierAI_GoHome::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
}

void EnemyAIState_GunSoldierAI_GoHome::Update() {
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

void EnemyAIState_GunSoldierAI_GoHome::Exit() {}

void EnemyAIState_GunSoldierAI_AppearHome::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
}

void EnemyAIState_GunSoldierAI_AppearHome::Update() {
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                &m_ai->m_enemy->field_0x6C, m_ai->m_enemy->GetPosition()));
    fn_8019F53C(&m_ai->m_turn, &direction);
    direction.y = 0.0f;
    if (fn_8040CF50(&direction) < 10.0f) {
        m_ai->StartAttack();
    }
}

void EnemyAIState_GunSoldierAI_AppearHome::Exit() {}

} // namespace GunSoldier
