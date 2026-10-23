/* EnemyWalkerAI (class name from RTTI; Game/EnemyWalkerAI.h): the AI of the walking enemies
 * (EggPawnAI, GunRobotAI), with the states they share: returning home (walking, dashing), waiting at
 * a point (standing, moving between the points) and caution. File name inferred from the class.
 * Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* See EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_STATES
/* Defines EnemyWalkerAI's destructor here (Game/EnemyWalkerAI.h). */
#define ENEMY_WALKER_AI_UNIT

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/EnemyAIState.h"
#include "Game/EnemyWalker.h"
#include "Game/EnemyWalkerAI.h"
#include "Game/MathTypes.h"

extern "C" {
/* v turned about y by angle (see GunRobot.cpp). */
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
/* Turns enemy toward target (behaviour inferred; Game/EggPawnAI.cpp passes it a target id). */
void fn_8019B0B0(Enemy *enemy, const Vec3f *target);
/* Whether the walking enemy has to react to its target first (behaviour inferred). */
bool fn_8019AF04(EnemyWalker *body);
}

/* The vector type's out-of-line assignment, called with a temporary the way the original's
 * operator is (name inferred; see JumpBoard.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

EnemyAIState_EnemyWalkerAI_ReturnHome::EnemyAIState_EnemyWalkerAI_ReturnHome() : m_ai(0) {}

void EnemyAIState_EnemyWalkerAI_ReturnHome::SetOwner(EnemyBaseAI *ai) {
    EnemyWalkerAI *owner = dynamic_cast<EnemyWalkerAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyWalkerAI_ReturnHomeDash::EnemyAIState_EnemyWalkerAI_ReturnHomeDash() : m_ai(0) {}

void EnemyAIState_EnemyWalkerAI_ReturnHomeDash::SetOwner(EnemyBaseAI *ai) {
    EnemyWalkerAI *owner = dynamic_cast<EnemyWalkerAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyWalkerAI_WaitAction_Standing::EnemyAIState_EnemyWalkerAI_WaitAction_Standing()
    : m_ai(0) {}

void EnemyAIState_EnemyWalkerAI_WaitAction_Standing::SetOwner(EnemyBaseAI *ai) {
    EnemyWalkerAI *owner = dynamic_cast<EnemyWalkerAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyWalkerAI_WaitAction_Moving::EnemyAIState_EnemyWalkerAI_WaitAction_Moving()
    : m_ai(0) {}

void EnemyAIState_EnemyWalkerAI_WaitAction_Moving::SetOwner(EnemyBaseAI *ai) {
    EnemyWalkerAI *owner = dynamic_cast<EnemyWalkerAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EnemyWalkerAI_Caution::EnemyAIState_EnemyWalkerAI_Caution() : m_ai(0) {}

void EnemyAIState_EnemyWalkerAI_Caution::SetOwner(EnemyBaseAI *ai) {
    EnemyWalkerAI *owner = dynamic_cast<EnemyWalkerAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyWalkerAI::EnemyWalkerAI() {
    m_body = dynamic_cast<EnemyWalker *>(m_enemy->m_disp);
}

EnemyWalkerAI::~EnemyWalkerAI() {}

void EnemyWalkerAI::vfAI_0x44() {
    fn_8019F944(&m_stateMachine, GetCautionState());
}

void EnemyAIState_EnemyWalkerAI_ReturnHome::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
}

void EnemyAIState_EnemyWalkerAI_ReturnHome::Update() {
    if (m_ai->m_body->vf_0x0C()) {
        m_ai->vfAI_0x3C();
    }
}

void EnemyAIState_EnemyWalkerAI_ReturnHome::Exit() {}

void EnemyAIState_EnemyWalkerAI_ReturnHomeDash::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vf_0x254(), true);
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
}

void EnemyAIState_EnemyWalkerAI_ReturnHomeDash::Update() {
    if (m_ai->m_body->vf_0x0C()) {
        m_ai->vfAI_0x3C();
    }
}

void EnemyAIState_EnemyWalkerAI_ReturnHomeDash::Exit() {}

void EnemyAIState_EnemyWalkerAI_WaitAction_Standing::Enter() {
    m_ai->m_step = 0;
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
}

void EnemyAIState_EnemyWalkerAI_WaitAction_Standing::Update() {
    if (fn_8019AF04(m_ai->m_body)) {
        m_ai->vfAI_0x44();
        return;
    }
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_body->vf_0x0C()) {
            m_ai->m_step++;
            Vec3f direction(0.0f, 0.0f, -1.0f);
            AssignVector(direction, fn_8040D0AC(&direction, m_ai->m_enemy->field_0x60.y));
            fn_8019B0B0(m_ai->m_enemy, &direction);
        }
        break;
    case 1:
        if (m_ai->m_body->vf_0x10()) {
            m_ai->m_step++;
            m_ai->SetTimer(m_ai->m_body->vf_0x240(m_ai->m_point));
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vf_0x24C(m_ai->m_point), true);
        }
        break;
    case 2:
        if (m_ai->IsTimeUp()) {
            m_ai->m_step++;
            m_ai->SetTimer(m_ai->m_body->vf_0x240(m_ai->m_point));
            if (m_ai->m_body->vfStat_0x24() != m_ai->m_body->vfPara_0x48()) {
                m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vfPara_0x48(), true);
            }
        }
        break;
    case 3:
        if (m_ai->IsTimeUp()) {
            m_ai->m_step--;
            m_ai->SetTimer(m_ai->m_body->vf_0x240(m_ai->m_point));
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vf_0x24C(m_ai->m_point), true);
        }
        break;
    }
}

void EnemyAIState_EnemyWalkerAI_WaitAction_Standing::Exit() {}

void EnemyAIState_EnemyWalkerAI_WaitAction_Moving::Enter() {
    m_ai->m_step = 0;
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vfPara_0x4C(), true);
    if (m_ai->m_body->vf_0x250() == 3) {
        m_ai->m_point = 3;
    } else {
        m_ai->m_point = 0;
    }
    fn_8017DE10(m_ai->m_enemy,
                reinterpret_cast<const Vec3f *>(&(const Fn8040CopyVector3 &)m_ai->m_body->vf_0x244(m_ai->m_point)));
}

void EnemyAIState_EnemyWalkerAI_WaitAction_Moving::Update() {
    if (fn_8019AF04(m_ai->m_body)) {
        m_ai->vfAI_0x44();
        return;
    }
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_body->vf_0x10()) {
            m_ai->m_step = 1;
            m_ai->SetTimer(2.0f);
        }
        break;
    case 1:
        if (m_ai->m_body->vf_0x0C() || m_ai->IsTimeUp()) {
            m_ai->m_step = 2;
            Vec3f direction(0.0f, 0.0f, -1.0f);
            AssignVector(direction, fn_8040D0AC(&direction, m_ai->m_enemy->field_0x60.y));
            fn_8019B0B0(m_ai->m_enemy, &direction);
        }
        break;
    case 2:
        if (m_ai->m_body->vf_0x10()) {
            m_ai->m_step = 3;
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vf_0x24C(m_ai->m_point), true);
            m_ai->SetTimer(m_ai->m_body->vf_0x240(m_ai->m_point));
        }
        break;
    case 3:
        if (m_ai->IsTimeUp()) {
            switch (m_ai->m_body->vf_0x250()) {
            case 1:
                if (m_ai->m_point == 1) {
                    m_ai->m_point = 0;
                } else {
                    m_ai->m_point = 1;
                }
                break;
            case 2:
                if (m_ai->m_point == 1) {
                    m_ai->m_point = 2;
                } else if (m_ai->m_point == 2) {
                    m_ai->m_point = 0;
                } else {
                    m_ai->m_point = 1;
                }
                break;
            }
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vfPara_0x4C(), true);
            fn_8017DE10(m_ai->m_enemy, reinterpret_cast<const Vec3f *>(
                                           &(const Fn8040CopyVector3 &)m_ai->m_body->vf_0x244(m_ai->m_point)));
            m_ai->m_step = 0;
        } else if (m_ai->m_body->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vf_0x24C(m_ai->m_point), true);
        }
        break;
    }
}

void EnemyAIState_EnemyWalkerAI_WaitAction_Moving::Exit() {}

void EnemyAIState_EnemyWalkerAI_Caution::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x54(), true);
    m_ai->SetTimer(1.0f);
}

void EnemyAIState_EnemyWalkerAI_Caution::Update() {
    fn_8019B0B0(m_ai->m_enemy, reinterpret_cast<const Vec3f *>(fn_8019F65C(&m_ai->m_target)));
    m_ai->m_enemy->vf_0x10();
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->vfAI_0x3C();
    } else if (m_ai->IsTimeUp()) {
        m_ai->vfAI_0x48();
    }
}

void EnemyAIState_EnemyWalkerAI_Caution::Exit() {}

