/* BkChaos::BkChaosAI (class name from RTTI): the Black Arms chaos's AI (Game/BkChaos.h). While its
 * chibis are apart it waits (WaitChibi); once they have combined it waits at home (Wait), turns to a
 * target in its area (Caution) and attacks: by rolling into it when it is close (AttackRolling),
 * else by stretching toward it (AttackStrech, sic). File name inferred from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
/* See EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_HELPERS

#include "Game/BkChaos.h"
#include "Game/EnemyAIState.h"
#include "Game/MathTypes.h"

extern "C" {
/* Whether the enemy sees a target (behaviour inferred). */
bool fn_8019AF04(Enemy *enemy);
/* Turns the enemy toward position, and toward rotation (behaviours inferred). */
void fn_8019B0B0(Enemy *enemy, const Vec3f *position);
void fn_8019B0F8(Enemy *enemy, const Vec3f *rotation);
}

namespace BkChaos {

/* The chaos's states (class names from RTTI; vtables 0x8055756C to 0x8055761C). */
class EnemyAIState_BkChaosAI_AttackRolling : public TEnemyAIState {
  public:
    EnemyAIState_BkChaosAI_AttackRolling();
    virtual ~EnemyAIState_BkChaosAI_AttackRolling() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkChaosAI *m_ai;
};

class EnemyAIState_BkChaosAI_AttackStrech : public TEnemyAIState {
  public:
    EnemyAIState_BkChaosAI_AttackStrech();
    virtual ~EnemyAIState_BkChaosAI_AttackStrech() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkChaosAI *m_ai;
};

class EnemyAIState_BkChaosAI_Caution : public TEnemyAIState {
  public:
    EnemyAIState_BkChaosAI_Caution();
    virtual ~EnemyAIState_BkChaosAI_Caution() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkChaosAI *m_ai;
};

class EnemyAIState_BkChaosAI_Wait : public TEnemyAIState {
  public:
    EnemyAIState_BkChaosAI_Wait();
    virtual ~EnemyAIState_BkChaosAI_Wait() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkChaosAI *m_ai;
};

class EnemyAIState_BkChaosAI_WaitChibi : public TEnemyAIState {
  public:
    EnemyAIState_BkChaosAI_WaitChibi();
    virtual ~EnemyAIState_BkChaosAI_WaitChibi() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ BkChaosAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetWaitChibiState() {
    static EnemyAIState_BkChaosAI_WaitChibi s_state;
    return &s_state;
}

static inline TEnemyAIState *GetWaitState() {
    static EnemyAIState_BkChaosAI_Wait s_state;
    return &s_state;
}

static inline TEnemyAIState *GetCautionState() {
    static EnemyAIState_BkChaosAI_Caution s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAttackStrechState() {
    static EnemyAIState_BkChaosAI_AttackStrech s_state;
    return &s_state;
}

static inline TEnemyAIState *GetAttackRollingState() {
    static EnemyAIState_BkChaosAI_AttackRolling s_state;
    return &s_state;
}

EnemyAIState_BkChaosAI_WaitChibi::EnemyAIState_BkChaosAI_WaitChibi() : m_ai(0) {}

void EnemyAIState_BkChaosAI_WaitChibi::SetOwner(EnemyBaseAI *ai) {
    BkChaosAI *owner = dynamic_cast<BkChaosAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkChaosAI_Wait::EnemyAIState_BkChaosAI_Wait() : m_ai(0) {}

void EnemyAIState_BkChaosAI_Wait::SetOwner(EnemyBaseAI *ai) {
    BkChaosAI *owner = dynamic_cast<BkChaosAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkChaosAI_Caution::EnemyAIState_BkChaosAI_Caution() : m_ai(0) {}

void EnemyAIState_BkChaosAI_Caution::SetOwner(EnemyBaseAI *ai) {
    BkChaosAI *owner = dynamic_cast<BkChaosAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkChaosAI_AttackStrech::EnemyAIState_BkChaosAI_AttackStrech() : m_ai(0) {}

void EnemyAIState_BkChaosAI_AttackStrech::SetOwner(EnemyBaseAI *ai) {
    BkChaosAI *owner = dynamic_cast<BkChaosAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_BkChaosAI_AttackRolling::EnemyAIState_BkChaosAI_AttackRolling() : m_ai(0) {}

void EnemyAIState_BkChaosAI_AttackRolling::SetOwner(EnemyBaseAI *ai) {
    BkChaosAI *owner = dynamic_cast<BkChaosAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

BkChaosAI::BkChaosAI() {
    m_body = dynamic_cast<BkChaos *>(m_enemy->m_disp);
}

BkChaosAI::~BkChaosAI() {}

} // namespace BkChaos

/* Slot 0x28, the handler of the messages sent to the enemy. It takes the message, which
 * Game/EnemyAI.h does not declare without ENEMY_AI_MESSAGE_ARG (that macro gives the slot another
 * symbol), so it is defined under the slot's symbol, as Game/EggShadowAndroidAI.cpp does: message
 * 0x14 makes the chaos gather its chibis and let them go again. */
extern "C" void vfAI_0x28__11EnemyBaseAIFv(EnemyBaseAI *ai, UnkEnemyMessage_8019DC9C *message);

extern "C" void vfAI_0x28__Q27BkChaos9BkChaosAIFv(BkChaos::BkChaosAI *ai,
                                                   UnkEnemyMessage_8019DC9C *message) {
    switch (message->m_id) {
    case 0x14:
        ai->m_body->Gather();
        ai->m_body->ReleaseChibis();
        message->m_flags |= 2;
    }
    if (!(message->m_flags & 2)) {
        vfAI_0x28__11EnemyBaseAIFv(ai, message);
    }
}

namespace BkChaos {

void BkChaosAI::vfAI_0x38() {
    if (m_body->m_state == 2) {
        ChangeState(GetWaitState());
    } else {
        ChangeState(GetWaitChibiState());
    }
}

void BkChaosAI::vfAI_0x3C() {
    vfAI_0x38();
}

void BkChaosAI::vfAI_0x48() {
    if (m_body->IsTargetNear()) {
        ChangeState(GetAttackRollingState());
    } else {
        ChangeState(GetAttackStrechState());
    }
}

void BkChaosAI::vfAI_0x44() {
    ChangeState(GetCautionState());
}

void BkChaosAI::vfAI_0x58() {
    m_enemy->m_disp->vfDisp_0x4C();
    m_enemy->m_set->vfSet_0x10();
}

void EnemyAIState_BkChaosAI_WaitChibi::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
}

void EnemyAIState_BkChaosAI_WaitChibi::Update() {
    if (m_ai->m_body->m_state == 2) {
        m_ai->ChangeState(GetWaitState());
        return;
    }
    void *key = m_ai->vfCtrl_0x10();
    if (m_ai->m_enemy->field_0xA8.Contains(key)) {
        fn_8019B0B0(m_ai->m_enemy, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    } else {
        fn_8019B0F8(m_ai->m_enemy, &m_ai->m_enemy->field_0x60);
    }
    m_ai->m_enemy->vf_0x10();
}

void EnemyAIState_BkChaosAI_WaitChibi::Exit() {}

void EnemyAIState_BkChaosAI_Wait::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
    m_ai->m_step = 0;
}

void EnemyAIState_BkChaosAI_Wait::Update() {
    if (fn_8019AF04(m_ai->m_enemy)) {
        m_ai->vfAI_0x44();
        return;
    }
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_enemy->vf_0x0C()) {
            fn_8019B0F8(m_ai->m_enemy, &m_ai->m_enemy->field_0x60);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->m_enemy->vf_0x10()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
            m_ai->m_step++;
        }
        break;
    case 2:
        break;
    }
}

void EnemyAIState_BkChaosAI_Wait::Exit() {}

void EnemyAIState_BkChaosAI_Caution::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    fn_8019B0B0(m_ai->m_enemy, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
    m_ai->m_step = 0;
}

void EnemyAIState_BkChaosAI_Caution::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_enemy->vf_0x10()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x54(), true);
            m_ai->SetTimer(1.0f);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->m_enemy->m_status->vfStat_0x2C() || m_ai->IsTimeUp()) {
            if (m_ai->m_body->m_combined > 0) {
                m_ai->vfAI_0x48();
            }
            return;
        }
        break;
    }
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->ChangeState(GetWaitState());
    }
}

void EnemyAIState_BkChaosAI_Caution::Exit() {}

void EnemyAIState_BkChaosAI_AttackStrech::Enter() {
    m_ai->SetTimer(3.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_BkChaosAI_AttackStrech::Update() {
    if (m_ai->m_body->m_combined <= 0) {
        m_ai->ChangeState(GetCautionState());
        return;
    }
    switch (m_ai->m_step) {
    case 0:
        fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f));
        if (m_ai->m_enemy->vf_0x10() || m_ai->IsTimeUp()) {
            m_ai->m_enemy->m_status->vfStat_0x20(3, true);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->ChangeState(GetWaitState());
        }
        break;
    }
}

void EnemyAIState_BkChaosAI_AttackStrech::Exit() {}

void EnemyAIState_BkChaosAI_AttackRolling::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(2, true);
}

void EnemyAIState_BkChaosAI_AttackRolling::Update() {
    if (m_ai->m_enemy->m_status->vfStat_0x2C() || m_ai->m_body->m_combined <= 0) {
        m_ai->ChangeState(GetCautionState());
    }
}

void EnemyAIState_BkChaosAI_AttackRolling::Exit() {}

} // namespace BkChaos
