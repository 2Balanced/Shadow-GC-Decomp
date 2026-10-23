/* GunRobotAI (class name from RTTI): the Gun Robot's AI (Game/GunRobot.h), an EnemyWalkerAI
 * (Game/EnemyWalkerAI.h). It waits standing or walking between its points or returns home
 * (EnemyWalkerAI's states), cautions (EnemyBaseAI's state) and fires its weapon (WeaponAttack).
 * File name inferred from the class. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). The
 * records the walker states register their objects with sit after a block of unused ones in
 * retail's .bss, which GC/2.7 does not reproduce (see Game/EggPawnAI.cpp). */
/* See EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_STATES

#include "Game/GunRobot.h"
#include "Game/EnemyAIState.h"
#include "Game/EnemyWalkerAI.h"
#include "Game/MathTypes.h"

extern "C" {
/* A point dist ahead of enemy toward position (behaviour inferred). */
Vec3f fn_8019AADC(Enemy *enemy, const Vec3f *position, float dist);
/* Turns enemy toward position at once (behaviour inferred). */
void fn_8019B0B0(Enemy *enemy, const Vec3f *position);
}

/* The Robot's attack state (class name from RTTI; vtable 0x8052FC94). */
class EnemyAIState_GunRobotAI_WeaponAttack : public TEnemyAIState {
  public:
    EnemyAIState_GunRobotAI_WeaponAttack();
    virtual ~EnemyAIState_GunRobotAI_WeaponAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunRobotAI *m_ai;
};

/* The state's object (name inferred). Inline, and the AI switches to it through
 * EnemyBaseAI::ChangeState, so it is called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetWeaponAttackState() {
    static EnemyAIState_GunRobotAI_WeaponAttack s_state;
    return &s_state;
}

EnemyAIState_GunRobotAI_WeaponAttack::EnemyAIState_GunRobotAI_WeaponAttack() : m_ai(0) {}

void EnemyAIState_GunRobotAI_WeaponAttack::SetOwner(EnemyBaseAI *ai) {
    GunRobotAI *owner = dynamic_cast<GunRobotAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

GunRobotAI::GunRobotAI() {
    m_body = dynamic_cast<GunRobot *>(m_enemy->m_disp);
}

void GunRobotAI::vfAI_0x38() {
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
    case 3:
        fn_8019F944(&m_stateMachine, GetReturnHomeState());
        break;
    default:
        fn_8019F944(&m_stateMachine, GetStandingState());
        break;
    }
}

void GunRobotAI::vfAI_0x3C() {
    if (m_body->vf_0x250() == 0) {
        fn_8019F944(&m_stateMachine, GetStandingState());
    } else {
        fn_8019F944(&m_stateMachine, GetMovingState());
    }
}

void GunRobotAI::vfAI_0x44() {
    fn_8019F944(&m_stateMachine, EnemyBaseAI::GetCautionState());
}

void GunRobotAI::vfAI_0x48() {
    ChangeState(GetWeaponAttackState());
}

void GunRobotAI::vfAI_0x58() {
    m_enemy->m_disp->vfDisp_0x4C();
    m_enemy->m_set->vfSet_0x10();
}

void EnemyAIState_GunRobotAI_WeaponAttack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vfPara_0x58(), true);
    m_ai->m_step = 0;
}

void EnemyAIState_GunRobotAI_WeaponAttack::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vfPara_0x4C(), true);
            fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)fn_8019AADC(m_ai->m_body,
                                                                    m_ai->m_enemy->GetPosition(),
                                                                    30.0f));
            m_ai->m_step++;
        }
        break;
    case 1: {
        void *key = m_ai->vfCtrl_0x10();
        if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
            m_ai->vfAI_0x3C();
        } else if (m_ai->m_body->vf_0x0C()) {
            fn_8019B0B0(m_ai->m_enemy, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
            m_ai->m_step++;
        }
        break;
    }
    case 2:
        if (m_ai->m_body->vf_0x10()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vfPara_0x58(), true);
            m_ai->m_step = 0;
        }
        break;
    }
}

void EnemyAIState_GunRobotAI_WeaponAttack::Exit() {}
